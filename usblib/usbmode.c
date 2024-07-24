//#############################################################################
// FILE: usbmode.c
// TITLE: Functions related to dual mode USB device/host operation.
//#############################################################################
//
//
// $Copyright:
// Copyright (C) 2013-2023 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_usb.h"
#include "debug.h"
#include "interrupt.h"
#include "sysctl.h"
#include "usb.h"
#include "usblib.h"
#include "usblibpriv.h"
#include "device/usbdevice.h"
#include "usbhost.h"
#include "usbhostpriv.h"
#include "usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup general_usblib_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The following label defines interrupts that we will always pass to the host
// interrupt handler even if we are in dual mode and not yet sure of which
// mode we are operating in.
//
//*****************************************************************************
#define USB_HOST_INTS           (USB_INTCTRL_VBUS_ERR)

//*****************************************************************************
//
// Global variable indicating which mode of operation the application has
// requested.
//
//*****************************************************************************
volatile tUSBMode g_iUSBMode = eUSBModeNone;

//*****************************************************************************
//
// The default and the current polling rate for the USB OTG library.
//
//*****************************************************************************
volatile uint32_t g_ui32PollRate;

//*****************************************************************************
//
// The current time remaining in milliseconds before checking the cable
// connection.
//
//*****************************************************************************
volatile uint32_t g_ui32WaitTicks = 0;

//*****************************************************************************
//
// This enum holds the various states that we can be in while performing
// USB mode checking. This involves use of the OTG session request to poll
// the USB ID pin to determine whether a device or a host is connected.
//
//*****************************************************************************
typedef enum
{
    //
    // No checking is currently pending.
    //
    eUSBOTGModeIdle,

    //
    // Waiting on ID mode detection.
    //
    eUSBOTGModeWaitID,

    //
    // Waiting for next poll interval.
    //
    eUSBOTGModeWait,

    //
    // Now in B-side wait for connect.
    //
    eUSBOTGModeBWaitCon,

    //
    // Now in A-side device mode.
    //
    eUSBOTGModeBDevice,

    //
    // Now in A-side host mode.
    //
    eUSBOTGModeAHost,
}
tUSBOTGState;

volatile tUSBOTGState g_eOTGModeState;

//*****************************************************************************
//
// Global variable indicating whether we are currently operating in host or
// device mode if the user has requested Dual mode operation.
//
//*****************************************************************************
static volatile tUSBMode g_iDualMode = eUSBModeNone;

//*****************************************************************************
//
// Global variable holding a pointer to the callback function which will be
// called when the USB mode changes between device and host.
//
//*****************************************************************************
static tUSBModeCallback g_pfnUSBModeCallback;

//*****************************************************************************
//
//! Allows dual mode application to switch between USB device and host modes
//! and provides a method to force the controller into the desired mode.
//!
//! \param ui32Index specifies the USB controller whose mode of operation is to
//! be set.  This parameter must be set to 0.
//! \param iUSBMode indicates the mode that the application wishes to operate
//! in.  Valid values are \b eUSBModeDevice to operate as a USB device and
//! \b eUSBModeHost to operate as a USB host.
//! \param pfnCallback is a pointer to a function which the USB library will
//! call each time the mode is changed to indicate the new operating mode.  In
//! cases where \e iUSBMode is set to either \b eUSBModeDevice or
//! \b eUSBModeHost, the callback will be made immediately to allow the
//! application to perform any host or device specific initialization.
//!
//! This function allows a USB application that can operate in host
//! or device mode to indicate to the USB stack the mode that it wishes to
//! use.  The caller is responsible for cleaning up the interface and removing
//! itself from the bus prior to making this call and reconfiguring afterwards.
//! The \e pfnCallback function can be a NULL(0) value to indicate that no
//! notification is required.
//!
//! For successful dual mode mode operation, an application must register
//! USB0DualModeIntHandler() as the interrupt handler for the USB0 interrupt.
//! This handler is responsible for steering interrupts to the device or host
//! stack depending upon the chosen mode.  Devices which do not require dual
//! mode capability should register either \e USB0DeviceIntHandler() or
//! \e USB0HostIntHandler() instead.  Registering \e USB0DualModeIntHandler()
//! for a single mode application will result in an application binary larger
//! than required since library functions for both USB operating modes will be
//! included even though only one mode is required.
//!
//! Single mode applications (those offering exclusively USB device or USB
//! host functionality) are only required to call this function if they need to
//! force the mode of the controller to Host or Device mode.  This is usually
//! in the event that the application needs to reused the USBVBUS and/or USBID
//! pins as GPIOs.
//!
//! \return None.
//
//*****************************************************************************
void
USBStackModeSet(uint32_t ui32Index, tUSBMode iUSBMode,
                tUSBModeCallback pfnCallback)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Index == 0);

    //
    // Remember the mode so that we can steer the interrupts appropriately.
    //
    g_iUSBMode = iUSBMode;

    //
    // Remember the callback pointer.
    //
    g_pfnUSBModeCallback = pfnCallback;

    //
    // If we are being asked to be either a host or device, we will not be
    // trying to auto-detect the mode so make the callback immediately.
    //
    if((iUSBMode == eUSBModeDevice) || (iUSBMode == eUSBModeHost))
    {
        //
        // Make sure that a callback was provided.
        //
        if(g_pfnUSBModeCallback)
        {
            g_pfnUSBModeCallback(0, iUSBMode);
        }
    }
}
//*****************************************************************************
//
// Close the Doxygen group general_usblib_api.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup dualmode_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Returns the USB controller to the default mode when in dual mode operation.
//!
//! \param ui32Index specifies the USB controller whose dual mode operation is
//! to be ended.  This parameter must be set to 0.
//!
//! Applications using both host and device modes may call this function to
//! disable interrupts in preparation for shutdown or a change of operating
//! mode.
//!
//! \return None.
//
//*****************************************************************************
void
USBDualModeTerm(uint32_t ui32Index)
{
    //
    // We only support a single USB controller.
    //
    ASSERT(ui32Index == 0);

    //
    // Disable the USB interrupt.
    //
    Interrupt_disable(g_psDCDInst[0].ui32IntNum);

    USBIntDisableControl(USB_BASE, USB_INTCTRL_ALL);

    USBIntDisableEndpoint(USB_BASE, USB_INTEP_ALL);
}

//*****************************************************************************
//
// Close the Doxygen group dualmode_api.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup usblib_otg
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Returns the USB controller to and inactive state when in OTG mode
//! operation.
//!
//! \param ui32Index specifies the USB controller to end OTG mode operations.
//!
//! Applications using OTG mode may call this function to disable interrupts
//! in preparation for shutdown or a change of operating mode.
//!
//! \return None.
//

//*****************************************************************************
//
//! Initializes the USB controller for OTG mode operation.
//!
//! \param ui32Index specifies the USB controller that is to be initialized for
//! OTG mode operation.
//! \param ui32PollingRate is the rate in milliseconds to poll the controller
//! for changes in mode.
//! \param pvPool is a pointer to the data to use as a memory pool for this
//! controller.
//! \param ui32PoolSize is the size in bytes of the buffer passed in as
//! \e pvPool.
//!
//! This function initializes the USB controller hardware into a state
//! suitable for OTG mode operation.  Applications must use this function to
//! ensure that the controller is in a neutral state and able to receive
//! appropriate interrupts before host or device mode is chosen by OTG
//! negotiation.  The \e ui32PollingRate parameter is used to set the rate at
//! which the USB library will poll the controller to determine the mode.  This
//! has the most effect on how quickly the USB library will detect changes when
//! going to host mode.  The parameters \e pvPool and \e ui32PoolSize are
//! passed on to the USB host library functions to provide memory for the USB
//! library when it is acting as a  host. Any device and host initialization
//! should have been called before calling this function to prevent the USB
//! library from attempting to run in device or host mode before the USB
//! library is fully configured.
//!
//! \return None.
//
//*****************************************************************************

//*****************************************************************************
//
// This function handles the steps required to remove power in OTG mode.
//
// \param ui32Index specifies which USB controller should remove power.
//
// This function will perform the steps required to remove power from the USB
// bus as required by the OTG specification.  This call will first issue a
// bus suspend followed by clearing the current session and then removing
// power.
//
// \return None.
//

//*****************************************************************************
//
//! This call sets the USB OTG controllers poll rate when checking for the mode
//! of the controller.
//!
//! \param ui32Index specifies which USB controller to set the polling rate.
//! \param ui32PollRate is the rate in milliseconds to poll for changes in the
//! controller mode.
//!
//! This function is called to set the USB OTG libraries polling rate when
//! checking the status of the cable.  The \e ui32PollRate value used sets the
//! rate in milliseconds that the USB OTG library will poll the cable to see
//! if the controller should enter host mode.  This value has no effect on
//! device detection rate as the controller will detect being connected to a
//! host controller automatically.  The \e ui32PollRate can be set to 0 to
//! disable polling.  The USB OTG library can still function with the polling
//! rate set to zero, however it will fail to detect host mode properly when no
//! device is present at the end of the USB OTG B side of the cable.
//!
//! \note This function should only be called on devices that support OTG
//! functionality.
//!
//! \return None.
//
//*****************************************************************************
void
USBOTGPollRate(uint32_t ui32Index, uint32_t ui32PollRate)
{
    //
    // Save the timeout.
    //
    g_ui32PollRate = ui32PollRate;
}

//*****************************************************************************
//
//! Handles OTG mode changes and also steers other USB interrupts from
//! the controller to the correct handler in the USB stack.
//!
//! This interrupt handler is used in applications which require to operate
//! in both host and device mode using OTG.  When in host or device mode, it
//! steers the USB hardware interrupt to the correct handler in the USB stack
//! depending upon the current operating mode.  It also handles other OTG
//! related interrupt events.
//!
//! For successful OTG mode operation, an application must register
//! USB0OTGModeIntHandler() in the CPU vector table as the interrupt handler
//! for the USB0 interrupt.
//!
//! \note This interrupt handler should only be used on controllers that
//! support OTG functionality.
//!
//! \return None.
//
//*****************************************************************************

//*****************************************************************************
//
// This function is called by the USB host stack code to indicated that it
// has completed handing the device disconnection.
//
// \param ui32Index specifies the USB controller that has completed disconnect.
//
// This internal library function is used when the hsot controller has
// completed any deferred handling when it has detected a device has been
// disconnected.  The functions main purpose is to return the OTG controller to
// a state that allows for resuming normal OTG cable detection and negotiation.
//
// \note This function should not be called outside the library.
//
//*****************************************************************************
//
//! This function is the main routine for the OTG Controller Driver.
//!
//! \param ui32MsTicks is the number of milliseconds that have passed since the
//! last time this function was called.
//!
//! This function is the main routine for the USB controller when using the
//! library in OTG mode.  This routine must be called periodically by the main
//! application outside of a callback context.  The \e ui32MsTicks value is
//! used for basic timing needed by the USB library when operating in OTG mode.
//! This allows for a simple cooperative system to access the the OTG
//! controller driver interface without the need for an RTOS.  All time
//! critical operations are handled in interrupt context but all longer
//! operations are run from the this function to allow them to block and wait
//! for completion without holding off other interrupts.
//!
//! \return None.
//

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
