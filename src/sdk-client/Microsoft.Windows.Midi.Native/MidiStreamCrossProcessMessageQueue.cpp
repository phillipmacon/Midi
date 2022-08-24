// ------------------------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the GitHub project root for license information.
// ------------------------------------------------------------------------------------------------

#include "MidiMessageQueue.h"


namespace Windows::Devices::Midi::Internal
{


	// TODO: Need to see if size is in messages or bytes. 


	std::unique_ptr<MidiStreamCrossProcessMessageQueue> MidiStreamCrossProcessMessageQueue::CreateNew(const GUID sessionId, const GUID deviceId, const GUID streamId, const MidiMessageQueueType type, const wchar_t* name, const int capacityInMidiWords)
	{
		assert(name != nullptr);
		assert(capacityInMidiWords > 0);

		auto q = std::make_unique<MidiStreamCrossProcessMessageQueue>();

		// Create the new queue
		q->_queue = std::make_unique<boost::interprocess::message_queue>(
			boost::interprocess::create_only,
			name,
			capacityInMidiWords,
			sizeof uint32_t);

		std::wstring mutexName(L"midi_mtx_");
		mutexName.append(name);

		// create the cross-process synchronization mechanism
		q->_mutex = std::make_unique<boost::interprocess::named_mutex>(
			mutexName.c_str(),
			boost::interprocess::create_only
			);

		return q;
	}

	std::unique_ptr<MidiStreamCrossProcessMessageQueue> MidiStreamCrossProcessMessageQueue::OpenExisting(const GUID sessionId, const GUID deviceId, const GUID streamId, const MidiMessageQueueType type)
	{
		assert(name != nullptr);

		auto q = std::make_unique<MidiStreamCrossProcessMessageQueue>();

		q->_queue = std::make_unique<boost::interprocess::message_queue>(
			boost::interprocess::open_only,
			name);

		return q;
	}



	// returns true if the queue is empty
	const bool MidiStreamCrossProcessMessageQueue::IsEmpty()
	{
		assert(_queue != nullptr);

		return _queue->get_num_msg() == 0;
	}

	// returns true if the queue is full
	const bool MidiStreamCrossProcessMessageQueue::IsFull()
	{
		assert(_queue != nullptr);

		return _queue->get_max_msg() == _queue->get_num_msg();
	}



	// returns the number of words currently in the queue
	const uint16_t MidiStreamCrossProcessMessageQueue::getCountWords()
	{
		assert(_queue != nullptr);

		return _queue->get_num_msg();
	}

	// returns the current max capacity
	const uint16_t MidiStreamCrossProcessMessageQueue::getMaxCapacityInWords()
	{
		assert(_queue != nullptr);

		return _queue->get_max_msg();
	}

	// resizes queue while keeping contents
	//virtual bool Resize(uint16_t newCapacityInWords);					

	// for beginning a transaction / message. Locks the queue for writing.
	bool MidiStreamCrossProcessMessageQueue::BeginWrite()
	{
		assert(_queue != nullptr);
		assert(_mutex != nullptr);
		assert(!_inWrite);


		this->_inWrite = true;

		// TODO: Lock queue. Pause sending updates


		_mutex->lock();


	}	
	
	// end the current set and notify listeners. Unlocks queue when done.
	void MidiStreamCrossProcessMessageQueue::EndWrite()
	{
		assert(_queue != nullptr);
		assert(_mutex != nullptr);
		assert(_inWrite);

		// TODO: Unlock queue. Send notifications

		this->_inWrite = false;
	}

	// adds the number of words from the address
	bool MidiStreamCrossProcessMessageQueue::WriteWords(const uint32_t* wordsBuffer, const int count)
	{
		assert(_queue != nullptr);
		assert(wordsBuffer != nullptr);
		assert(count > 0);
		assert(_inWrite);

		_queue->send(wordsBuffer, count, MidiStreamCrossProcessMessageQueue::DefaultPriority);
	}


	// updates word and count with the number actually ready
	bool MidiStreamCrossProcessMessageQueue::ReadWords(uint32_t* wordsBuffer, int& count)
	{
		assert(_queue != nullptr);
		assert(wordsBuffer != nullptr);
		assert(count > 0);

		_queue->receive(wordsBuffer, count, )
	}

	// gets the first word but doesn't remove it.
	const bool MidiStreamCrossProcessMessageQueue::PeekWord(uint32_t& word)
	{
		assert(_queue != nullptr);

	}

};


