// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once

class MidiLoopbackBidiDevice
{
public:
    void SetCallbackA(_In_ IMidiCallback* callback);
    void SetCallbackB(_In_ IMidiCallback* callback);

    HRESULT SendMidiMessageFromAToB(
        _In_ void*,
        _In_ UINT32,
        _In_ LONGLONG);

    HRESULT SendMidiMessageFromBToA(
        _In_ void*,
        _In_ UINT32,
        _In_ LONGLONG);


    MidiLoopbackBidiDevice();
    ~MidiLoopbackBidiDevice();

private:
    IMidiCallback* m_callbackA;
    IMidiCallback* m_callbackB;

};