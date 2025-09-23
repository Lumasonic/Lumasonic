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

#include <string.h>

// The frequency of the Lumasonic data/reference tone
#define LS_REF_TONE_FREQ			22500.f

// The frequency of the Lumasonic red tone
#define LS_RED_TONE_FREQ			21000.f

// The frequency of the Lumasonic green tone
#define LS_GREEN_TONE_FREQ			19500.f

// The frequency of the Lumasonic blue tone
#define LS_BLUE_TONE_FREQ			18000.f

// The frequency of the SpectraStrobe reference tone
#define SS_REF_TONE_FREQ			18200.f

// The frequency of the SpectraStrobe red tone
#define SS_RED_TONE_FREQ			18700.f

// The frequency of the SpectraStrobe red tone
#define SS_GREEN_TONE_FREQ			19200.f

// The frequency of the SpectraStrobe blue tone
#define SS_BLUE_TONE_FREQ			19700.f

// The SpectraStrobe reference tone panning LFO frequency
#define SS_REF_PAN_LFO_FREQ			20.f

// The frequency of the AudioStrobe tone
#define AS_TONE_FREQ				19200.f

// Define the buffer size for strings for host name/IP address
#define LS_NET_HOST_NAME_SIZE       256

// Define the buffer size for strings for the network interface
#define LS_NET_INTERFACE_NAME_SIZE  64

//==============================================================================
/**
    @brief Different light/sound codecs.
*/
enum class LightSoundCodecs
{
    None = 0,				///< No codec specified
    Lumasonic,				///< Default Lumasonic codec
    SpectraStrobe,			///< [SpectraStrobe codec](https://docs.cymaticsomatics.com/docs/latest/spectrastrobe)
    AudioStrobe				///< [AudioStrobe codec](https://docs.cymaticsomatics.com/docs/latest/spectrastrobe#similarities-to-audiostrobe)
};

//==============================================================================
/**
	@brief Different types of built-in [listeners](@ref LumasonicStereoColorListener).
*/
enum class LumasonicListenerTypes
{
	None = 0,					///< No codec specified
	LS_Stereo_Udp_Listener		///< Listener that relays stereo color values over a UDP socket.
};


//==============================================================================
/** 
 *	@brief Stores two sets of RGB floating point values and a timestamp.
 */
struct StereoColorSample
{
	/** @brief Constructor. Creates an empty stereo color initialized to black on both left and right channels.*/
	StereoColorSample()
	{
		ts = 0;

		// Default to black
		r0 = g0 = b0 = 0;
		r1 = g1 = b1 = 0;
	}

	/** @brief Constructor. Initializes the sample with a given timestamp and left and right red, blue, green color values.*/
	StereoColorSample(unsigned long long time, 
					  float red0, float green0, float blue0, 
					  float red1, float green1, float blue1)
	{
		ts = time;
		r0 = red0;
		g0 = green0;
		b0 = blue0;
		r1 = red1;
		g1 = green1;
		b1 = blue1;
	}

	unsigned long long ts;		///< timestamp
	float r0;					///< red channel 0
	float g0;					///< green channel 0
	float b0;					///< blue channel 0
	float r1;					///< red channel 1
	float g1;					///< green channel 1
	float b1;					///< blue channel 1

	/** @brief Equality operator */
	bool operator == (const StereoColorSample& a)
	{
		return isEqual(a);
	}

	/** @brief Inequality operator */
	bool operator != (const StereoColorSample& a)
	{
		return !isEqual(a);
	}

	/** @brief Determines whether two instances are equal.*/
	inline bool isEqual(const StereoColorSample& a)
	{
		// Pointing to the same memory address?
		if (&a == this)
			return true;

		// Otherewise compare values
		return	a.ts == ts &&
				a.r0 == r0 && a.g0 == g0 && a.b0 == b0 &&
				a.r1 == r1 && a.g1 == g1 && a.b1 == b1;
	}

	/** @brief Returns an empty/default stereo color.
		@return			An empty (all black) stereo color sample.
	*/
	inline static StereoColorSample empty() { return { 0, 0, 0, 0, 0, 0, 0 }; }

	/** @brief Determines whether two instances are equal.*/
	inline static bool areEqual(const StereoColorSample& a, const StereoColorSample& b)
	{
		// Pointing to the same memory address?
		if (&a == &b)
			return true;

		// Otherewise compare values
		return	a.ts == b.ts &&
				a.r0 == b.r0 && a.g0 == b.g0 && a.b0 == b.b0 &&
				a.r1 == b.r1 && a.g1 == b.g1 && a.b1 == b.b1;
	}
};

//==============================================================================
/**
	@brief Different threading modes for getting data from the audio thread to a worker thread.
*/
enum class LumasonicThreadModes
{
	LS_Thread_Sleep = 0,		///< The thread will sleep by ~1 millisecond every loop while it waits for data; system sleep timing can introduce a small amount of jitter
	LS_Thread_Event,		///< The thread waits to be notified via a conditional event (good performance but not strictly realtime audio safe)
	LS_Thread_Loop			///< An aggressive thread that loops without stopping waiting for data (best timing performance, but worse CPU performance)
};

//==============================================================================
/**
 * @brief Contains metrics related to Lumasonic data read performance.
 * 
 * @details
 * This struct is used by the @ref LumasonicStereoReader to capture timing 
 * performance statistics related to the throughput of decoded Lumasonic data.
 * 
 * Timing statistics are useful in helping identify the best **audio driver**
 * and reader [thread mode](@ref LumasonicThreadModes) to maximize Lumasonic
 * output stability and minimize jitter.
 * 
 * Reader performance information includes total min/max values reached per
 * session, as well as a rolling average window on the current timing values.
 * 
 * ### Timing Categories
 * 
 * Timing performance data is split between two categories:
 * 
 * Name   | Description
 * -------|------------
 * thread | the delta time between calls to the reader's thread
 * data   | the delta time between continuous decoded data availability
 * 
 * ### thread
 * 
 * The reader's [thread mode](@ref LumasonicThreadModes) affects the timing
 * characteristics of the reader thread.
 * 
 * ### data
 * 
 * Ideally, data would be available for use as soon as it is ready during
 * decoding in the realtime audio thread. However, because of realtime 
 * audio thread restrictions the data must be queued and consumed from 
 * a non-realtime thread.
 * 
 * > [!NOTE]
 * > Getting the producing audio thread and the consuming reader thread to 
 * > line up in time as much as possible is the key to achieving the 
 * > lowest latency and jitter.
 * 
 * ### Timing Performance Conditions
 * 
 * Since **production** and **consumption** of Lumasonic data occurs on two separate
 * threads, the timing parity between the two is critical. When the two threads
 * have different timing, jitter is introduced. The goal is to minimize jitter.
 * 
 * The steadiness of an audio thread callback will be set by the type of audio device
 * and audio driver being used (**on Windows favor ASIO**).
 * 
 * As timing between the two threads varies, the reader thread might
 * read too early before data is ready, or too late. In this case,
 * the data will be consumed at the next available read cycle in the future.
 * 
 * Small timing variance (**1 - 2 ms**) can usually be ignored, especially at lower
 * visual modulation frequencies. However, higher visual modulation
 * frequencies will begin to show timing jitter (around **40 Hz** gamma visual modulation).
 * 
 * ### Reading Performance Data
 * 
 * To read timing performance data use the `LumasonicStereoReader::popPerfInfo(LumasonicReadPerfInfo&)` method.
 * 
 * This method can be safely called from any thread.
 * 
 * ```c++
 * 
 * LumasonicReadPerfInfo info {};
 *
 * while (reader->popPerfInfo(info))
 * {
 *		// Access all of the timing performance values here
 *		auto avgThreadTimeMs = info.threadAverageTimeMs;
 *		auto avgDataTimeMs = info.dataAverageTimeMs;
 * }
 * 
 * ```
 * 
 * ### Interpreting Performance Data
 * 
 * To calculate your target Lumasonic frames per second for the audio thread:
 * 
 * ```
 * Sample Rate / Audio Buffer Size = Frames per Second
 * ```
 * 
 * To calculate the audio frame time in milliseconds:
 * 
 * ```
 * 1000 / Frames per Second = Audio Frame Time (ms)
 * ```
 * 
 * Example:
 * 
 * ```c++
 * float fps = 48000.f / 256;			// 187.5 fps
 * float frameTimeMs = 1000.f / fps;	// 5.33 ms
 * ```
 * 
 * > [!NOTE]
 * > @ref dataAverageTimeMs should be as close to your calculated audio frame time as possible (in the example above, 5.33 ms)
 * 
 * #### Thread Timing Values
 * 
 * Name			   |Member						 | Description
 * ----------------|-----------------------------|-------------
 * Min. Time       | @ref threadCurrentMinTimeMs | rolling time window: shortest time between reader thread calls
 * Max. Time       | @ref threadCurrentMaxTimeMs | rolling time window: longest time between reader thread calls
 * Average Time	   | @ref threadAverageTimeMs	 | rolling time window: average time between reader thread calls
 * Total Min. Time | @ref threadTotalMinTimeMs	 | session total: lowest time between reader thread calls
 * Total Max. Time | @ref threadTotalMaxTimeMs	 | session total: longest time between reader thread calls
 * 
 * #### Data Timing Values
 * 
 * Name			   |Member						 | Description
 * ----------------|-----------------------------|-------------
 * Min. Time       | @ref dataCurrentMinTimeMs	 | rolling time window: shortest time between data output availability
 * Max. Time       | @ref dataCurrentMaxTimeMs	 | rolling time window: longest time between data output availability
 * Average Time	   | @ref dataAverageTimeMs		 | rolling time window: average time between data output availability
 * Total Min. Time | @ref dataTotalMinTimeMs	 | session total: lowest time between data output availability
 * Total Max. Time | @ref dataTotalMaxTimeMs	 | session total: longest time between data output availability
 */
struct LumasonicReadPerfInfo
{
	double threadCurrentMinTimeMs;	///< The current minimum delta time between calls to the decoder read thread (in milliseconds).
	double threadCurrentMaxTimeMs;	///< The current maximum delta time between calls to the decoder read thread (in milliseconds).
	double threadAverageTimeMs;		///< The current average delta time between calls to the decoder read thread (in milliseconds).
	double threadTotalMinTimeMs;	///< The shortest delta time between calls to the decoder read thread since performance monitoring started (in milliseconds).
	double threadTotalMaxTimeMs;	///< The longest delta time between calls to the decoder read thread since performance monitoring started (in milliseconds).
	double dataCurrentMinTimeMs;	///< The current minimum delta time between the current data output availablity and the previous data output availability (in milliseconds).
	double dataCurrentMaxTimeMs;	///< The current maximum delta time between the current data output availablity and the previous data output availability (in milliseconds).
	double dataAverageTimeMs;		///< The current average delta time between data output availablity cycles.
	double dataTotalMinTimeMs;		///< The shortest delta time between data output availability cycles since performance monitoring started (in milliseconds).
	double dataTotalMaxTimeMs;		///< The shortest delta time between data output availability cycles since performance monitoring started (in milliseconds).
};

//==============================================================================
/**
 *	@brief Interface for classes that wait to be notifyed before consuming some work.
 */
class LumasonicWaitingProcess
{
public:
	/** @brief Called by the producing thread when there is more work to process.*/
	virtual void notify() = 0;
};

//==============================================================================
/**
 * @brief Interface for classes that have their own thread which can
 * be signaled for shutdown from a listener that runs within the scope of that thread.
 */
class LumasonicRunningProcess
{
public:
	/** @brief Called by a listener within a running process to indicate that the process should exit.*/
	virtual void signalProcessShouldExit() = 0;
};

//==============================================================================
/**
 * @brief Interface for classes that will listen for available Lumasonic
 * @ref StereoColorSample data to become ready from a @ref LumasonicStereoDecoder
 * consumed by a @ref LumasonicStereoReader.
 * 
 * ### Stereo Color Listeners
 * 
 * The @ref LumasonicStereoReader class can have one or more 
 * @ref StereoColorSample consuming listeners registered.
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
 * // An example listener class that might update LEDs or screen visuals
 * class VisualsUpdater : private LumasonicStereoColorListener
 * {
 * private:
 *		void onStereoColorRead(LumasonicRunningProcess& proc, StereoColorSample sc) override
 *		{
 *			// TODO Implement custom logic here for using the decoded data
 *			auto ts = sc.ts;				// timestamp
 *			auto r0 = sc.r0;				// left red
 *			auto g0 = sc.g0;				// left green
 *			auto b0 = sc.b0;				// left blue
 *			auto r1 = sc.r1;				// right red
 *			auto g1 = sc.g1;				// right green
 *			auto b1 = sc.b1;				// right blue
 * 
 *			//----------------------------------------------------------------------------------------------
 *			// Only use this if the listener's callback needs to signal to the reader to abort reading.
 *			// This stops the reader's thread entirely so all registered listener's will stop receiving data.
 *			//----------------------------------------------------------------------------------------------
 *			proc.signalProcessShouldExit();	// signals to the parent reader that it should stop reading
 *		}
 * };
 * 
 * ```
 * 
 * When a reader consumes new decoded data from a decoder it will notify
 * each registered listener and pass it the new data by calling 
 * `LumasonicStereoColorListener::onStereoColorRead(LumasonicRunningProcess&, StereoColorSample)`.
 */
class LumasonicStereoColorListener
{
public:
	/** @brief Constructor*/
	LumasonicStereoColorListener() = default;

	/** @brief Destructor*/
	virtual ~LumasonicStereoColorListener() {};

	/** @brief This method is called when the reader's thread has new color data available.
		@param process				A reference to the running process that called this listener.
		@param stereoColor			The stereo color sample value that has been read.
	*/
	virtual void onStereoColorRead(LumasonicRunningProcess& process, StereoColorSample stereoColor) = 0;
};

//==============================================================================
/**
	@brief Contains network/socket configuration settings for the @ref LumasonicStereoUdpListener class.
*/
struct UdpListenerConfig
{
	unsigned int localAddress;		///< The IPv4 address of the local interface to use, as a 4 byte unsigned int
	unsigned int remoteAddress;		///< The IPv4 address of the remote host to send to, as a 4 byte unsigned int
	unsigned short remotePort;      ///< The remote port number to send to.
	unsigned short localPort;       ///< The local port number to bind to. Use 0 to automatically select an open local port.
	bool exclusive;                 ///< Whether the socket will bind in exclusive mode or share socket with other processes.
};