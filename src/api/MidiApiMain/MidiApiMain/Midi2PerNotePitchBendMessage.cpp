#include "pch.h"
#include "Midi2PerNotePitchBendMessage.h"
#include "Midi2PerNotePitchBendMessage.g.cpp"


namespace winrt::Windows::Devices::Midi::implementation
{
    uint8_t Midi2PerNotePitchBendMessage::NoteNumber()
    {
        throw hresult_not_implemented();
    }
    uint32_t Midi2PerNotePitchBendMessage::Data()
    {
        throw hresult_not_implemented();
    }
}
