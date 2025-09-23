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

#define LS_SAMPLE_RATE          48000.f
#define LS_AUDIO_BUFFER_SIZE    256
#define LS_SAMPLES_TO_DECODE    512

#include <iostream>
#include <iomanip>
#include <memory.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <LumasonicDecoder.h>
#include <LumasonicUtils.h>

using namespace std;

//==============================================================================
// Task to read decoded stereo color samples; read on a separate thread.
void readStereoColorSamples(LumasonicStereoDecoder* lsDecoder, std::atomic<bool>* isFinished);

//==============================================================================
// Main Entry
int main(/*int argc, char* argv[]*/)
{
    // Create a static Lumasonic encoder and create a decoder
    LsUtils::StaticLumasonicEncoder encoder;
    auto* lsDecoder = new LumasonicStereoDecoder();

    // Create the left and right float audio buffers to test encoding and decoding
    int numSamples = (int)LS_AUDIO_BUFFER_SIZE;
    float ch0[LS_AUDIO_BUFFER_SIZE];
    float ch1[LS_AUDIO_BUFFER_SIZE];

    // Clear the buffers to 0
    memset(ch0, 0, sizeof(float) * LS_AUDIO_BUFFER_SIZE);
    memset(ch1, 0, sizeof(float) * LS_AUDIO_BUFFER_SIZE);

    // Reset the codec to the proper sample rate and buffer size
    encoder.reset(LS_SAMPLE_RATE);
    lsDecoder->reset(LS_SAMPLE_RATE, numSamples);

    // Set the static encoder color values
    encoder.setStereoColor(1.f, 0.5f, 0.25f, 0.5f, 0.25f, 0.125f);

    // Start a second thread that will read decoded color values when they become avilable
    cout << endl << "Lumasonic encoding & decoding test threads running..." << endl;
    std::atomic<bool> isFinished{ false };
    std::thread readTask(readStereoColorSamples, lsDecoder, &isFinished);

    // Render a number of continuous encoded audio blocks
    while (!isFinished.load())
    {
        // For each audio buffer, encode values and then immediately decode that buffer
        encoder.processBlock(ch0, ch1, numSamples);
        lsDecoder->processBlock(ch0, ch1, numSamples);

        // Sleep a bit to simulate an audio callback thread
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // Ensure the reading task finishes...
    readTask.join();

    // Delete the decoder and finalize logging
    delete lsDecoder;
    LumasonicStereoDecoder::stopLogging();

    return 0;
}

//==============================================================================
// Color sample reading thread task.
void readStereoColorSamples(LumasonicStereoDecoder* lsDecoder, std::atomic<bool>* isFinished)
{
    if (lsDecoder == nullptr || isFinished == nullptr)
        return;

    // Prepare to read decoded color samples
    int totalSamples = 0;
    StereoColorSample sc;

    // While we wait to read in a specific number of color samples, keep reading...
    while (totalSamples < LS_SAMPLES_TO_DECODE)
    {
        // If there are decoded samples available, pop/consume them safely across threads
        while (lsDecoder->popColorSample(sc))
            ++totalSamples;

        cout << "Stereo color samples decoded: " << totalSamples << "/" << LS_SAMPLES_TO_DECODE << "\r";

        // Sleep a bit to simulate an update loop on a non-audio thread
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // The last/final color sample will still be in memory on the stack; log its values
    cout << endl << endl;
    cout << fixed << setprecision(5) <<
        "[Stereo Color]" << endl <<
        "r0:" << sc.r0 <<
        ", g0:" << sc.g0 <<
        ", b0:" << sc.b0 <<
        ", r1:" << sc.r1 <<
        ", g1:" << sc.g1 <<
        ", b1:" << sc.b1 << endl;

    cout << endl << "[AudioStrobe]" << endl << (lsDecoder->isAudioStrobe() ? "true" : "false") << endl << endl;

    // Signal that the reading thread is done
    isFinished->store(true);
}