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

//==============================================================================
/**
 * @brief A Lumasonic decoder for stereo audio streams.
 *
 * @details
 * This module can process **32 bit floating point stereo audio** streams and
 * decode [Lumasonic](https://docs.cymaticsomatics.com/docs/latest/lumasonic) encoded signals.
 * 
 * ### Encoded Signal Composition
 * 
 * Each mono audio channel is assumed to have 4 encoded signals:
 *
 * # | Encoded Signal
 * --|----------------
 * 1 | Reference Level
 * 2 | Red Level
 * 3 | Green Level
 * 4 | Blue Level
 * 
 * See @ref LumasonicCommon.h for a list of encoded carrier tone frequencies.
 * 
 * ### Creating a Decoder Instance
 * 
 * ```c++
 * 
 * auto* lsDecoder = new LumasonicStereoDecoder();
 * 
 * ```
 * 
 * ### Decoding Filter Settings
 * 
 * When a Lumasonic decoder instance is initialized at a specific **sample rate**
 * and **audio sample buffer size**, it internally selects the most appropriate
 * **decoding filter design** and **decoding filter buffer size**.
 * 
 * ### Initializing the Decoder
 * 
 * ```c++
 * 
 * // initialize with a 48 KHz sample rate, and an audio buffer size of 256 samples
 * lsDecoder->reset(48000.f, 256);
 * 
 * ```
 *
 * ### The Decoding Process
 * 
 * Every time the decoder processes the same number of audio samples as the
 * decoding filter's buffer size, a @ref StereoColorSample is produced. An internal
 * circular buffer is filled with decoded output.
 * 
 * Lumasonic Decoder's circular color output buffer:
 * - wait-free & lock-free for glitch-free realtime audio
 * - safely accessible by a non-audio thread (for processing Lumasonic output)
 * - intended to be accessed by a single consuming thread
 * 
 * #### Processing Audio Buffers
 * 
 * ```c++
 * 
 * // (const float*) 'bufferL' and 'bufferR' are pointers to the left and right audio sample buffers
 * // (int) 'numSamples' is the total number of samples in the buffers to process 
 * lsDecoder->processBlock(bufferL, bufferR, numSamples);
 * 
 * ```
 * 
 * #### Reading Decoded Values
 * 
 * ```c++
 * // Consume all of the available decoded stereo color samples and count the number read
 * int count = 0;
 * StereoColorSample sc;
 * while (lsDecoder->popColorSample(sc)) // thread-safe from a single non-audio consumer thread
 * {
 *      // TODO Use color values here to update target visual systems: sc.r0, sc.g0, sc.b0 | sc.r1, sc.g1, sc.b1
 *      ++count; // total number of Lumasonic color samples consumed/read
 * }
 * 
 * ```
 * 
 * #### Clearing Decoded Values
 * 
 * ```c++
 * auto* lsDecoder = new LumasonicStereoDecoder();
 * lsDecoder->clearColorSamples(); // clear all decoded stereo color samples in the buffer; thread safe
 * 
 * ```
 * 
 * ### Decoded Lumasonic Output
 * 
 * Each @ref StereoColorSample has the following layout:
 * 
 * Name | Value              | Type
 * -----|--------------------|------------
 * ts   | Time Stamp         | 64 bit unsigned int
 * r0   | Red Level Left     | 32 bit float
 * g0   | Green Level Left   | 32 bit float
 * b0   | Blue Level Left    | 32 bit float
 * r1   | Red Level Right    | 32 bit float
 * g1   | Green Level Right  | 32 bit float
 * b1   | Blue Level Right   | 32 bit float
 * 
 * ### Frame Rate Formula
 * 
 * The Lumasonic codec is built for a sample rate of **48000 Hz**.
 * 
 * ```
 * Sample Rate / Audio Buffer Size = Frames per Second
 * ```
 * 
 * ```
 * 48000 / 256 = 187.5 fps
 * ```
 *
 * ```
 * 48000 / 192 = 250 fps
 * ```
 * 
 * ```
 * 48000 / 128 = 375 fps
 * ```
 * 
 * ### Frame Rate Jitter
 * 
 * > [!NOTE]
 * > Lumasonic output is tied to the quality of the audio driver used.
 * > The more time-stable the audio callback rate is, the more stable Lumasonic
 * > output will be.
 * 
 * Some audio drivers will deliver audio by the system deadline, but will jitter
 * the timing of audio device callbacks within the range of the system delivery 
 * deadline window.
 * 
 * Driver jitter does appear on Windows and some Linux default audio drivers.
 * On Windows, ASIO drivers should always provide the most stable output, 
 * and on Linux, JACK has produced great results.
 *
 * The Lumasonic SDK has tools for testing performance of a system's audio drivers
 * and thread models.
 * 
 * Neuro enterainment applications should always try to minimize jitter and delay
 * as much as possible.
 */
class LumasonicStereoDecoder
{
public:
    //==============================================================================
    /** @brief Constructor
        
        > [!NOTE]
        > If you set a log file path, after all decoder instances are deleted, call
        > the static stopLogging() method to free file logger memory and prevent memory leaks.

        @param logFilePath      An optional absolute file path to a log file to write log information to.
        @param maxLogFileSize   When using a log file, the maximum size of the log file in bytes.
    */
    LumasonicStereoDecoder(const char* logFilePath = nullptr, const unsigned long long maxLogFileSize = 128 * 1024);

    /** @brief Destructor*/
    ~LumasonicStereoDecoder();

    //==============================================================================
    /** @brief The unique ID of the instance.*/
    int id = -1;

    /** @brief Gets the current decoding codec. This method is thread-safe/atomic.
        @return                     The enumeration value for the current codec being used.
    */
    LightSoundCodecs getCodec();

    /** @brief Sets the current decoding codec. This method is thread-safe/atomic.
        
        > [!NOTE]
        > The codec is typically set automatically and internally by the decoder.

        @param newCodec             The new codec to use for decoding.
    */
    void setCodec(LightSoundCodecs newCodec);

    /** @brief Whether the current stream is detected as AudioStrobe. This method is thread-safe/atomic.
        @return                     True if the current stream is detected as AudioStrobe, False if Lumasonic is detected.
    */
    bool isAudioStrobe();

    /** @brief Gets the current output brightness level (0.0 - 1.0). This method is thread-safe/atomic.
        @return                     The normalized global output brightness level in the range from 0.0 to 1.0.
    */
    float getBrightness();

    /** @brief Sets the current output brightness level (0.0 - 1.0). This method is thread-safe/atomic.
        @param newBrightness        The new brightness value to set. This value should be normalized from 0.0 - 1.0.
    */
    void setBrightness(float newBrightness);

    /** @brief Initializes the decoder with the given sample rate and audio sample buffer size.
        This method should be called from the audio thread.

         * When a Lumasonic decoder instance is initialized at a specific **sample rate**
         * and **audio sample buffer size**, it internally selects the most appropriate
         * **decoding filter design** and **decoding filter buffer size**.
         
         * Often times the internal filter buffer size matches the audio buffer size, but not always.

        @param sampleRate           The number of samples per second of the audio signal.
        @param bufferSize           The maximum number of audio samples per audio buffer processed.
    */
    void reset(float sampleRate, int bufferSize);

    /** @brief Processes a block of audio through the decoder.
        
        The audio samples should be 32 bit floating point.

        > [!Note]
        > This method should be called from the audio thread only.
        > If using interleaved stereo audio streams, the audio samples will
        > need to be deinterleaved first before calling processBlock().

        @param in0                  A pointer to the left/channel 0 input sample buffer to decode.
        @param in1                  A pointer to the right/channel 1 input sample buffer to decode.
        @param numSamples           The number of samples in each audio buffer to process.
    */
    void processBlock(const float* in0, const float* in1, int numSamples);

    /** @brief Gets the current red parameter normalized level for the left channel. This method is thread-safe/atomic.*/
    float getRedLevelL() const;

    /** @brief Gets the current red parameter normalized level for the right channel. This method is thread-safe/atomic.*/
    float getRedLevelR() const;

    /** @brief Gets the current green parameter normalized level for the left channel. This method is thread-safe/atomic.*/
    float getGreenLevelL() const;

    /** @brief Gets the current green parameter normalized level for the right channel. This method is thread-safe/atomic.*/
    float getGreenLevelR() const;

    /** @brief Gets the current blue parameter normalized level for the left channel. This method is thread-safe/atomic.*/
    float getBlueLevelL() const;

    /** @brief Gets the current blue parameter normalized level for the right channel. This method is thread-safe/atomic.*/
    float getBlueLevelR() const;

    /** @brief Pops the next available stereo color sample from the decoded buffer. This method is thread-safe/atomic.
    
        > [!NOTE]
        > When using a @ref LumasonicStereoReader DO NOT call this method manually.
        > The reader will become the single non-audio consuming thread.

        @param sample               The sample reference that will be assigned the loaded values if any are available.
        @return                     True if a new sample was available, False if not.
    */
    bool popColorSample(StereoColorSample& sample);

    /** @brief Clears all decoded stereo color samples from the internal buffer. This method is thread-safe.
        @return                     The number of stereo color samples cleared.
    */
    int clearColorSamples();

    /** @brief Sets the reader process instance for the decoder. This method is thread-safe/atomic.
        
        This can be an instance of any class that implements the @ref LumasonicWaitingProcess
        interface. Typically, this will be an instance of class @ref LumasonicStereoReader.

        LumasonicWaitingProcess derived classes implement a LumasonicWaitingProcess::notify() method,
        that when called by the decoder, signals new decoded Lumasonic data is available to process.

        @ref LumasonicStereoReader instances will receive notifications from the decoder
    *   when the reader is configured with the event-driven **Efficient** thread mode 
        (@ref LumasonicThreadModes::LS_Thread_Event).
        
        > [!NOTE]
        > Although the event-driven thread mode is not strictly realtime audio safe,
        > in practice it often is, and it has great timing and efficiency characteristics.
        >
        > When using thread modes that are not event-driven, assigning a reader process
        > is unnecessary since the reader will read from the decoder with its own timing.
        >
        > When using a reader, DO NOT use popColorSample() manually. The reader will become the single
        > consuming non-audio thread.
    */
    void setReader(LumasonicWaitingProcess* readerProc);

    /** @brief Call this after removing all instances of @ref LumasonicStereoDecoder if a file logger was configured
        when creating any decoder instances. This prevents leaking of the file logger instance.*/
    static void stopLogging();

private:
    class Pimpl;
    Pimpl* pimpl = nullptr;
};