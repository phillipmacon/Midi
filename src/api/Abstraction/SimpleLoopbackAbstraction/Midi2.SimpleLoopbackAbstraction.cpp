// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================


#include "pch.h"

_Use_decl_annotations_
HRESULT
CMidi2SimpleLoopbackAbstraction::Activate(
    REFIID Riid,
    void **Interface
)
{
//    OutputDebugString(L"" __FUNCTION__ " Enter");

    RETURN_HR_IF(E_INVALIDARG, nullptr == Interface);

   if (__uuidof(IMidiIn) == Riid)
    {
       OutputDebugString(L"" __FUNCTION__ " Activating IMidiIn");

        TraceLoggingWrite(
            MidiSimpleLoopbackAbstractionTelemetryProvider::Provider(),
            __FUNCTION__ "- Midi in",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(__FUNCTION__),
            TraceLoggingPointer(this, "this")
            );

        wil::com_ptr_nothrow<IMidiIn> midiIn;
        RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<CMidi2SimpleLoopbackMidiIn>(&midiIn));
        *Interface = midiIn.detach();
    }
    else if (__uuidof(IMidiOut) == Riid)
    {
       OutputDebugString(L"" __FUNCTION__ " Activating IMidiOut");
       
       TraceLoggingWrite(
            MidiSimpleLoopbackAbstractionTelemetryProvider::Provider(),
            __FUNCTION__ "- Midi Out",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(__FUNCTION__),
            TraceLoggingPointer(this, "this")
            );

        wil::com_ptr_nothrow<IMidiOut> midiOut;
        RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<CMidi2SimpleLoopbackMidiOut>(&midiOut));
        *Interface = midiOut.detach();
    }
    else if (__uuidof(IMidiBiDi) == Riid)
    {
       OutputDebugString(L"" __FUNCTION__ " Activating IMidiBiDi");

        TraceLoggingWrite(
            MidiSimpleLoopbackAbstractionTelemetryProvider::Provider(),
            __FUNCTION__ "- Midi BiDi",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(__FUNCTION__),
            TraceLoggingPointer(this, "this")
            );

        wil::com_ptr_nothrow<IMidiBiDi> midiBiDi;
        RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<CMidi2SimpleLoopbackMidiBiDi>(&midiBiDi));
        *Interface = midiBiDi.detach();
    }
    else if (__uuidof(IMidiEndpointManager) == Riid)
    {
        TraceLoggingWrite(
            MidiSimpleLoopbackAbstractionTelemetryProvider::Provider(),
            __FUNCTION__ "- Midi Endpoint Manager",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(__FUNCTION__),
            TraceLoggingPointer(this, "this")
        );

        wil::com_ptr_nothrow<IMidiEndpointManager> midiEndpointManager;
        RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<CMidi2SimpleLoopbackEndpointManager>(&midiEndpointManager));
        *Interface = midiEndpointManager.detach();
    }
    else
    {
        OutputDebugString(L"" __FUNCTION__ " Returning E_NOINTERFACE. Was an interface added that isn't handled in the Abstraction?");

        return E_NOINTERFACE;
    }

    return S_OK;
}

