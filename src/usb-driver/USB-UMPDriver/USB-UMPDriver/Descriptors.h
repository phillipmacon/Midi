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
    UINT8   baAssoGrpTrmBlkID[];// Array of Grp Terminal Block IDs (MIDI2) or Jacks (MIDI1-virtual cables)
} USBMIDI_CSMSENDPOINT, * PUSBMIDI_CSMSENDPOINT;

//
// USB MIDI 1.0 Jack Descriptors
//
#define USBMIDI_JACK_TYPE_EMBEDDED 0x01
#define USBMIDI_JACK_DESCRPTOR_TYPE USBMIDI_CSMSINTERFACE_TYPE

// IN Jack
#define USBMIDI_MIDIIN_JACK_SUBTYPE 0x02
#pragma pack(1)
typedef struct _USBMIDI_MIDIIN_JACK
{
    UINT8   bLength;            // size of descriptor in bytes - 6
    UINT8   bDescriptorType;    // CS_INTERFACE descrptor type - 0x24
    UINT8   bDescriptorSubtype; // MIDI_IN_JACK descrptor type - 0x02
    UINT8   bJackType;          // EMBEDDED or EXTERNAL - only care about embedded 0x01
    UINT8   bJackID;            // Constant uniquily identifying MIDI IN Jack
    UINT8   iJack;              // Index to string descriptor
} USBMIDI_MIDIIN_JACK, *PUSBMIDI_MIDIIN_JACK;

// OUT Jack
#define USBMIDI_MIDIOUT_JACK_SUBTYPE 0x03
#pragma pack(1)
typedef struct _USBMIDI_MIDIOUT_JACK
{
    UINT8   bLength;            // size of descriptor in bytes - 6
    UINT8   bDescriptorType;    // CS_INTERFACE descrptor type - 0x24
    UINT8   bDescriptorSubtype; // MIDI_OUT_JACK descrptor type
    UINT8   bJackType;          // EMBEDDED or EXTERNAL - only care about embedded 0x01
    UINT8   bJackID;            // Constant uniquily identifying MIDI IN Jack
    UINT8   iJack;              // Index to string descriptor
    UINT8   baPinInfo[];        // Array to Pin information
} USBMIDI_MIDIOUT_JACK, * PUSBMIDI_MIDIOUT_JACK;

//
// USB MIDI 2.0 Group Terminal Block Descriptors
//
#define USBMIDI_CS_GR_TRM_BLOCK 0x26

#define USBMIDI_GS_TRM_BLOCK_HEADER_SUBTYPE 0x01
#pragma pack(1)
typedef struct _USBMIDI_GS_TRM_BLOCK_HEADER
{
    UINT8   bLength;            // Size of descriptor - 5
    UINT8   bDescriptorType;    // CS_GR_TRM_BLOCK - 0x26
    UINT8   bDescriptorSubtype; // GS_TRM_BLOCK_HEADER - 1
    UINT16  wTotalLength;       // Total number of bytes returned for class specific GTB
} USBMIDI_GS_TRM_BLOCK_HEADER, * PUSBMIDI_GS_TRM_BLOCK_HEADER;

#define USBMIDI_GR_TRM_BLOCK_SUBTYPE 0x02
#pragma pack(1)
typedef struct _USBMIDI_GR_TRM_BLOCK
{
    UINT8   bLength;            // Size of descrptor - 13
    UINT8   bDescriptorType;    // CS_GR_TRM_BLOCK - 0x26
    UINT8   bDescriptorSubtype; // GR_TRM_BLOCK - 2
    UINT8   bGrpTrmBlkID;       // ID of this Group Terminal Block
    UINT8   bGrpTrmBlkType;     // Group Terminal Block type
    UINT8   nGroupTrm;          // First member of Group Terminals spanned
    UINT8   nNumGroupTrm;       // Number of Group Terminals spanned
    UINT8   iBlockItem;         // String ID for GTB
    UINT8   bMIDIProtocol;      // Default MIDI Protocol
    UINT16  wMaxInputBandwidth; // Maximum input bandwidth capability
    UINT16  wMaxOutputBandwidth;// Maximum output bandwidth capability
};

typedef enum
{
    GTBT_BIDIRECTIONAL = 0,     // bidirectional - has mathching IN/OUT pair
    GTBT_INONLY,                // input only
    GTBT_OUTONLY,               // output only
} eGrpTrmBlkType;

typedef enum
{
    MIDI_PROTOCOL_Unknown = 0,      // unkown, use MIDI-CI
    MIDI_PROTOCOL_MIDI1 = 1,        // MIDI 1.0, support up to 64 bit size (default)
    MIDI_PROTOCOL_MIDI1_JR = 2,     // MIDI 1.0 using Jitter Reduction
    MIDI_PROTOCOL_MIDI1_128 = 3,    // MIDI 1.0, support up to 128 bits in size
    MIDI_PROTOCOL_MIDI1_128_JR = 4, // MIDI 1.0, up to 128 bits using Jitter Reduction
    MIDI_PROTOCOL_MIDI2 = 0x11,     // MIDI 2.0
    MIDI_PROTOCOL_MIDI2_JR = 0x12,  // MIDI 2.0 using Jitter Reduction
} eMIDIProtocol;

