//##############################################################################
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
//##############################################################################

//
// Included Files
//
#include <usbcfg/board.h>
#include <usbcfg/usb_structs.h>
#include "usb_hal.h"
#include "usblib.h"
#include "usb_ids.h"
#include "device/usbdevice.h"
#include "device/F2837xD_device.h"

volatile enum
{
    //
    // Unconfigured.
    //
    MSC_DEV_DISCONNECTED,

    //
    // Connected but not yet fully enumerated.
    //
    MSC_DEV_CONNECTED,

    //
    // Connected and fully enumerated but not currently handling a command.
    //
    MSC_DEV_IDLE,

    //
    // Currently reading the SD card.
    //
    MSC_DEV_READ,

    //
    // Currently writing the SD card.
    //
    MSC_DEV_WRITE
}
g_eMSCState;

static unsigned int g_ulFlags;
static unsigned int g_ulIdleTimeout;

#define USBMSC_ACTIVITY_TIMEOUT 30
#define FLAG_UPDATE_STATUS      1

unsigned int
USBDMSCEventCallback(void *pvCBData, unsigned int ulEvent,
                     unsigned int ulMsgParam, void *pvMsgData)
{
    //
    // Reset the time out every time an event occurs.
    //
    g_ulIdleTimeout = USBMSC_ACTIVITY_TIMEOUT;

    switch(ulEvent)
    {
        //
        // Writing to the device.
        //
        case USBD_MSC_EVENT_WRITING:
        {
            //
            // Only update if this is a change.
            //
            if(g_eMSCState != MSC_DEV_WRITE)
            {
                //
                // Go to the write state.
                //
                g_eMSCState = MSC_DEV_WRITE;

                //
                // Cause the main loop to update the screen.
                //
                g_ulFlags |= FLAG_UPDATE_STATUS;
            }

            break;
        }

        //
        // Reading from the device.
        //
        case USBD_MSC_EVENT_READING:
        {
            //
            // Only update if this is a change.
            //
            if(g_eMSCState != MSC_DEV_READ)
            {
                //
                // Go to the read state.
                //
                g_eMSCState = MSC_DEV_READ;

                //
                // Cause the main loop to update the screen.
                //
                g_ulFlags |= FLAG_UPDATE_STATUS;
            }

            break;
        }
        case USBD_MSC_EVENT_IDLE:
        default:
        {
            break;
        }
    }

    return(0);
}

//******************************************************************************
// ModeCallback - USB Mode callback
//
// \param ui32Index is the zero-based index of the USB controller making the
//        callback.
// \param eMode indicates the new operating mode.
//
// This function is called by the USB library whenever an OTG mode change
// occurs and, if a connection has been made, informs us of whether we are to
// operate as a host or device.
//
// \return None.
//
//******************************************************************************
void ModeCallback(uint32_t ui32Index, tUSBMode eMode) {

}
//******************************************************************************
//
// Main
//
//******************************************************************************
int
main(void) {
    DCSM_secureZone1();
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Initialize GPIO and configure GPIO pins for USB.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();
    
    Board_init();

    //
    // Initialize the USB stack mode and pass in a mode callback.
    //
    USBStackModeSet(0, eUSBModeForceDevice, ModeCallback);
    USBDMSCInit(0, &g_sMSCDevice);

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    //
    // Enable the GPIO peripheral used for USB, and configure the USB
    // pins.
    //
    USBGPIOEnable();

    Interrupt_enableMaster();

    while(1)
    {
        switch(g_eMSCState)
        {
            case MSC_DEV_READ:
            {
                //
                // Update the screen if necessary.
                //
                if(g_ulFlags & FLAG_UPDATE_STATUS)
                {
                    //UpdateStatus("Reading", 0);
                    g_ulFlags &= ~FLAG_UPDATE_STATUS;
                }

                //
                // If there is no activity then return to the idle state.
                //
                if(g_ulIdleTimeout == 0)
                {
                    //UpdateStatus("Idle     ", 0);
                    g_eMSCState = MSC_DEV_IDLE;
                }

                break;
            }
            case MSC_DEV_WRITE:
            {
                //
                // Update the screen if necessary.
                //
                if(g_ulFlags & FLAG_UPDATE_STATUS)
                {
                    //UpdateStatus("Writing ", 0);
                    g_ulFlags &= ~FLAG_UPDATE_STATUS;
                }

                //
                // If there is no activity then return to the idle state.
                //
                if(g_ulIdleTimeout == 0)
                {
                    //UpdateStatus("Idle     ", 0);
                    g_eMSCState = MSC_DEV_IDLE;
                }
                break;
            }
            case MSC_DEV_IDLE:
            default:
            {
                break;
            }
        }
    }
}

//******************************************************************************
//
//! Device interrupt service routine wrapper to make ISR compatible with
//! C2000 PIE controller.
//
//******************************************************************************
__interrupt void
INT_myUSB0_ISR(void)
{
    USB0DeviceIntHandler();
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//
// End of file
//
