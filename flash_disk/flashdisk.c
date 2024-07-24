/**
 * \file  ramdisk.c
 *
 * \brief Basic RAMDISK for USB example application
 *
 */

/*
* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
*/
/*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
#define CPU1 1
#include "F28x_Project.h"
#include "device.h"
#include <string.h>
#include <flash_disk/flashdisk.h>
#include "F021_F2837xD_C28x.h"

#define Bzero_64KSector_u32length   0x4000
#define Bzero_16KSector_u32length   0x1000

//320KB
#define RAM_DISK_SIZE 0x50000
#define SECTOR_SIZE 0x8000
//#define BLOCK_SIZE 0x2000
#define BLOCK_SIZE 0x1000
#define TRANSFER_SIZE 64U
#define MULT (SECTOR_SIZE / BLOCK_SIZE * 2)

#pragma DATA_SECTION(sector_buffer, "FLASH_SECTOR_CACHE");
uint16_t sector_buffer[SECTOR_SIZE];
uint16_t *ram_disk = (uint16_t *)0x090000;
//存放密码
uint16_t *usb_password = (uint16_t *)0x0B8000;
extern bool usb_unlocked;

char* memmem(char* haystack, uint32_t hlen,char* needle, uint32_t nlen) {
	char* cur;
	char* last;
	uint32_t i;
	last =  haystack + hlen - nlen;
	for (cur = haystack; cur <= last;cur++) {
        for (i = 0; i < nlen; i++) {
            if (cur[i] != needle[i])
                break;
        }
        if (i == nlen) {
            return cur;
        }
    }
	return 0;
}

inline void Example_Error(Fapi_StatusType status)
{
    __asm("    ESTOP0");
}

//
// Init_Flash_Sectors - Initialize flash API and active flash bank sectors
//
void Init_Flash_Sectors(void)
{
    EALLOW;
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0x0;
    Fapi_StatusType oReturnCheck;

    oReturnCheck = Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, 120);

    if(oReturnCheck != Fapi_Status_Success)
    {
        Example_Error(oReturnCheck);
    }

    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);

    if(oReturnCheck != Fapi_Status_Success)
    {
        Example_Error(oReturnCheck);
    }
}

void disk_initialize(void)
{
    Init_Flash_Sectors();
    uint64_t magic = 0x3a4b4b43304c4e55;
    char *password_in_disk = memmem(ram_disk,RAM_DISK_SIZE/2,&magic,4);
    if (password_in_disk) {
        usb_unlocked = verify_password(password_in_disk);
    } else if (*usb_password == 0xFFFF) {
        usb_unlocked = true;
    }
}

unsigned int disk_read(uint32_t lba, uint16_t *buf,
                       uint32_t off,uint32_t len)
{
    uint32_t start,i;

    start = lba * BLOCK_SIZE+off;
    len = len * TRANSFER_SIZE;

    if (!usb_unlocked) {
        memset(buf,0,len);
        return len;
    }
    if (start + len <= RAM_DISK_SIZE )
    {
        for (i=0;i<len;i+=2) {
            uint16_t data = ram_disk[(start+i)/2];
            buf[i] = data & 0xFF;
            buf[i+1] = data >> 8;
        }
    }
    return len;
}
void set_usb_password(uint16_t *password) {
    EALLOW;
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0x0;
    uint16_t buf[0x20];
    uint32_t u32Index, i;
    Fapi_StatusType oReturnCheck = Fapi_Status_Success;
    Fapi_FlashStatusWordType  oFlashStatusWord ;
    volatile Fapi_FlashStatusType oFlashStatus = 0;

    if (*usb_password != 0xFFFF) {
        // Erase Sector
        oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,
            (uint32 *)usb_password);
        //
        // Wait until FSM is done with erase sector operation.
        //
        while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady)
        {

        }
        //
        // Verify that Sector is erased. The erase step itself does verification
        // as it goes. This verify is a second verification that can be done.
        //
        oReturnCheck = Fapi_doBlankCheck((uint32 *)usb_password,
            Bzero_16KSector_u32length,
            &oFlashStatusWord);

        if(oReturnCheck != Fapi_Status_Success)
        {
            //
            // Check Flash API documentation for possible errors.
            // If erase command fails, use Fapi_getFsmStatus() function to get the
            // FMSTAT register contents to see if any of the EV bit, ESUSP bit,
            // CSTAT bit or VOLTSTAT bit is set. Refer to API documentation for
            // more details.
            //
            Example_Error(oReturnCheck);
        }
    }
    memset(buf,0,0x20);
    int len = strlen(password) + 1;
    if (len > 0x20) len = 0x20;
    memcpy(buf,password,len-1);

    for (i = 0, u32Index = (uint32_t)usb_password;
         (u32Index < ((uint32_t)usb_password + 0x20)) && (oReturnCheck == Fapi_Status_Success);
         i += 8, u32Index += 8) {
        oReturnCheck = Fapi_issueProgrammingCommand((uint32 *)u32Index,buf + i,
                                                    8,
                                                    0,
                                                    0,
                                                    Fapi_AutoEccGeneration);

        //
        // Wait until FSM is done with program operation.
        //
        while (Fapi_checkFsmForReady() == Fapi_Status_FsmBusy)
        {
        }

        if (oReturnCheck != Fapi_Status_Success)
        {
            //
            // Check Flash API documentation for possible errors.
            //
            Example_Error(oReturnCheck);
        }
        oFlashStatus = Fapi_getFsmStatus();

        oReturnCheck = Fapi_doVerify((uint32 *)u32Index,
                                     4,
                                     (uint32_t *)(buf + i),
                                     &oFlashStatusWord);
        if (oReturnCheck != Fapi_Status_Success)
        {
            //
            // Check Flash API documentation for possible errors.
            //
            // Example_Error(oReturnCheck);
            __asm("    ESTOP0");
        }
    }
}
unsigned int disk_write(uint32_t lba, uint16_t *buf,
                        uint32_t off,uint32_t len)
{
    static uint16_t erase = 0;
    uint32_t u32Index, start,i;
    Fapi_StatusType oReturnCheck = Fapi_Status_Success;
    Fapi_FlashStatusWordType  oFlashStatusWord ;
    volatile Fapi_FlashStatusType oFlashStatus = 0;

    start = lba * BLOCK_SIZE + off;
    len = len * TRANSFER_SIZE;

    if (!usb_unlocked) {
        if (off + len == BLOCK_SIZE)
            usb_unlocked = verify_password(buf);
        goto end;
    }

    EALLOW;
    DcsmCommonRegs.FLSEM.all = 0xA501;
    EDIS;

    //设置USB密码
    if (!strncmp((char *)buf,"UNL0CKK:",8)) {
        set_usb_password((char *)(buf+8));
    }
    if (start + len <= RAM_DISK_SIZE)
    {
        start = start / 2;
        uint32_t lsector = start / SECTOR_SIZE;
        start = start % SECTOR_SIZE;
        uint16_t *sector_begin = ram_disk + lsector * SECTOR_SIZE;

        //off = off % BLOCK_SIZE;
        if (off == 0) {
            //拷贝整个sector
            memcpy(sector_buffer,sector_begin,SECTOR_SIZE);
            //检查需要写入的区域是否已经格式化
            for (i=0;i<BLOCK_SIZE/2;i++) {
                uint16_t x = sector_begin[start + i];
                if (x != 0xFFFF) {
                    erase = 1;
                    break;
                }
            }
        }
        //更新指定block区的数据
        for (i=0;i<len;i+=2) {
            uint16_t data1 = buf[i] & 0xFF;
            uint16_t data2 = buf[i+1] & 0xFF;
            sector_buffer[start+i/2] = data1 | (data2 << 8);
        }
        //向flash写入数据
        if (off + len == BLOCK_SIZE) {
            EALLOW;
            Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0x0;
            //如果需要erase
            if (erase) {
                // Erase Sector
                oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,
                    (uint32 *)sector_begin);
                //
                // Wait until FSM is done with erase sector operation.
                //
                while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady)
                {

                }
                
                //
                // Verify that Sector is erased. The erase step itself does verification
                // as it goes. This verify is a second verification that can be done.
                //
                oReturnCheck = Fapi_doBlankCheck((uint32 *)sector_begin,
                    Bzero_64KSector_u32length,
                    &oFlashStatusWord);
                    
                if(oReturnCheck != Fapi_Status_Success)
                {
                    //
                    // Check Flash API documentation for possible errors.
                    // If erase command fails, use Fapi_getFsmStatus() function to get the
                    // FMSTAT register contents to see if any of the EV bit, ESUSP bit,
                    // CSTAT bit or VOLTSTAT bit is set. Refer to API documentation for
                    // more details.
                    //
                    Example_Error(oReturnCheck);
                }
            }

            for (i = 0, u32Index = (uint32_t)sector_begin;
                    (u32Index < ((uint32_t)sector_begin + SECTOR_SIZE)) && (oReturnCheck == Fapi_Status_Success);
                    i += 8, u32Index += 8) {
                oReturnCheck = Fapi_issueProgrammingCommand((uint32 *)u32Index, sector_buffer+i,
                                                            8,
                                                            0,
                                                            0,
                                                            Fapi_AutoEccGeneration);

                //
                // Wait until FSM is done with program operation.
                //
                while (Fapi_checkFsmForReady() == Fapi_Status_FsmBusy)
                {
                }

                if (oReturnCheck != Fapi_Status_Success) {
                    //
                    // Check Flash API documentation for possible errors.
                    //
                    Example_Error(oReturnCheck);
                }
                oFlashStatus = Fapi_getFsmStatus();

                oReturnCheck = Fapi_doVerify((uint32 *)u32Index,
                                             4,
                                             (uint32_t *)(sector_buffer+i),
                                             &oFlashStatusWord);
                if (oReturnCheck != Fapi_Status_Success) {
                    //
                    // Check Flash API documentation for possible errors.
                    //
                    //Example_Error(oReturnCheck);
                    __asm("    ESTOP0");
                }
            }
        }
    }
    DcsmCommonRegs.FLSEM.all = 0xA500;
    EDIS;
    end:
    return len;
}

void disk_ioctl (unsigned int drive, unsigned int  command,  unsigned int *buffer)
{
    switch(command)
    {

        case GET_SECTOR_COUNT:
        {
            *buffer = (RAM_DISK_SIZE / BLOCK_SIZE);
            break;
        }
        case GET_SECTOR_SIZE:
        {
           *buffer = BLOCK_SIZE;

            break;
        }
        default:
        {
            buffer = 0;
            break;
        }
    }
}

int verify_password(char *password) {
    int i;
    if (*usb_password == 0xFFFF) { //无密码
        return true;
    }
    if (!strncmp(password,"UNL0CKK:",8) && !strcmp(password+8,usb_password)) {
        return true;
    }
    if (*(uint64_t *)password == 0x3a4b4b43304c4e55) {
        int len = strlen(usb_password);
        password = password + 4;
        for (i=0;i<len;i+=2) {
            uint16_t x = password[i/2];
            if (usb_password[i] != (x & 0xFF) || usb_password[i+1] != (x >> 8)) {
                return false;
            }
        }
        return true;
    }
    return false;
}
