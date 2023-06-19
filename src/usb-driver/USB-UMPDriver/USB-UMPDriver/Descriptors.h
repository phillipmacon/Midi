/************************************************************************************
Copyright 2023 Association of Musical Electronics Industry
Copyright 2023 Microsoft
Driver source code developed by AmeNote. Some components Copyright 2023 AmeNote Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
************************************************************************************/
/*++

Module Name:

    Descriptors.h

Abstract:

    This file contains the structure definitions for the class specific desciptors.

Environment:

    Kernel-mode Driver Framework

--*/

#pragma once

//
// Class-Specific MS Interface Header Descriptor
//
#define USBMIDI_CSMSINTERFACE_TYPE 0x24
#pragma pack(1)
typedef struct _USBMIDI_CSMSINTERFACE
{
    UINT8   bLength;            // size descriptor in bytes - 7
    UINT8   bDescriptorType;    // CS_INTERFACE descriptor - 0x24
    UINT8   bDescriptorSubtype; // MS_HEADER subtype - 0x01
    USHORT  bcdMSC;             // MS_MIDI 2.0 0x0200 or MS_MIDI 1.0 0x0100
    USHORT  wTotalLength;       // Total size of class specific descriptors
                                //  not used in MIDI2, so is set to 7
} USBMIDI_CSMSINTERFACE, *PUSBMIDI_CSMSINTERFACE;

//
// Class-Specific Endpoint Descriptors
//
#define USBMIDI_CSMSENDPOINT_TYPE 0x25
#pragma pack(1)
typedef struct _USBMIDI_CSMSENDPOINT
{
    UINT8   bLength;            // size descriptor in bytes - >= 7 (depends on number of jacks / GTBs
    UINT8   bDescriptorType;    //  CS_ENDPOINT descriptor - 0x25
    UINT8   bDescriptorSubtype; // MS_GENERAL subtype - 0x01
    UINT8   bNumGrpTrmBlock;    // Number of group terminal blocks (MIDI2) or Jacks (MIDI1)
    UINT8*  baAssoGrpTrmBlkID;  // Array of Grp Terminal Block IDs (MIDI2) or Jacks (MIDI1-virtual cables)
} USBMIDI_CSMSENDPOINT, * PUSBMIDI_CSMSENDPOINT;
