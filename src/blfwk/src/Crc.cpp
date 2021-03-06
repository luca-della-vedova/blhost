/*
* Copyright (c) 2013-15, Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "blfwk/Crc.h"
#include "bootloader/bootloader.h"
#include "crc/crc32.h"

using namespace blfwk;

// See Crc.h for documentation of this method.
uint32_t Crc::calculate_application_crc32(const uint8_t *start, uint32_t length)
{
    uint32_t crc32;

    // Initialize the CRC32 information
    crc32_data_t crcInfo;
    crc32_init(&crcInfo);

    // Run CRC, Considering skip crcExpectedValue address
    const bootloader_configuration_data_t *config =
        NULL; // Just used for calculate the offset, must not change the region it points to.
    uint32_t bypassStartAddress = 0x3C0 + ((uint32_t)&config->crcExpectedValue - (uint32_t)&config->tag);
    uint32_t bypassEndAddress = bypassStartAddress + sizeof(config->crcExpectedValue);

    if (length <= bypassStartAddress)
    {
        crc32_update(&crcInfo, (uint8_t *)start, length);
    }
    else
    {
        // Assume that crcExpectedValue address (4 byte) resides in crc addresses completely
        crc32_update(&crcInfo, (uint8_t *)start, bypassStartAddress);
        crc32_update(&crcInfo, (uint8_t *)(start + bypassEndAddress), length - bypassEndAddress);
    }

    // Finalize the CRC calculations
    crc32_finalize(&crcInfo, &crc32);

    return crc32;
}
