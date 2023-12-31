// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================


#include "pch.h"

#include "catch_amalgamated.hpp"

#include <iostream>
#include <algorithm>

using namespace winrt::Windows::Devices::Midi2;

TEST_CASE("Offline.Ump.MessageAndPacketTypes UMP Message and Packet Types")
{
    SECTION("Test UMP32")
    {
        MidiUmpMessageType mt = MidiUmpMessageType::Midi1ChannelVoice32;

        MidiUmp32 ump;
        ump.MessageType(mt);

        REQUIRE(ump.MessageType() == mt);
        REQUIRE(ump.UmpPacketType() == MidiUmpPacketType::Ump32);
    }


    SECTION("Test UMP64")
    {
        MidiUmpMessageType mt = MidiUmpMessageType::Midi2ChannelVoice64;

        MidiUmp64 ump;
        ump.MessageType(mt);

        REQUIRE(ump.MessageType() == mt);
        REQUIRE(ump.UmpPacketType() == MidiUmpPacketType::Ump64);
    }

    SECTION("Test UMP96")
    {
        MidiUmpMessageType mt = MidiUmpMessageType::FutureReservedB96;

        MidiUmp96 ump;
        ump.MessageType(mt);

        REQUIRE(ump.MessageType() == mt);
        REQUIRE(ump.UmpPacketType() == MidiUmpPacketType::Ump96);
    }

    SECTION("Test UMP128")
    {
        MidiUmpMessageType mt = MidiUmpMessageType::UmpStream128;

        MidiUmp128 ump;
        ump.MessageType(mt);

        REQUIRE(ump.MessageType() == mt);
        REQUIRE(ump.UmpPacketType() == MidiUmpPacketType::Ump128);
    }



    // TODO: Add message type checking into the UMP class and then test it here (message type must work for packet type)






}


TEST_CASE("Offline.Ump.Casting UMP Interface Casting")
{
    MidiUmpMessageType mt = MidiUmpMessageType::Midi1ChannelVoice32;

    MidiUmp32 originalUmp;
    originalUmp.Word0(0x08675309);
    originalUmp.MessageType(mt);        // set message type after the word because it changes Word0

    REQUIRE(originalUmp.MessageType() == mt);
    REQUIRE(originalUmp.UmpPacketType() == MidiUmpPacketType::Ump32);

    // cast as the interface
    IMidiUmp iface = originalUmp.as<IMidiUmp>();
    REQUIRE(iface.MessageType() == mt);
    REQUIRE(iface.UmpPacketType() == MidiUmpPacketType::Ump32);

    // recast from the interface back to the UMP type to validate data is still there
    MidiUmp32 recastUmp = iface.as<MidiUmp32>();
    REQUIRE(recastUmp.MessageType() == mt);
    REQUIRE(recastUmp.UmpPacketType() == MidiUmpPacketType::Ump32);
    REQUIRE(recastUmp.Word0() == originalUmp.Word0());

}


//TEST_CASE("UMP Buffer Access")
//{
//    SECTION("Test UMP32")
//    {
//        MidiUmpMessageType mt = MidiUmpMessageType::Midi1ChannelVoice32;
//
//        MidiUmp32 ump;
//        ump.MessageType(mt);
//
//
//
//        // TODO: Set properties and then check that the buffer data matches.
//
//
//
//    }
//}

