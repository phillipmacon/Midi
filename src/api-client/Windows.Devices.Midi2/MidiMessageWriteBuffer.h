#pragma once
#include "MidiMessageWriteBuffer.g.h"

#include <boost/circular_buffer.hpp>

namespace winrt::Windows::Devices::Midi2::implementation
{
    struct MidiMessageWriteBuffer : MidiMessageWriteBufferT<MidiMessageWriteBuffer>
    {
    private:
        const size_t BufferSize = 1001;

        boost::circular_buffer<uint32_t> _buffer { BufferSize };
        
    public:
        MidiMessageWriteBuffer() = default;

        bool CanWrite(uint16_t numberOfWords);
        void WriteWord(uint32_t word);
        void WriteWords(array_view<uint32_t const> words);

        // TODO: Internal methods

    };
}
namespace winrt::Windows::Devices::Midi2::factory_implementation
{
    struct MidiMessageWriteBuffer : MidiMessageWriteBufferT<MidiMessageWriteBuffer, implementation::MidiMessageWriteBuffer>
    {
    };
}
