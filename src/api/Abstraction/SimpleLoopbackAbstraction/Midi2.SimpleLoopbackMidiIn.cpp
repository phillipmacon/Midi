// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================


#include "pch.h"
#include "midi2.SimpleLoopbackabstraction.h"

_Use_decl_annotations_
HRESULT
CMidi2SimpleLoopbackMidiIn::Initialize(
    LPCWSTR,
    DWORD *,
    IMidiCallback * callback
)
{
    TraceLoggingWrite(
        MidiSimpleLoopbackAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
        );


    RETURN_HR_IF_NULL(E_INVALIDARG, callback);
    m_callback = callback;

    // if you're looking for code to emulate for your own transport, this isn't the best
    // example. Why? We completely ignore the device Id and simply return a static device
    // based on the type of endpoint requested.

    m_midiDevice = (MidiLoopbackDevice*)(MidiDeviceTable::Current().GetInOutDevice());
    RETURN_HR_IF_NULL(E_POINTER, m_midiDevice);

    m_midiDevice->SetCallback(this);

    return S_OK;
}

HRESULT
CMidi2SimpleLoopbackMidiIn::Cleanup()
{
    TraceLoggingWrite(
        MidiSimpleLoopbackAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
        );

    m_callback = nullptr;
    m_midiDevice = nullptr;

    return S_OK;
}

_Use_decl_annotations_
HRESULT
CMidi2SimpleLoopbackMidiIn::Callback(
    PVOID message,
    UINT size,
    LONGLONG timestamp
)
{
    if (m_callback == nullptr)
    {
        // TODO log that callback is null
        return E_POINTER;
    }

    if (message == nullptr)
    {
        // TODO log that message was null
        return E_INVALIDARG;
    }

    if (size < sizeof(uint32_t))
    {
        // TODO log that data was smaller than minimum UMP size
        return E_INVALIDARG;
    }

    return m_callback->Callback(message, size, timestamp);
}

