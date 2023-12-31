﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

// Windows MIDI Services sample code

#include "pch.h"
#include <iostream>

using namespace winrt::Windows::Devices::Midi2;        // API
using namespace winrt::Microsoft::Devices::Midi2;      // Core SDK for checking for API, naming entities, etc.
// standated WinRT enumeration support. This is how you find attached devices.
using namespace winrt::Windows::Devices::Enumeration;

// where you find types like IAsyncOperation, IInspectable, etc.
namespace foundation = winrt::Windows::Foundation;

int main()
{
    winrt::init_apartment();

    std::cout << "Checking for MIDI Services..." << std::endl;

    // check for presence of compatible Windows MIDI Services using the SDK
    auto checkResult = MidiServices::CheckForWindowsMidiServices();

    // proceed only if MIDI services is present and compatible. Your own application may decide
    // to fall back to WinRT/WinMM MIDI 1.0 APIs, or to prompt the user to install the latest 
    // version of Windows MIDI Services

    if (checkResult == WindowsMidiServicesCheckResult::PresentAndUsable)
    {
        std::cout << "MIDI Services Present and usable." << std::endl;

        // create the MIDI session, giving us access to Windows MIDI Services. An app may open 
        // more than one session. If so, the session name should be meaningful to the user, like
        // the name of a browser tab, or a project.

        std::cout << std::endl << "Creating session..." << std::endl;

        auto session = MidiSession::CreateSession(L"Sample Session");

        // you can ask for MIDI 1.0 Byte Stream devices, MIDI 2.0 UMP devices, or both. Note that Some MIDI 2.0
        // endpoints may have MIDI 1.0 function blocks in them, so this is endpoint/device-level only.
        // Note that every device uses UMP through this API, but it can be helpful to know when a device is
        // a MIDI 1.0 device at the main interface level.

        std::cout << std::endl << "Creating Device Selector..." << std::endl;

        winrt::hstring deviceSelector = MidiBidirectionalEndpointConnection::GetDeviceSelector();

        // Enumerate UMP endpoints. Note that per C++, main cannot be a co-routine,
        // so we can't just co_await this async call, but instead use the C++/WinRT Extension "get()". 
        // We may end up wrapping this enumeration code into another class to instantly transform to 
        // MidiDeviceInformation instances, and to simplify calling code (reducing need for apps to
        // incorporate async handling).

        std::cout << std::endl << "Enumerating through Windows::Devices::Enumeration..." << std::endl;

        foundation::IAsyncOperation<DeviceInformationCollection> op = DeviceInformation::FindAllAsync(deviceSelector);
        DeviceInformationCollection endpointDevices = op.get();

        // this currently requires you have a USB MIDI 1.0 device. If you have nothing connected, just remove this check for now
        // That will change once MIDI 2.0 device selectors have been created
        if (endpointDevices.Size() > 0)
        {
            std::cout << "MIDI Endpoints were found:" << std::endl;

            DeviceInformation selectedEndpointInformation { nullptr };

            for (auto const& device : endpointDevices)
            {
                std::cout << std::endl;
                std::cout << winrt::to_string(device.Name()) << std::endl;
                std::cout << "   " << winrt::to_string(device.Id()) << std::endl;


                std::string id = winrt::to_string(device.Id());

                // TODO: We will have a more deterministic way of identifying the loopback device using a property
                // or you could directly use the Id
                if (id.find("LOOPBACK") != std::string::npos)
                {
                    selectedEndpointInformation = device;
                }
            }
            std::cout << std::endl;

            if (selectedEndpointInformation == nullptr)
            {
                std::cout << "No loopback endpoint found. This is not normal. Exiting." << std::endl;
                return 0;
            }

            // then you connect to the UMP endpoint
            std::cout << std::endl << "Connecting to UMP Endpoint..." << std::endl;

            auto endpoint = session.ConnectBidirectionalEndpoint(selectedEndpointInformation.Id());

            std::cout << "Connected to endpoint: " << winrt::to_string(selectedEndpointInformation.Name()) << std::endl;


            // Each UMP Endpoint connection creates a number of resources for communication between the API and
            // the service, and also for wiring up within the service itself. We do not prevent multiple
            // connections to the same single UMP Endpoint from within the same session, but if you do so, you 
            // want to consider the memory and processing cost associated with that.
            
            // After connecting, you can send and receive messages to/from the endpoint. Sending and receiving is
            // performed one complete UMP at a time. Each message has an associated timestamp.
            
            
            // Wire up an event handler to receive the message. There is a single event handler type, but the
            // MidiMessageReceivedEventArgs class provides the different ways to access the data
            // Your event handlers should return quickly as they are called synchronously.

            auto MessageReceivedHandler = [](foundation::IInspectable const& sender, MidiMessageReceivedEventArgs const& args)
                {
                    // there are several ways to get the message data from the arguments. If you want to use
                    // strongly-typed UMP classes, then you may start with the GetUmp() method. The GetXXX calls 
                    // are all generating something within the function, so you want to call them once and then
                    // keep the result around in a variable if you plan to refer to it multiple times. In 
                    // contrast, the FillXXX functions will update values in provided (pre-allocated) types
                    // passed in to the functions.
                    auto ump = args.GetUmp();

                    std::cout << std::endl;
                    std::cout << "Received UMP" << std::endl;
                    std::cout << "- Current Timestamp: " << std::dec << MidiClock::GetMidiTimestamp() << std::endl;
                    std::cout << "- UMP Timestamp:     " << std::dec << ump.Timestamp() << std::endl;
                    std::cout << "- UMP Msg Type:      0x" << std::hex << (uint32_t)ump.MessageType() << std::endl;
                    std::cout << "- UMP Packet Type:   0x" << std::hex << (uint32_t)ump.UmpPacketType() << std::endl;
                  

                    // if you wish to cast the IMidiUmp to a specific Ump Type, you can do so using .as<T> WinRT extension

                    if (ump.UmpPacketType() == MidiUmpPacketType::Ump32)
                    {
                        // we'll use the Ump32 type here. This is a runtimeclass that the strongly-typed 
                        // 32-bit messages derive from. There are also MidiUmp64/96/128 classes.
                        auto ump32 = ump.as<MidiUmp32>();

                        std::cout << "- Word 0:            0x" << std::hex << ump32.Word0() << std::endl;
                    }

                    std::cout << std::endl;

                };

            // the returned token is used to deregister the event later.
            auto eventRevokeToken = endpoint.MessageReceived(MessageReceivedHandler);


            std::cout << std::endl << "Opening endpoint connection (this sends out the required discovery messages which will loop back)..." << std::endl;

            // once you have wired up all your event handlers, added any filters/listeners, etc.
            // You can open the connection. Doing this will query the cache for the in-protocol 
            // endpoint information and function blocks. If not there, it will send out the requests
            // which will come back asynchronously with responses.
            endpoint.Open();


            std::cout << std::endl << "Creating MIDI 1.0 Channel Voice 32-bit UMP..." << std::endl;

            MidiUmp32 ump32{};
            ump32.MessageType(MidiUmpMessageType::Midi1ChannelVoice32);

            // here you would set other values in the UMP word(s)

            std::cout << "Sending single UMP..." << std::endl;

            auto ump = ump32.as<IMidiUmp>();
            endpoint.SendUmp(ump);

            std::cout << std::endl << "Wait for the sent UMP to arrive, and then press enter to cleanup." << std::endl;

            system("pause");

            std::cout << std::endl << "Deregistering event handler..." << std::endl;

            // deregister the event by passing in the revoke token
            endpoint.MessageReceived(eventRevokeToken);

            std::cout << "Disconnecting UMP Endpoint Connection..." << std::endl;

            // not strictly necessary as the session.Close() call will do it, but it's here in case you need it
            session.DisconnectEndpointConnection(endpoint.Id());
        }
        else
        {
            // no MIDI endpoints found. We'll just bail here
            std::cout << "No MIDI Endpoints were found." << std::endl;
        }

        // close the session, detaching all Windows MIDI Services resources and closing all connections
        // You can also disconnect individual Endpoint Connections when you are done with them
        session.Close();
    }
    else
    {
        if (checkResult == WindowsMidiServicesCheckResult::NotPresent)
        {
            std::cout << "MIDI Services Not Present" << std::endl;
        }
        else if (checkResult == WindowsMidiServicesCheckResult::IncompatibleVersion)
        {
            std::cout << "MIDI Present, but is not a compatible version." << std::endl;
        }

        // allow the user to install the minimum required version. You'd provide a UI which enables the download
        std::cout << "Here you would prompt the user to install the latest version from " << winrt::to_string(MidiServices::LatestMidiServicesInstallUri().ToString()) << std::endl;

    }

}
