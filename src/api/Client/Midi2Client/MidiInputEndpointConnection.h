// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once

#include <pch.h>



#include "MidiInputEndpointConnection.g.h"

#include "MidiMessageReceivedEventArgs.h"

#include "midi_service_interface.h"

#include "InternalMidiMessageReceiverHelper.h"


namespace winrt::Windows::Devices::Midi2::implementation
{
    struct MidiInputEndpointConnection : MidiInputEndpointConnectionT<
        MidiInputEndpointConnection, 
        IMidiCallback>
    {
        MidiInputEndpointConnection() = default;
        ~MidiInputEndpointConnection();

        static hstring GetDeviceSelector() noexcept { return L"System.Devices.InterfaceClassGuid:=\"{AE174174-6396-4DEE-AC9E-1E9C6F403230}\" AND System.Devices.InterfaceEnabled:=System.StructuredQueryType.Boolean#True"; }


        hstring Id() const noexcept { return m_id; }
        hstring DeviceId() const noexcept { return m_deviceId; }
        bool IsOpen() const noexcept { return m_isOpen; }
        IMidiEndpointDefinedConnectionSettings Settings() noexcept { return m_settings; }

        winrt::Windows::Devices::Midi2::MidiEndpointConnectionSharing ActiveSharingMode() { return m_activeSharingMode; }

        IInspectable Tag() const noexcept { return m_tag; }
        void Tag(_In_ IInspectable value) noexcept { m_tag = value; }





        winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Devices::Midi2::IMidiEndpointMessageListener> MessageListeners() { return m_messageListeners; }



        STDMETHOD(Callback)(_In_ PVOID Data, _In_ UINT Size, _In_ LONGLONG Position) override;

        inline winrt::event_token MessageReceived(_In_ winrt::Windows::Foundation::TypedEventHandler<IInspectable, winrt::Windows::Devices::Midi2::MidiMessageReceivedEventArgs> const& handler)
        {
            return m_messageReceivedEvent.add(handler);
        }

        inline void MessageReceived(_In_ winrt::event_token const& token) noexcept
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
        winrt::com_ptr<IMidiIn> m_endpointInterface{ nullptr };

        internal::InternalMidiMessageReceiverHelper m_messageReceiverHelper;

        winrt::event<winrt::Windows::Foundation::TypedEventHandler<IInspectable, winrt::Windows::Devices::Midi2::MidiMessageReceivedEventArgs>> m_messageReceivedEvent;

        winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Devices::Midi2::IMidiEndpointMessageListener>
            m_messageListeners{ winrt::single_threaded_vector<winrt::Windows::Devices::Midi2::IMidiEndpointMessageListener>() };

        _Success_(return == true)
            bool ActivateMidiStream(
                _In_ winrt::com_ptr<IMidiAbstraction> serviceAbstraction,
                _In_ const IID & iid,
                _Out_ void** iface);

    };
}
namespace winrt::Windows::Devices::Midi2::factory_implementation
{
    struct MidiInputEndpointConnection : MidiInputEndpointConnectionT<MidiInputEndpointConnection, implementation::MidiInputEndpointConnection>
    {
    };
}
