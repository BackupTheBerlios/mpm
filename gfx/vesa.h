/*
 * Copyright (c) 2006, Ivo van Poorten
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * My name may NOT be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef VESA_H
#define VESA_H

#include <stdint.h>

/* ACK does not support packed attributes. Having uint32_t inside a struct
 * causes some fields to be padded, which is wrong, hence I changed the code
 * to use a single buffer and macro's to access the values */

typedef uint8_t vesa_info_t[256];
typedef uint8_t vesa_oem_data_t[256]; /* VBE2 only */

#define VESA_STRING(x, y)           (&x[y])
#define VESA_BYTE(x, y)             (x[y])
#define VESA_WORD(x, y)             (x[y] + (x[y+1] << 8))
#define VESA_PTR(x, y)              (VESA_WORD(x,y) + (VESA_WORD(x,y+2)<<16))

#define VI_SIGNATURE(x)             VESA_STRING(x, 0x00)
#define VI_VERSION(x)               VESA_WORD(x,   0x04)
#define VI_OEMSTRINGPTR(x)          VESA_PTR(x,    0x06)
#define VI_CAPABILITIES(x)          VESA_STRING(x, 0x0a)
#define VI_VIDEOMODEPTR(x)          VESA_PTR(x,    0x0e)
#define VI_TOTALMEMORY(x)           VESA_WORD(x,   0x12)
#define VI_VBE2_OEMSOFTWAREREV(x)   VESA_WORD(x,   0x14)
#define VI_VBE2_OEMVENDORNAMEPTR(x) VESA_PTR(x,    0x16)
#define VI_VBE2_PRODUCTNAMEPTR(x)   VESA_PTR(x,    0x1a)
#define VI_VBE2_PRODUCTREVPTR(x)    VESA_PTR(x,    0x1e)

#if 0
typedef struct vesa_mode_info_s {
    uint16_t    ModeAttributes;
    uint8_t     WinAAttributes;
    uint8_t     WinBAttributes;
    uint16_t    WinGranularity;
    uint16_t    WinSize;
    uint16_t    WinASegment;
    uint16_t    WinBSegment;
    uint32_t    WinFuncPtr;
    uint16_t    BytesPerScanLine;
    struct optional {   /* mandatory from vbe 1.2 and onwards */
        uint16_t    XResolution;
        uint16_t    YResolution;
        uint8_t     XCharSize;
        uint8_t     YCharSize;
        uint8_t     NumberOfPlanes;
        uint8_t     BitsPerPixel;
        uint8_t     NumberOfBanks;
        uint8_t     MemoryModel;
        uint8_t     BankSize;
        uint8_t     NumberOfImagePages;
        uint8_t     Reserved1;
    } optional;
    struct vbe12 {
        uint8_t     RedMaskSize;
        uint8_t     RedFieldPosition;
        uint8_t     GreenMaskSize;
        uint8_t     GreenFieldPosition;
        uint8_t     BlueMaskSize;
        uint8_t     BlueFieldPosition;
        uint8_t     RsvdMaskSize;
        uint8_t     DirectColorModeInfo;
    } vbe12;
    struct vbe20_s2 {
        uint32_t    PhysBasePtr;
        uint32_t    OffScreenMemOffset;
        uint16_t    OffScreenMemSize;   /* in 1k units */
    } vbe20;
    uint8_t     Reserved2[207];
} vesa_mode_info_t;
#endif

#endif
