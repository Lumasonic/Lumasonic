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
#include <cmath>

// 2 * PI
#ifndef M_2PI
#define M_2PI       6.283185307179586476925286766559005768394338798750211642
#endif

// 20 over LN10 (for volume conversion float to dB)
#ifndef M_LN10_20
#define M_LN10_20   8.6858896380650365530225783783321
#endif

// LN10 over 20 (for volume conversion dB to float)
#ifndef M_20_LN10
#define M_20_LN10   0.11512925464970228420089957273422
#endif

// The default decibel level of the Lumasonic reference tone for encoding
#ifndef LS_DEFAULT_REF_TONE_DB
#define LS_DEFAULT_REF_TONE_DB -32.f
#endif

namespace LsUtils
{
    //==============================================================================
    // @brief Turns decibels into float gain factor
    template <typename FloatType>
    inline const FloatType dbToGain(const FloatType& dB)
    {
        return static_cast<FloatType>(std::exp(dB * (FloatType)M_20_LN10));
    }

    // @brief Turns float gain factor into decibels
    template <typename FloatType>
    inline const FloatType gainToDb(const FloatType& gain, FloatType infDb = -100)
    {
        if (gain < (FloatType)0.0000000298023223876953125)
            return infDb;

        const FloatType dB = std::log(gain) * (FloatType)M_LN10_20;
        return (dB > infDb) ? dB : infDb;
    }

    /** @brief De-interleaves 32-bit PCM float audio from a single interleaved stereo buffer into two separate left and right channel buffers of half the size.
        @param input                The left/right interleaved stereo audio samples.
        @param outL                 The pointer to the buffer that will contained the de-interleaved left samples.
        @param outR                 The pointer to the buffer that will contained the de-interleaved right samples.
        @param samplesPerChannel    The total number of de-interleaved audio samples per channel (for stereo this is half of the total interleaved samples of the input).
        @return                     True if the process was successful, False it not.
    */
    template <typename FloatType>
    inline bool deinterleaveStereoAudio(const FloatType* input, FloatType* outL, FloatType* outR, int samplesPerChannel)
    {
        if (input == nullptr || outL == nullptr || outR == nullptr || samplesPerChannel <= 0)
            return false;

        int sampleCount = 0;
        int outChannels = 2; // stereo

        for (int s = 0; s < samplesPerChannel; ++s)
        {
            outL[sampleCount] = input[s * outChannels];
            outR[sampleCount++] = input[s * outChannels + 1];
        }

        return true;
    }

    /** @brief De-interleaves 32-bit PCM float audio from a single interleaved stereo buffer into four separate channel buffers 1/4th the size of the input.
        @param input                The interleaved four channel audio sample buffer.
        @param out0                 The pointer to channel 0's de-interleaved sample buffer.
        @param out1                 The pointer to channel 1's de-interleaved sample buffer.
        @param out2                 The pointer to channel 2's de-interleaved sample buffer.
        @param out3                 The pointer to channel 3's de-interleaved sample buffer.
        @param samplesPerChannel    The total number of de-interleaved audio samples per channel (for four channels this is a quarter of the total interleaved samples of the input).
        @return                     True if the process was successful, False it not.
    */
    template <typename FloatType>
    inline bool deinterleaveQuadAudio(const FloatType* input, FloatType* out0, FloatType* out1, FloatType* out2, FloatType* out3, int samplesPerChannel)
    {
        if (input == nullptr || out0 == nullptr || out1 == nullptr || out2 == nullptr || out3 == nullptr || samplesPerChannel <= 0)
            return false;

        int sampleCount = 0;
        int outChannels = 4; // quad channel audio

        for (int s = 0; s < samplesPerChannel; ++s)
        {
            out0[sampleCount]   = input[s * outChannels];
            out1[sampleCount]   = input[s * outChannels + 1];
            out2[sampleCount]   = input[s * outChannels + 2];
            out3[sampleCount++] = input[s * outChannels + 3];
        }

        return true;
    }

    //==============================================================================
    /**
     * @brief Utility class for generating a sine wave tone at a given
     * frequency, amplitude, and phase.
     * 
     * @details
     * This is an unoptimized and minimal design that can be used to
     * encode Lumasonic signals.
     * 
     * ### Initializing the Generator
     * 
     * Before generating a tone, initialize to a specific sample rate using the `reset(double)` method.
     * 
     * ```c++
     * 
     * ToneGenerator tone;
     * tone.reset(48000.); // initialize to 48 KHz sample rate
     * 
     * ```
     * 
     * ### Setting Values
     * 
     * You can get and set the frequency, amplitude, and phase of the tone:
     * 
     * ```c++
     * 
     * tone.setFrequency(18000.);   // set to 18000 Hz
     * tone.setAmplitude(0.5);      // set to 50% output
     * tone.setPhase(0.5);          // normalized 0.0 - 1.0
     * 
     * ```
     * 
     * ### Generating Audio Samples
     * 
     * To generate a new sample of the tone call the `nextSample()` method:
     * 
     * ```c++
     * 
     * double sample = tone.nextSample(); // put these samples into your audio buffers to produce the tone
     * 
     * ```
     * 
     */
    class ToneGenerator
    {
    public:
        /** @brief Constructor*/
        ToneGenerator()
        {
            setPhase(0.);
            setAmplitude(0.25);
            setFrequency(1.);
        }

        /** @brief Sets the signal's amplitude.
            @param newAmplitude The new amplitude to set for the tone generator
        */
        inline void setAmplitude(double newAmplitude) { amplitude = newAmplitude; }

        /** @brief Get's the tone's amplitude. */
        inline double getAmplitude() const { return amplitude; }

        /** @brief Sets the signal's frequency.
            @param newFrequencyHz The new frequency to set in cycles per second (Hz)
        */
        void setFrequency(double newFrequencyHz)
        {
            frequency = newFrequencyHz;
            phasePerSample = double(M_2PI) / (sampleRate / frequency);
        }

        /** @brief Get's the tone's frequency. */
        inline double getFrequency() const { return frequency; }

        /** @brief Resets the tone generator state.
            @param newSampleRate The sample rate to use
        */
        void reset(double newSampleRate)
        {
            currentPhase = 0.0;
            sampleRate = newSampleRate;
            phasePerSample = double(M_2PI) / (sampleRate / frequency);
        }

        /** @brief Gets the next available sample */
        inline double nextSample()
        {
            double sample = amplitude * std::sin(currentPhase);
            currentPhase += phasePerSample;
            return sample;
        }

        /** @brief Sets the phase of the waveform using a 0.0 - 1.0 normalized value.
        *   @param phase    The new phase to set (0.0 - 1.0)
        */
        inline void setPhase(double phase)
        {
            currentPhase = phase * double(M_2PI);
        }

    private:
        double frequency;
        double sampleRate;
        double currentPhase;
        double phasePerSample;
        double amplitude;
    };

    //==============================================================================
    /**
     * @brief A utility class for generating static Lumasonic signals.
     * 
     * @details
     * This class generates the necessary carrier tones to encode a static set
     * of Lumasonic color values into an audio buffer. It can be used for
     * continuous encoding.
     * 
     * ### Setting the Values to Encode
     * 
     * The stereo color values to encode can be set using the `setStereoColor()` method:
     * 
     * ```c++
     * 
     * StaticLumasonicEncoder lsEncoder;
     * lsEncoder.setStereoColor(1.0f, 0.5f, 0.25f, 0.5f, 0.25f, 0.125f);    // normalized floating point rgb values
     * 
     * ```
     * 
     * > [!NOTE]
     * > Although you can update the encoded stereo color values during encoding,
     * > this simple encoder class is only intended to encode a static set of values
     * > into an audio stream.
     * > 
     * > Modifying the stereo color values too quickly or drastically can create 
     * > high frequency aliasing in the signal. Aliasing can introduce unintended 
     * > noise into the performance of the codec.
     * > A complete Lumasonic encoder also needs to eliminate aliasing caused by
     * > rapid modulation of the tone levels.
     * 
     * ### Initializing an Encoder
     * 
     * Before you use an encoder instance, initialize it with a sample rate using the `reset(float)` method.
     * 
     * ```c++
     * 
     * lsEncoder.reset(48000.f);
     * 
     * ```
     * 
     * ### Encoding into an Audio Stream
     * 
     * The `processBlock(float*, float*, int, bool);` method performs the
     * actual encoding.
     * 
     * The first two arguments should be pointers to floating point audio buffers.
     * 
     * The third argument is the number of samples to encode into the buffers.
     * 
     * The final boolean argument (optional) determines if the encoded samples will 
     * replace samples in the buffer or mix additively with them.
     * 
     * ```c++
     * 
     * lsEncoder.processBlock(bufferL, bufferR, numSamples);        // defaults to replace: true 
     * lsEncoder.processBlock(bufferL, bufferR, numSamples, true);  // overwrite each sample in the buffers with the new encoded samples
     * lsEncoder.processBlock(bufferL, bufferR, numSamples, false); // additively mix new samples with the existing data in the buffers
     * 
     * ```
     * 
     */
    class StaticLumasonicEncoder
    {
    public:
        /** @brief Constructor
            @param maxLevelDb       Sets the decibel level that equals 100% output for the encoder's carrier tones.
        */
        StaticLumasonicEncoder(float maxLevelDb = LS_DEFAULT_REF_TONE_DB) 
            : cr0(0.f), cg0(0.f), cb0(0.f), cr1(0.f), cg1(0.f), cb1(0.f)
        {
            toneF.setAmplitude((double)dbToGain(maxLevelDb));               // reference to to full gain
            toneR0.setAmplitude(0.);                                        // color channel r to 0%
            toneG0.setAmplitude(0.);                                        // color channel g to 0%
            toneB0.setAmplitude(0.);                                        // color channel b to 0%
            toneR1.setAmplitude(0.);                                        // color channel r to 0%
            toneG1.setAmplitude(0.);                                        // color channel g to 0%
            toneB1.setAmplitude(0.);                                        // color channel b to 0%

            // Set each tone's unique frequency
            toneF.setFrequency(LS_REF_TONE_FREQ);       // reference tone
            toneR0.setFrequency(LS_RED_TONE_FREQ);      // left red
            toneG0.setFrequency(LS_GREEN_TONE_FREQ);    // left green
            toneB0.setFrequency(LS_BLUE_TONE_FREQ);     // left blue
            toneR1.setFrequency(LS_RED_TONE_FREQ);      // right red
            toneG1.setFrequency(LS_GREEN_TONE_FREQ);    // right green
            toneB1.setFrequency(LS_BLUE_TONE_FREQ);     // right blue
        }

        /** @brief Sets the current stereo color values of the encoder. The color values set
            will be Lumasonic encoded into the provided buffers when calling processBlock() @see processBlock.
        
            @param r0               The left red channel value.
            @param g0               The left green channel value.
            @param b0               The left blue channel value.
            @param r1               The right red channel value.
            @param g1               The right green channel value.
            @param b1               The right blue channel value.
        */
        void setStereoColor(float r0, float g0, float b0, float r1, float g1, float b1)
        {
            cr0 = r0;   cr1 = r1;
            cg0 = g0;   cg1 = g1;
            cb0 = b0;   cb1 = b1;
            auto refLevel = dbToGain(LS_DEFAULT_REF_TONE_DB);
            toneR0.setAmplitude((double)refLevel * (double)cr0);
            toneG0.setAmplitude((double)refLevel * (double)cg0);
            toneB0.setAmplitude((double)refLevel * (double)cb0);
            toneR1.setAmplitude((double)refLevel * (double)cr1);
            toneG1.setAmplitude((double)refLevel * (double)cg1);
            toneB1.setAmplitude((double)refLevel * (double)cb1);
        }

        /** @brief Resets the static encoder with the given sample rate.
            @param sampleRate       The sample rate to reset the encoder to.
        */
        void reset(float sampleRate)
        {
            toneF.reset(sampleRate);
            toneR0.reset(sampleRate);   toneR1.reset(sampleRate);
            toneG0.reset(sampleRate);   toneG1.reset(sampleRate);
            toneB0.reset(sampleRate);   toneB1.reset(sampleRate);
        }

        /** @brief Encodes the current stereo color values to two audio floating point buffers for a given number of samples.
            @param in0              The left audio buffer to encode into.
            @param in1              The right audio buffer to encode into.
            @param numSamples       The number of samples to encode into the buffers.
            @param replace          When True, samples in the buffer will be replaced, when False encoded samples will be mixed with the existing buffer data.
        */
        void processBlock(float* in0, float* in1, int numSamples, bool replace = true)
        {
            if (replace)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    auto f  = toneF.nextSample();
                    auto r0 = toneR0.nextSample();  auto r1 = toneR1.nextSample();
                    auto g0 = toneG0.nextSample();  auto g1 = toneG1.nextSample();
                    auto b0 = toneB0.nextSample();  auto b1 = toneB1.nextSample();
                    in0[i] = (float)(f + r0 + g0 + b0);
                    in1[i] = (float)(f + r1 + g1 + b1);
                }
            }
            else
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    auto f  = toneF.nextSample();
                    auto r0 = toneR0.nextSample();  auto r1 = toneR1.nextSample();
                    auto g0 = toneG0.nextSample();  auto g1 = toneG1.nextSample();
                    auto b0 = toneB0.nextSample();  auto b1 = toneB1.nextSample();
                    in0[i] += (float)(f + r0 + g0 + b0);
                    in1[i] += (float)(f + r1 + g1 + b1);
                }
            }
        }

    private:
        float cr0, cg0, cb0, cr1, cg1, cb1; // left & right red, green, and blue values
        ToneGenerator toneF;                // reference tone
        ToneGenerator toneR0;               // left red tone
        ToneGenerator toneG0;               // left green tone
        ToneGenerator toneB0;               // left blue tone
        ToneGenerator toneR1;               // right red tone
        ToneGenerator toneG1;               // right green tone
        ToneGenerator toneB1;               // right blue tone
    };

} // namespace Lumasonic::Utils


