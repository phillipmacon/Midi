// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once
#include <pch.h>


#include "MidiBidirectionalEndpointConnection.g.h"

#include "midi_service_interface.h"

#include "MidiMessageReceivedEventArgs.h"

#include "MidiUmp32.h"
#include "MidiUmp64.h"
#include "MidiUmp96.h"
#include "MidiUmp128.h"

#include "InternalMidiMessageReceiverHelper.h"
#include "InternalMidiMessageSenderHelper.h"



namespace winrt::Windows::Devices::Midi2::implementation
{
    struct MidiBidirectionalEndpointConnection : MidiBidirectionalEndpointConnectionT<
        MidiBidirectionalEndpointConnection, 
        IMidiCallback>
    {
        MidiBidirectionalEndpointConnection() = default;
        ~MidiBidirectionalEndpointConnection();

        static hstring GetDeviceSelector() noexcept { return L"System.Devices.InterfaceClassGuid:=\"{E7CCE071-3C03-423f-88D3-F1045D02552B}\" AND System.Devices.InterfaceEnabled:=System.StructuredQueryType.Boolean#True"; }



        hstring Id() const noexcept { return m_id; }
        hstring DeviceId() const noexcept { return m_deviceId; }
        bool IsOpen() const noexcept { return m_isOpen; }
        IMidiEndpointDefinedConnectionSettings Settings() noexcept { return m_settings; }

        winrt::Windows::Devices::Midi2::MidiEndpointConnectionSharing ActiveSharingMode() { return m_activeSharingMode; }

        IInspectable Tag() const noexcept { return m_tag; }
        void Tag(_In_ IInspectable value) noexcept { m_tag = value; }







        winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Devices::Midi2::IMidiEndpointMessageListener> MessageListeners() { return m_messageListeners; }


        _Success_(return == true)
        bool SendUmp(
            _In_ winrt::Windows::Devices::Midi2::IMidiUmp const& ump);

        _Success_(return == true)
        bool SendUmpWords(
            _In_ internal::MidiTimestamp const timestamp, 
            _In_ uint32_t const word0);

        _Success_(return == true)
        bool SendUmpWords(
            _In_ internal::MidiTimestamp const timestamp, 
            _In_ uint32_t const word0, 
            _In_ uint32_t const word1);

        _Success_(return == true)
        bool SendUmpWords(
            _In_ internal::MidiTimestamp const timestamp, 
            _In_ uint32_t const word0, 
            _In_ uint32_t const word1, 
            _In_ uint32_t const word2);

        _Success_(return == true)
        bool SendUmpWords(
            _In_ internal::MidiTimestamp const timestamp, 
            _In_ uint32_t const word0, 
            _In_ uint32_t const word1, 
            _In_ uint32_t const word2, 
            _In_ uint32_t const word3);

        _Success_(return == true)
        bool SendUmpWordArray(
            _In_ internal::MidiTimestamp const timestamp, 
            _In_ array_view<uint32_t const> words, 
            _In_ uint32_t const wordCount);

        _Success_(return == true)
        bool SendUmpBuffer(
            _In_ internal::MidiTimestamp timestamp, 
            _In_ winrt::Windows::Foundation::IMemoryBuffer const& buffer, 
            _In_ uint32_t byteOffset, 
            _In_ uint32_t byteLength);


        STDMETHOD(Callback)(_In_ PVOID Data, _In_ UINT Size, _In_ LONGLONG Position) override;
        
        winrt::event_token MessageReceived(_In_ winrt::Windows::Foundation::TypedEventHandler<IInspectable, winrt::Windows::Devices::Midi2::MidiMessageReceivedEventArgs> const& handler)
        {
            return m_messageReceivedEvent.add(handler);
        }

        void MessageReceived(_In_ winrt::event_token const& token) noexcept
        {
            if (m_messageReceivedEvent) m_messageReceivedEvent.remove(token);
        }

        _Success_(return == true)
        bool InternalInitialize(
            _In_ winrt::com_ptr<IMidiAbstraction> serviceAbstraction,
            _In_ winrt::hstring const endpointId,
            _In_ winrt::hstring const deviceId);

        _Success_(return == true)
        bool Open();


    private:
        hstring m_id{};
        hstring m_deviceId{};
        IInspectable m_tag{ nullptr };
        winrt::Windows::Devices::Midi2::MidiEndpointConnectionSharing m_activeSharingMode{ winrt::Windows::Devices::Midi2::MidiEndpointConnectionSharing::Unknown };

        bool m_isOpen{ false };

        IMidiEndpointDefinedConnectionSettings m_settings{ nullptr };

        winrt::com_ptr<IMidiAbstraction> m_serviceAbstraction{ nullptr };
        winrt::com_ptr<IMidiBiDi> m_endpointInterface{ nullptr };
        internal::InternalMidiMessageReceiverHelper m_messageReceiverHelper;
        internal::InternalMidiMessageSenderHelper<IMidiBiDi> m_messageSenderHelper;

        winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Devices::Midi2::IMidiEndpointMessageListener>
            m_messageListeners{ winrt::single_threaded_vector<winrt::Windows::Devices::Midi2::IMidiEndpointMessageListener>() };


        winrt::event<winrt::Windows::Foundation::TypedEventHandler<IInspectable, winrt::Windows::Devices::Midi2::MidiMessageReceivedEventArgs>> m_messageReceivedEvent;

        _Success_(return == true)
        bool ActivateMidiStream(
            _In_ winrt::com_ptr<IMidiAbstraction> serviceAbstraction, 
            _In_ const IID& iid, 
            _Out_ void** iface);


    };
}
namespace winrt::Windows::Devices::Midi2::factory_implementation
{
    struct MidiBidirectionalEndpointConnection : MidiBidirectionalEndpointConnectionT<MidiBidirectionalEndpointConnection, implementation::MidiBidirectionalEndpointConnection>
    {
    };
}
