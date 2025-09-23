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
// Utility class used as a custom listener
class CustomStereoColorListener : public LumasonicStereoColorListener
{
public:
    // A pointer to the decoder being used
    LumasonicStereoDecoder* lsDecoder = nullptr;

    // Implement this method on the listener to receive decoded data
    void onStereoColorRead(LumasonicRunningProcess& proc, StereoColorSample sc) override
    {
        // Increment the total number of samples read
        ++totalSamples;

        cout << "Stereo color samples decoded: " << totalSamples << "/" << LS_SAMPLES_TO_DECODE << "\r";

        // If we've read the target number of stereo color samples, stop the reader
        if (totalSamples == LS_SAMPLES_TO_DECODE)
        {
            proc.signalProcessShouldExit();

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

            if (lsDecoder != nullptr)
                cout << endl << "[AudioStrobe]" << endl << (lsDecoder->isAudioStrobe() ? "true" : "false") << endl << endl;
        }
    }

private:
    // The total number of stereo color samples read
    int totalSamples = 0;
};

//==============================================================================
// Main Entry
int main(/*int argc, char* argv[]*/)
{
    // Create a static Lumasonic encoder, a decoder, a reader, and a listener
    auto* lsDecoder = new LumasonicStereoDecoder();
    auto* lsReader = new LumasonicStereoReader();
    LsUtils::StaticLumasonicEncoder encoder;
    CustomStereoColorListener listener;

    // Hook up the reader and decoder
    lsReader->setDecoder(lsDecoder);
    lsDecoder->setReader(lsReader);
    lsReader->setThreadMode(LumasonicThreadModes::LS_Thread_Sleep);

    // Add the reader listener
    lsReader->addListener(&listener);
    listener.lsDecoder = lsDecoder;

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
    cout << endl << "Lumasonic encoding & reader test threads running..." << endl;

    // Start listening thread
    lsReader->start();

    // Render a number of continuous encoded audio blocks
    while (lsReader != nullptr && lsReader->isRunning())
    {
        // For each audio buffer, encode values and then immediately decode that buffer
        encoder.processBlock(ch0, ch1, numSamples);
        lsDecoder->processBlock(ch0, ch1, numSamples);

        // Sleep a bit to simulate an audio callback thread
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // Shutdown and delete the reader
    lsReader->stop();
    lsReader->removeListener(&listener);
    lsReader->setDecoder(nullptr);
    delete lsReader;

    // Remove the listener pointer to the decoder
    listener.lsDecoder = nullptr;

    // Delete the decoder and finalize logging
    lsDecoder->setReader(nullptr);
    delete lsDecoder;
    LumasonicStereoDecoder::stopLogging();

    return 0;
}