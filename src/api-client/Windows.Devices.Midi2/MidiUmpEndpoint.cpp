#include "pch.h"
#include "MidiUmpEndpoint.h"
#include "MidiUmpEndpoint.g.cpp"


namespace winrt::Windows::Devices::Midi2::implementation
{
    // TODO: these will be broken out into interfaces
    // IInputUmpEndpoint            : for mapping MIDI 1.0 input-only byte stream devices to UMP
    // IOutputUmpEndpoint           : for mapping MIDI 1.0 output-only byte stream devices to UMP
    // IBidirectionalUmpEndpoint    : approach for native MIDI 2.0 devices and MIDI 1.0 devices with in and out

    winrt::Windows::Devices::Midi2::MidiMessageReadBuffer MidiUmpEndpoint::IncomingMessages()
    {
        throw hresult_not_implemented();
    }
    winrt::Windows::Devices::Midi2::MidiMessageWriteBuffer MidiUmpEndpoint::OutcomingMessages()
    {
        throw hresult_not_implemented();
    }
}


// Prototype: Hard-code creating a new network MIDI endpoint
// or maybe just direct to the USB MIDI driver
