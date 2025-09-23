/*
  ==============================================================================

   This file is part of the Lumasonic SDK.
   Copyright (c) 2025 - Cymatic Somatics Inc.

   This version of the Lumasonic SDK is an archived version and no longer
   commercially supported.

   The code included in this file is provided under the terms of the MIT license.
   https://mit-license.org/
   https://github.com/Lumasonic/Lumasonic/blob/main/LICENSE

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted provided that the above
   copyright notice and this permission notice appear in all copies.

   THE LUMASONIC SDK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE,
   ARE DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include "LumasonicCommon.h"
#include "LumasonicStereoDecoder.h"

//==============================================================================
/**
 * @brief A threaded @ref StereoColorSample reader that safely consumes 
 * @ref LumasonicStereoDecoder decoded color output so it can be routed
 * to visual output or a desired control scheme.
 *
 * @details
 * The LumasonicStereoReader is meant as a complimentary class to the 
 * LumasonicStereoDecoder class.
 
 * The **decoder** processes and produces data on the **audio thread**, and the
 * **reader** safely consumes the data from a **non-audio thread** so it can be 
 * used with other systems and I/O.
 * 
 * The reader implements a configurable internal thread mode that tunes
 * various performance and timing trade-offs. It's goal is to get decoded
 * data from the audio thread to a consuming thread safely, and as fast
 * as possible.
 * 
 * > [!NOTE]
 * > The the reader's internal thread consumes from the audio thread in a
 * > **lock-free** & **wait-free** manner, ensuring realtime audio safety.
 * 
 * Audio safely, in this case, means: 
 * - no system calls
 * - no memory (de)allocations
 * - no I/O calls
 * - no synchronization primitives (locks/mutexes, etc.)
 * - nothing that could potentially block the thread
 * 
 * ### Creating a Reader Instance
 * 
 * ```c++
 * 
 * auto* lsReader = new LumasonicStereoReader();
 * 
 * ```
 * 
 * ### Connecting a Reader to a Decoder
 * 
 * ```c++
 * 
 * // Create the Lumasonic decoder instance
 * auto* lsDecoder = new LumasonicStereoDecoder();
 *
 * // Create the Lumasonic reader instance
 * auto* lsReader = new LumasonicStereoReader();
 *
 * // Hook up the reader and decoder
 * lsReader->setDecoder(lsDecoder);
 * lsDecoder->setReader(lsReader);
 * 
 * // Set the thread mode of the reader
 * lsReader->setThreadMode(LumasonicThreadModes::LS_Thread_Sleep);
 *
 * // Start the reading thread
 * lsReader->start();
 * 
 * ```
 * 
 * ### Stereo Color Listeners
 * 
 * Readers can have one or more @ref StereoColorSample consuming listeners
 * registered.
 *
 * Registered listeners will be notified when new decoded
 * data is ready to be dispatched outside of the audio thread.
 * 
 * #### Implementing the LumasonicStereoColorListener Interface
 * 
 * To receive and use decoded data as it becomes available, classes should 
 * implement the @ref LumasonicStereoColorListener interface:
 * 
 * ```c++
 * 
 * // An example listener class that might update LED or screen visuals
 * class VisualsUpdater : private LumasonicStereoColorListener
 * {
 * private:
 *		void onStereColorRead(StereoColorSample stereoColor) override
 *		{
 *			// TODO Implement custom logic here for using the decoded data
 *		}
 * };
 * 
 * ```
 * 
 * ### Using Listeners with Readers
 * 
 * ```c++
 * // API use examples
 * auto* lsReader = new LumasonicStereoReader();
 * auto* visUpdater = new VisualsUpdater();		// example listener class
 * lsReader->addListener(visUpdater);			// add the listener instance to the reader
 * lsReader->removeListener(visUpdater);		// remove the listener instance from the reader when done
 * lsReader->addListener(this);					// if the owning class implements the listener interface
 * lsReader->clearListeners();					// alternatively, remove all listeners at once
  * 
 * ```
 * 
 * ### Deleting a Reader
 * 
 * When a reader instance is no longer needed, use the following sequence:
 * 
 * ```c++
 * 
 * lsReader->stop();							// first, stop the reader thread
 * lsReader->clearListeners();					// next, clear the list of listeners
 * lsReader->setDecoder(nullptr);				// finally, clear the decoder pointer
 * delete lsReader;
 * 
 * ```
 * 
 * > [!NOTE]
 * > Reader instances should be shutdown and deleted first, before deleting decoder instances.
 */
class LumasonicStereoReader : public LumasonicWaitingProcess, public LumasonicRunningProcess
{
public:
    //==============================================================================
	/** @brief Constructor*/
    LumasonicStereoReader();

	/** @brief Destructor.*/
    virtual ~LumasonicStereoReader();

	//==============================================================================
	/** @brief The unique ID of the instance.*/
	int id = -1;

	/** @brief Gets the current reading thread mode. This method is thread-safe/atomic.
		@return					The current thread mode enumeration value.
	*/
	LumasonicThreadModes getThreadMode();

	/** @brief Sets the current reading thread mode. This method is thread-safe/atomic.
	 * 
	 * ### Safe Thread Mode ([LS_Thread_Sleep](@ref LumasonicThreadModes::LS_Thread_Sleep))
	 * 
	 * Best Jitter		  | Worst Jitter	| Realtime Safe | CPU %
	 * -------------------|-----------------|---------------|-------------
	 * 1 - 2 milliseconds | 10 milliseconds | yes			| low
	 * 
	 * This thread mode is strictly realtime audio safe and sleeps the thread for the lowest available system
	 * sleep time (usually **1 ms**).
	 * 
	 * Sleeping drastically reduces the CPU load of the reader, but thread timing is varied more by system scheduling.
	 * 
	 * The actual sleep time can vary a bit from the requested time, **up to 10 ms**. On non-busy systems jitter 
	 * is often minimal, between **1 - 2 ms**.
	 * 
	 * ### Efficient Thread Mode ([LS_Thread_Event](@ref LumasonicThreadModes::LS_Thread_Event))
	 * 
	 * Best Jitter		  | Worst Jitter	 | Realtime Safe | CPU %
	 * -------------------|------------------|---------------|-------------
	 * 10 microseconds	  | 800 microseconds | no			 | lowest
	 * 
	 * This is the only thread mode that is **not strictly realtime audio safe**. However, it may perform reliably
	 * so it is worth testing.
	 * 
	 * In this thread mode, the thread yields and waits to be notified that new Lumasonic decoded data is available.
	 * 
	 * The reader thread then wakes up fairly quickly to process the data, usually within hundreds of microseconds
	 * of the producer thread's notification signal.
	 * 
	 * It is CPU efficient and has jitter as low as 15 microseconds, often no higher than a few hundred microseconds.
	 * 
	 * ### Performance Thread Mode ([LS_Thread_Loop](@ref LumasonicThreadModes::LS_Thread_Loop))
	 * 
	 * Best Jitter		  | Worst Jitter	 | Realtime Safe | CPU %
	 * -------------------|------------------|---------------|-------------
	 * < 1 microseconds	  | 300 microseconds | yes			 | highest
	 * 
	 * In Performance mode, the reader thread loops as quickly as possible, checking for new data.
	 * 
	 * This thread mode is realtime audio safe, and often times provides the lowest jitter.
	 * 
	 * It does burn a bit of CPU % waiting for new data, but can provide steady visual output for
	 * visual modulation frequencies up to gamma (**40 Hz**).
	 * 
	 * On many systems this method provides the most stable Lumasonic output at the
	 * cost of the extra CPU time reserved for the reader.
	 * 
	 *  @param newMode			The new thread mode to use for reading.
	 */
	void setThreadMode(LumasonicThreadModes newMode);

	/** @brief Sets the decoder instance to read from. This method is thread-safe/atomic.
		@param decoder			A pointer to the decoder instance to read from.
	*/
	void setDecoder(LumasonicStereoDecoder* decoder);

	/** @brief Notifies that new Lumasonic data is available to consume.
		This method should be called by the producer thread (typically the audio thread).
	*/
	void notify() override;

	/** @brief Called by registered [listeners](@ref LumasonicStereoColorListener) to indicate
		that the reader should stop.

		Calling this will stop the reader's thread. This means that all registered
		listeners will stop receiving data until the reader is restarted.
	*/
	void signalProcessShouldExit() override;

	/** @brief Starts the Lumasonic reading thread.*/
	void start();

	/** @brief Stops the Lumasonic reading thread.*/
	void stop();

	/** @brief Whether the reader/dispatcher is currently running. This method is thread-safe.
		@return						True if the reader is currently running/reading, False if not.
	*/
	bool isRunning();

	/** @brief Adds a new listener to be notified when a new stereo color sample is ready. This method is thread-safe.
		@param listener				The instance of a @ref LumasonicStereoColorListener to register for events.
	*/
	void addListener(LumasonicStereoColorListener* listener);

	/** @brief Removes an existing listener from notifications. This method is thread-safe.
		@param listener				The instance of a @ref LumasonicStereoColorListener to unregister from events.
	*/
	void removeListener(LumasonicStereoColorListener* listener);

	/** @brief Clears the current listener list. This method is thread-safe.*/
	void clearListeners();

	/** @brief Reads and pops the next performance information from the buffer. This method is thread-safe.
		@return						True if data was available and assigned, False if not.
	*/
	bool popPerfInfo(LumasonicReadPerfInfo& info);

	/** @brief Clears the current buffer of performance information.*/
	void clearPerfInfo();

private:
	class Pimpl;
	Pimpl* pimpl = nullptr;
};