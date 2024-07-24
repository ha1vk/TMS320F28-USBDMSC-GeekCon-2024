//*****************************************************************************
//
// usbdmscglue.c - Routines supplied for use by the mass storage class device
// class.
//
// Copyright (c) 2009-2010 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

#include <flash_disk/flashdisk.h>
#include <stdint.h>
#include <usbcfg/usb_structs.h>
#include "inc/hw_types.h"
#include "debug.h"
#include "gpio.h"
#include "interrupt.h"
#include "usblib.h"
#include "usb_ids.h"
#include "usbdevice.h"
#include "usbdmsc.h"
#include "usbdmsc.h"
#define SDCARD_PRESENT          0x00000001
#define SDCARD_IN_USE           0x00000002
struct
{
    unsigned int ulFlags;
}
g_sDriveInformation;

//*****************************************************************************
//
// This function opens the drive number and prepares it for use by the Mass
// storage class device.
//
// /param ulDrive is the driver number to open.
//
// This function is used to initialize and open the physical drive number
// associated with the parameter /e ulDrive.  The function will return zero if
// the drive could not be opened for some reason.  In the case of removable
// device like an SD card this function should return zero if the SD card is
// not present.
//
// /return Returns a pointer to data that should be passed to other APIs or it
// will return 0 if no drive was found.
//
//*****************************************************************************
void *
USBDMSCStorageOpen(unsigned int ulDrive)
{
    //unsigned char ucPower;
    //unsigned int ulTemp;

    ASSERT(ulDrive == 0);
    disk_initialize();
    g_sDriveInformation.ulFlags = SDCARD_PRESENT | SDCARD_IN_USE;

    return((void *)&g_sDriveInformation);
}

//*****************************************************************************
//
// This function close the drive number in use by the mass storage class device.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
//
// This function is used to close the physical drive number associated with the
// parameter /e pvDrive.  This function will return 0 if the drive was closed
// successfully and any other value will indicate a failure.
//
// /return Returns 0 if the drive was successfully closed or non-zero for a
// failure.
//
//*****************************************************************************
void
USBDMSCStorageClose(void * pvDrive)
{

    ASSERT(pvDrive != 0);
    //
    // Clear all flags.
    //
    g_sDriveInformation.ulFlags = 0;

    //
    // Turn off the power to the card.
    //
 }

//*****************************************************************************
//
// This function will read a block from a device opened by the
// USBDMSCStorageOpen() call.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
// /param pucData is the buffer that data will be written into.
// /param ulNumBlocks is the number of blocks to read.
//
// This function is use to read blocks from a physical device and return them
// in the /e pucData buffer.  The data area pointed to by /e pucData should be
// at least /e ulNumBlocks * Block Size bytes to prevent overwriting data.
//
// /return Returns the number of bytes that were read from the device.
//
//*****************************************************************************
uint32_t USBDMSCStorageRead(void * pvDrive,
                                 uint8_t *pucData,
                                 uint32_t ulSector,uint32_t offset,
                                 uint32_t ulNumBlocks)
{
    ASSERT(pvDrive != 0);
    return disk_read(ulSector, pucData,offset,ulNumBlocks);
}

//*****************************************************************************
//
// This function will write a block to a device opened by the
// USBDMSCStorageOpen() call.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
// /param pucData is the buffer that data will be used for writing.
// /param ulNumBlocks is the number of blocks to write.
//
// This function is use to write blocks to a physical device from the buffer
// pointed to by the /e pucData buffer.  If the number of blocks is greater than
// one then the block address will increment and write to the next block until
// /e ulNumBlocks * Block Size bytes have been written.
//
// /return Returns the number of bytes that were written to the device.
//
//*****************************************************************************
uint32_t USBDMSCStorageWrite(void * pvDrive,
                                  uint8_t *pucData,
                                  uint32_t ulSector,uint32_t offset,
                                  uint32_t ulNumBlocks)
{
    ASSERT(pvDrive != 0);
    return disk_write(ulSector, pucData,offset, ulNumBlocks);
}

//*****************************************************************************
//
// This function will return the number of blocks present on a device.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
//
// This function is used to return the total number of blocks on a physical
// device based on the /e pvDrive parameter.
//
// /return Returns the number of blocks that are present in a device.
//
//*****************************************************************************
uint32_t
USBDMSCStorageNumBlocks(void * pvDrive)
{
    unsigned int ulSectorCount = 0;
    //
    // Read the number of sectors.
    //
    disk_ioctl(0, GET_SECTOR_COUNT, &ulSectorCount);

    return (ulSectorCount);
}

uint32_t
USBDMSCStorageBlockSize(void * pvDrive)
{
    unsigned int sector_size = 0;
    //
    // Read the number of sectors.
    //
    disk_ioctl(0, GET_SECTOR_SIZE, &sector_size);

    return (sector_size);
}

//*****************************************************************************
//
// This function will return the current status of a device.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
//
// This function is used to return the current status of the device indicated by
// the /e pvDrive parameter.  This can be used to see if the device is busy,
// or if it is present.
//
// /return Returns the size in bytes of blocks that in a device.
//
//*****************************************************************************
#define USBDMSC_IDLE            0x00000000
#define USBDMSC_NOT_PRESENT     0x00000001
unsigned int USBDMSCStorageStatus(void * pvDrive);
