#include "pch.h"
#include "MidiMessageWriteBuffer.h"
#include "MidiMessageWriteBuffer.g.cpp"


namespace winrt::Windows::Devices::Midi2::implementation
{
    bool MidiMessageWriteBuffer::CanWrite(uint16_t numberOfWords)
    {
        return _buffer.size() < _buffer.capacity();
    }


    void MidiMessageWriteBuffer::WriteWord(uint32_t word)
    {
        _buffer.push_back(word);
    }

    void MidiMessageWriteBuffer::WriteWords(array_view<uint32_t const> words)
    {
        // TODO
    }
}
