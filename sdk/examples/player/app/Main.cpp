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

#include <csignal>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>
#include <string>
#include <condition_variable>
#include <LumasonicDecoderApi.h>

using namespace std;

//==============================================================================
// Used to hold the main thread in a wait while the Lumasonic Codec and File Player run
std::condition_variable sigintCondition;
std::mutex sigintMutex;
bool sigintOccurred = false;

//==============================================================================
// Used to indicate to all threads that the file player task is finished.
std::atomic<bool> isFinished { false };

// Flag for whether a valid file was found as an input argument
bool validFileFound = false;

//==============================================================================
// SIGINT handler to handle ctrl + c shutdown signal
void handleSignal(int signal);

// Player update handler to simulate user/app interaction from a separate thread
void playerUpdate();

// Prints the usage of the command line tool
void printUsage();

//==============================================================================
int main(int argc, char* argv[])
{
    // We need at least one argument, the file to play
    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    // Register to handle process signals
    std::signal(SIGINT, handleSignal);

    // Initialize the Lumasonic codec
    //int retValue = ls_codec_init(nullptr, 0); // only output to console, no log file

    // Output to {USER SETTINGS DIR}/lsfplayer/lsfplayer.log and truncate the file at 128 * 1024 bytes
    int retValue = ls_codec_init("lsfplayer", 1024 * 128);

    if (retValue != 0)
    {
        cout << "Lumasonic codec failed to initialize." << endl;
        return 1;
    }

    // Create the lumasonic file player with default decoder and reader IDs and pass through command line args
    retValue = ls_player_create_with_args(ls_decoder_get_main_id(), ls_reader_get_main_id(), argc, argv);

    if (retValue != 0)
    {
        cout << "Lumasonic File Player failed to create." << endl;
        return 1;
    }

    // If the file player was not created, stop
    if (!ls_player_is_created())
    {
        cout << "Lumasonic File Player failed to create." << endl;
        return 1;
    }
    // If the player was created, load a file to play
    else if (argc > 0 && ls_player_is_file_loaded())
    {
        // For now just mark a state that there is a loaded valid file ready
        validFileFound = true;
    }

    // Start a thread to update and monitor the player
    std::thread updateTask(playerUpdate);

    // Start playing the loaded file if it is available
    if (validFileFound)
    {
        // Start playing the file stream
        ls_player_play();

        // Pause the current main thread and wait for the signal interrupt (SIGINT) event
        std::unique_lock<decltype(sigintMutex)> l(sigintMutex);
        sigintCondition.wait(l, [&] { return sigintOccurred; });
    }

    // Clean up and prepare for exit...
    sigintOccurred = false;

    // SIGINT was captured so it's time to tear things down
    isFinished.store(true);
    updateTask.join(); // wait for the monitoring thread to complete after isFinished signaled true

    // Clear the console if a file was just playing
    if (validFileFound)
        ls_codec_clear_console();

    // Shutdown the Lumasonic codec
    retValue = ls_codec_shutdown();

    if (retValue != 0)
    {
        cout << "Lumasonic codec failed to shut down." << endl;
        return 1;
    }

    // If no valid file was found, report on exit
    if (!validFileFound)
        cout << "No file found. The last argument should be the path of the file to play." << endl;

    // Final exit message
    if (validFileFound)
        ls_codec_clear_console();

    cout << "Lumasonic File Player exited" << endl;
    return 0;
}

//==============================================================================
void handleSignal(int signal)
{
    switch (signal)
    {
        case SIGINT:
            {
                std::unique_lock<decltype(sigintMutex)> l(sigintMutex);
                sigintOccurred = true;
            }
            sigintCondition.notify_one();
        break;
    }
}

//==============================================================================
void playerUpdate()
{
    while (!isFinished.load())
    {
        if (!ls_player_is_created())
            break;

        // Check to see if the player is finished with the currently file and no playlist, or the current playlist if there is one
        // If a webserver is running, ignore automatic exit since the UI could be up after the playlist finishes
        if (!ls_player_has_web_server() && ((!ls_player_has_playlist() && ls_player_has_stream_finished()) || ls_player_has_playlist_finished()))
        {
            // Update the SIGINT condition to fake ctrl + c to begin shutdown
            isFinished.store(true);
            std::unique_lock<decltype(sigintMutex)> l(sigintMutex);
            sigintOccurred = true;
            sigintCondition.notify_one();
            break;
        }

        // Sleep for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

//==============================================================================
void printUsage()
{
    cout << "Usage: lsfplayer [OPTIONS] [FILE]" << endl << endl << 
            "Plays a Lumasonic encoded file and displays decoded output and metrics." << endl << endl <<
            "Options first and the last argument is the absolute path of the audio file to play." << endl << endl <<
            "  --debug                  Takes over the console and prints debug values to it" << endl <<
            "  --loop                   Loops file playback, use ctrl + c to exit" << endl <<
            "  --thread-mode={MODE}     Sets the reader's thread mode: safe, efficient, performance" << endl <<
            "  --audio-settings={PATH}  Settings directory name or absolute path to a settings file" << endl <<
            "  --content-dir={PATH}     Lumasonic content directory loaded as a playlist" << endl <<
            "  --web-server={PORT}      Specifies the local port of the self-hosted web server API" << endl <<
            "  --web-dir={PATH}         Sets the path to static/root directory of the web server" << endl << endl <<
            "Create an AudioDeviceSettings.xml file for lsfplayer to configure its playback audio device." << endl <<
            endl;
}