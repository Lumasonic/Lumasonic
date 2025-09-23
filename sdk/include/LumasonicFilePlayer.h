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

// Enables or disables the @ref LumasonicFilePlayer class and associated API functions.
#ifndef LS_USE_FILE_PLAYER
#define LS_USE_FILE_PLAYER 1
#endif

// Enables or disables the @ref LumasonicFilePlayer class implementing a self-hosted web server.
#ifndef LS_USE_FILE_PLAYER_WWW
#define LS_USE_FILE_PLAYER_WWW 1
#endif

#if LS_USE_FILE_PLAYER

#include "LumasonicCommon.h"
#include "LumasonicStereoDecoder.h"
#include "LumasonicStereoReader.h"

//==============================================================================
/**
 * @brief A complete implementation of file-based audio player that can be
 * connected to a @ref LumasonicStereoDecoder to decode Lumasonic
 * audio files.
 *
 * @details
 * This class is a self-contained audio file player with basic transport controls.
 * 
 * In addition to audio file playback and transport, the player decodes Lumasonic
 * audio in realtime and publishes the decoded values and performance metrics.
 * 
 * This class can be incorporated into other projects and products if a turn key
 * audio file player that can decode Lumasonic signals is needed.
 * 
 * Adding custom [listeners](@ref LumasonicStereoColorListener) allows the data
 * decoded by the player to be processed by custom functions.
 * 
 * Optionally, when this class is started with the @ref start() method, it can be
 * passed an argument: `--debug`. This argument will cause the player
 * to take over the standard output console and print debug values and metrics.
 * 
 * ## Creating a Player
 * 
 * ```c++
 * 
 * // Create a player instance that uses internally managed decoder and reader
 * // Uses the default settings directory name "lsfplayer" to load audio device settings from
 * auto* lsPlayer = new LumasonicFilePlayer();
 * 
 * ```
 * 
 * ```c++
 * 
 * // Create a player instance that does not use an external decoder or reader (nullptr, nullptr)
 * // Uses the settings directory name "lsfplayer" to load audio device settings from (change it to use a custom dir name)
 * auto* lsPlayer = new LumasonicFilePlayer(nullptr, nullptr, "lsfplayer");
 * 
 * ```
 * 
 * ## Creating a Player with an External Decoder and Reader
 * 
 * ```c++
 * // Typically the decoder and reader instances will be declared already somewhere else
 * auto* lsDecoder = new LumasonicStereoDecoder();
 * auto* lsReader = new LumasonicStereoReader();
 *  
 * // Create a player instance that uses an external decoder and reader
 * // Uses the settings directory name "lsfplayer" to load audio device settings from (change it to use a custom dir name)
 * auto* lsPlayer = new LumasonicFilePlayer(lsDecoder, lsReader, "lsfplayer");
 * 
 * ```
 * 
 * ## Initializing a Player
 * 
 * In order to initialize an audio device for playback, and to load audio files, 
 * the player must be started using the @ref start() method.
 * 
 * ```c++
 * 
 * lsPlayer->start();           // start a player without any command line arguments
 * lsPlayer->start(0, nullptr); // start a player without any command line arguments explicitly
 
 * ```
 *
 * ### Audio Device Settings
 * 
 * > [!NOTE]
 * > In order to tell the @ref LumasonicFilePlayer class what audio device and driver to use,
 * > you must configure a **AudioDeviceSettings.xml** file in a known location.
 * 
 * The player class attempts to load audio device and driver settings from a file named **AudioDeviceSettings.xml**
 * 
 * OS       | Path
 * ---------|--------------------------------------------------------------------------------
 * Windows  | C:\\Users\\{CURRENT USER}\\AppData\\Roaming\\{SETTINGS DIR}\\AudioDeviceSettings.xml
 * macOS    | ~/Library/{SETTINGS DIR}/AudioDeviceSettings.xml
 * Linux    | ~/.config/{SETTINGS DIR}/AudioDeviceSettings.xml
 * 
 * It is possible to use a custom value in place of "lsfplayer" for the settings directory name
 * by configuring the player when you create it.
 * 
 * Alternatively, you can supply the absolute path to an AudioDeviceSettings.xml file using the **--audio-settings** command line argument.
 * 
 * ### Creating an AudioDeviceSettings.xml File
 * 
 * To create an AudioDeviceSettings.xml run the [LumasonicFilePlayer tool](./LumasonicFilePlayer.md).
 * 
 * Select the audio device driver to use, select 48000 Hz sample rate, and 128-256 sample buffer size.
 * 
 * When you exit the [LumasonicFilePlayer tool](./LumasonicFilePlayer.md), it will save out a compatible AudioDeviceSettings.xml
 * file to its own settings directory found here:
 * 
 * OS       | Path
 * ---------|--------------------------------------------------------------------------------
 * Windows  | C:\\Users\\{CURRENT USER}\\AppData\\Roaming\\LumasonicFilePlayer\\AudioDeviceSettings.xml
 * macOS    | ~/Library/LumasonicFilePlayer/AudioDeviceSettings.xml
 * Linux    | ~/.config/LumasonicFilePlayer/AudioDeviceSettings.xml
 * 
 * Copy the AudioDeviceSettings.xml from the [LumasonicFilePlayer tool ](./LumasonicFilePlayer.md) and paste it into
 * the settings directory the player was configured to read from (for example: **lsfplayer** in examples above).
 * 
 * ### Pass Through Command Arguments
 * 
 * It is also possible to pass in command line arguments to the @ref start(int, char*[]) method.
 * 
 * Argument                 | Required Value                       | Description
 * -------------------------|--------------------------------------|-------------------------------------------
 * --debug                  |                                      | takes over the console and prints debug values to it
 * --loop                   |                                      | causes the current file to loop playback
 * --thread-mode            | `safe`, `efficient`, `performance`   | sets the reader thread mode
 * --audio-settings         | absolute file path or directory name | sets the settings directory or settings file path to load audio device settings from
 * --content-dir            | absolute directory path              | sets a directory with audio files to mount as a playlist
 * --web-server             | web server port number               | enables a web server API to control the player via REST API
 * --web-dir                | absolute directory path              | sets the web server's root/static content directory path
 * 
 * 
 * ```c++
 * 
 * int main(int argc, char* argv[])
 * {
 *      auto* lsPlayer = new LumasonicFilePlayer(); // a new player with internal decoder and reader and default settings
 *      lsPlayer->start(argc, argv); // starts the player with the command line arguments "passed through"
 * }
 * 
 * ```
 * 
 * When a command line argument requires a value, it should have an equals sign and no spaces:
 * 
 * ```shell
 * lsfplayer --debug --loop --thread-mode=efficient --audio-settings=lsfplayer
 * ```
 * 
 * ### Argument Examples
 * 
 * Below are some examples of command line arguments that can be used with the player.
 *
 * > [!NOTE]
 * > These examples assume you have already placed an **AudioDeviceSettings.xml** file into the **lsfplayer** user settings directory,
 * > except when audio device settings are configured directly using the **--audio-settings** argument.
 * 
 * #### Playing a file (minimal)
 * 
 * ```shell
 * lsfplayer ~/media/file.flac
 * ```
 * #### Playing a file (looping)
 * 
 * ```shell
 * lsfplayer --loop ~/media/file.flac
 * ```
 * 
 * #### Playing a file (debug mode)
 * 
 * ```shell
 * lsfplayer --debug ~/media/file.flac
 * ```
 * 
 * #### Specifying a custom user settings directory name
 * 
 * An **AudioDeviceSettings.xml** file is loaded automatically from this settings directory. (see **Audio Device Settings** above).
 * 
 * ```shell
 * lsfplayer --audio-settings=myapp ~/media/file.flac
 * ```
 *
 * #### Specifying a custom audio device settings file directly
 * 
 * You can also directly specify an **AudioDeviceSettings.xml** file to use.
 *
 * ```shell
 * lsfplayer --audio-settings=/path/to/AudioDeviceSettings.xml ~/media/file.flac
 * ```
 * 
 * #### Playing a directory of files as a playlist
 * 
 * Specify a playlist content directory with the **--content-dir** argument. All files of type **.flac**, **.wav**, or **.ogg**
 * will be added to the playlist by scanning the directory. 
 * 
 * The last argument should be the initial file to play. This file should exist in the content directory you specified.
 * 
 * ```shell
 * lsfplayer --content-dir=~/media ~/media/file.flac
 * ```
 * 
 * #### Playing with a specific thread mode
 * 
 * ```shell
 * lsfplayer --thread-mode=safe ~/media/file.flac
 * ``` 
 * 
 * ```shell
 * lsfplayer --thread-mode=efficient ~/media/file.flac
 * ``` 
 *
 * ```shell
 * lsfplayer --thread-mode=performance ~/media/file.flac
 * ```
 *
 * #### Running a web server and REST API
 *
 * To launch a self-hosted web server with a REST API to the player, specify any non-zero port number to the **--web-server** argument.
 *
  * ```shell
 * lsfplayer --web-server=8080 ~/media/file.flac
 * ```
 *
 * > [!NOTE]
 * > If you intend to have the player serve its own web interface, you must also specify a static web directory with the **--web-dir** argument.
 *
 * ```shell
 * lsfplayer --web-server=8080 --web-dir=~/www/root ~/media/file.flac
 * ```
 *
 * #### Running a playlist web player interface with custom audio settings and debugging output to the console
 *
 * ```shell
 * lsfplayer --debug --thread-mode=efficient --audio-settings=~/path/to/AudioDeviceSettings.xml --content-dir=~/media --web-server=8080 --web-dir=~/www/root ~/media/file.flac
 * ```
 *
 * In this example:
 *
 * - debug output is printed to the console
 * - the "efficient" thread mode is used for reading decoded values
 * - ~/path/to/AudioDeviceSettings.xml is used as the custom audio device settings file for playback
 * - ~/media is used as the playlist content directory
 * - the web server runs locally on port 8080
 * - ~/www/root is used to serve the static files of the web player interface
 * 
 * ## Shutting Down a Player
 * 
 * > [!NOTE]
 * > Player instances need to be shutdown proprely before they are deleted. Before deleting player instances, call the @ref shutdown() method.
 * 
 * ```c++
 * 
 * // If start() is called, make sure to call shutdown() before deleting the instance
 * lsPlayer->shutdown();
 * 
 * ```
 * 
 * ## Loading a File
 * 
 * Load a new file to play by using the method: @ref loadFile()
 * 
 * ```c++
 * 
 * // Load a new file and store the result
 * int result = lsPlayer->loadFile("~/media/LumasonicTrack1.flac");
 * 
 * // Print a message if there was an error loading the file
 * if (result != 0)
 *      std::cout << "Failed to load file with error: " << result << std::endl;
 * 
 * ```
 * 
 * ## Transport Controls
 * 
 * ### Playback
 * 
 * To control audio file playback use the methods: @ref play(), @ref pause(), @ref stop().
 * 
 * ```c++
 * 
 * lsPlayer->play();   // plays the stream from its current position
 * lsPlayer->pause();  // pauses the stream at its current position
 * lsPlayer->stop();   // stops the stream and resets its position to 0
 * 
 * ```
 * 
 * ### Time
 * 
 * To get the current time of the audio stream use the methods: @ref getPlayTimeSeconds() and @ref getPlayTimePercent().
 * 
 * ```c++
 * 
 * // This gets the total play time of the current audio file stream in total seconds
 * double playTimeSec = lsPlayer->getPlaytimeSeconds();
 * 
 * // This gets the total play time of the current audio file stream as a normalized percent
 * double playTimePercent = lsPlayer->getPlaytimePercent(); // 0.0 to 1.0
 * 
 * ```
 * 
 * To change the time of the audio stream use the methods: @ref setPlayTimeSeconds() and @ref setPlayTimePercent().
 * 
 * ```c++
 * 
 * lsPlayer->setPlaytimeSeconds(120);  // jump to 120 seconds (2 minutes) into the audio stream
 * lsPlayer->setPlaytimePercent(0.5);  // jump to 50% of the audio stream's total duration
 *
 * ```
 * 
 * ### Duration
 * 
 * To get the duration of the current audio file in seconds use the method: @ref getLengthInSeconds().
 * 
 * ```c++
 * 
 * // Gets the duration of the current file in total seconds
 * double duration = lsPlayer->getLengthInSeconds();
 * 
 * 
 * ```
 * 
 * ## Output Gain
 * 
 * You can interact with the player's output gain by using the methods: @ref getGain() and @ref setGain().
 * 
 * ```c++
 * 
 * // Get the current output gain value
 * float outputGain = lsPlayer->getGain();
 * 
 * // Update to half of the previous output gain
 * lsPlayer->setGain(outputGain * 0.5f);
 * 
 * ```
 * 
 * ## Audio Stream
 * 
 * You can interact with the audio stream using the following methods: 
 *
 * Method                   | Description
 * -------------------------|------------------------------------------------
 * @ref isFileLoaded()      | returns **true** after a valid file has loaded
 * @ref isPlaying()         | returns **true** if the stream is currently playing
 * @ref isLooping()         | returns **true** if the stream will loop after finishing
 * @ref setLooping()        | sets whether the stream will loop
 * @ref hasStreamFinished() | returns **true** after the stream finishes playing
 * 
 * ## Web Server API
 * 
 * The @ref LumasonicFilePlayer class can launch a self-hosted simple web server that
 * provides control of the player using a REST API over HTTP.
 * 
 * There are two ways to enable the web server:
 * 
 * 1. Pass a non-zero web server port number to the constructor of @ref LumasonicFilePlayer.
 * 2. Pass a command line argument to the @ref start(int, char*[]) method: --web-server=8080
 * 
 * Currently, the web server only works with HTTP and without authentication.
 * 
 * > [!NOTE]
 * > All API requests and responses use JSON unless otherwise noted.
 * 
 * ---
 * 
 * **POST /api/v1/file/load**
 * 
 * Loads a new file into the player for playback.
 * 
 * Argument    | Type     | Required | Description
 * ------------|----------|----------|------------
 * **path**    | string   | yes      | the absolute path of the file to play
 *
 * Request:
 * 
 * ```json
 * { "path": "path_on_local_file_system_file.flac" }
 * ```
 *
 * Response:
 * 
 * ```json
 * { "success": true }
 * ```
 * 
 * ---
 * 
 * **GET | POST /api/v1/file/loaded**
 * 
 * Returns whether the player has loaded a valid file or not.
 * 
 * Response:
 * 
 * ```json
 * { "success": true, "value": true|false }
 * ```
 * 
 * ---
 * 
 * **GET | POST /api/v1/file/name**
 * 
 * Gets the name of the currently loaded file, if any.
 * 
 * Response:
 * 
 * ```json
 * { "success": true, "value": "current_file_name.flac" }
 * ``` 
 * 
 * --
 * 
 * **GET | POST /api/v1/transport/play**
 * 
 * Plays (or resumes playback of) the current file.
 * 
 * Response:
 * 
 * ```json
 * { "success": true }
 * ```
 * 
 * ---
 * 
 * **GET | POST /api/v1/transport/pause**
 * 
 * Pauses playback of the current file.
 * 
 * Response:
 * 
 * ```json
 * { "success": true }
 * ```
 * 
 * ---
 * 
 * **GET | POST /api/v1/transport/stop**
 * 
 * Stops playback of the current file and returns the time/position to the beginning of the file.
 * 
 * Response:
 * 
 * ```json
 * { "success": true }
 * ```
 * 
 * ---
 * 
 * **GET /api/v1/transport/time**
 * 
 * Gets the current playback time of the transport in seconds or percent complete 
 * (normalized) and returns the duration of the current stream in total seconds.
 * 
 * Argument    | Type     | Required | Description
 * ------------|----------|----------|------------
 * **unit**    | string   | yes      | specifies the time unit to use: sec, % 
 *
 * Request:
 * 
 * ```json
 * { "unit": "sec" } or { "unit": "%" }
 * ```
 *
 * Response:
 * 
 * ```json
 * { "success": true, "value": 120.5, "length": 480 } or { "success": true, "value": 0.5, "length" : 480}
 * ```
 * 
 * ---
 * 
 * **POST /api/v1/transport/time**
 * 
 * Sets the current playback time of the transport in total seconds or percent complete (normalized).
 * 
 * Argument    | Type     | Required | Description
 * ------------|----------|----------|------------
 * **unit**    | string   | yes      | specifies the time unit to use: sec, % 
 * **time**    | string   | yes      | the time value to set in total seconds or percent (0.0 - 1.0)
 *
 * Request:
 * 
 * ```json
 * { "unit": "sec", "time": 240.0 } or { "unit": "%", "value": 0.25 }
 * ```
 *
 * Response:
 * 
 * ```json
 * { "success": true }
 * ```
 * 
 * ---
 * 
 * **GET /api/v1/gain**
 * 
 * Gets the output gain of the player.
 * 
 * Response:
 * 
 * ```json
 * { "success": true, "value": 1.0 }
 * ```
 * 
 * ---
 * 
 * **POST /api/v1/gain**
 * 
 * Sets the output gain of the player.
 * 
 * Argument    | Type     | Required | Description
 * ------------|----------|----------|------------
 * **gain**    | double   | yes      | specifies the output gain value to set (0.0 = 0%, 1.0 = 100%)
 *
 * Request:
 * 
 * ```json
 * { "gain": "0.5" }
 * ```
 * 
 * Response:
 * 
 * ```json
 * { "success": true }
 * ```
 * 
 * ---
 * 
 * **GET | POST /api/v1/stream/playing**
 * 
 * Gets whether the player is currently playing a file.
 * 
 * Response:
 * 
 * ```json
 * { "success": true, "value": true|false }
 * ```
 * 
 * ---
 * 
 * **GET /api/v1/stream/loop**
 * 
 * Gets whether the player is set to loop playback.
 * 
 * Response:
 * 
 * ```json
 * { "success": true, "value": true|false }
 * ```
 * 
 * ---
 * 
 * **POST /api/v1/stream/loop**
 * 
 * Sets whether the player should loop playback.
 * 
 * Argument    | Type     | Required | Description
 * ------------|----------|----------|------------
 * **loop**    | boolean  | yes      | whether playback should loop or finish after one play through
 *
 * Request:
 * 
 * ```json
 * { "loop": true|false }
 * ``` 
 * 
 * Response:
 * 
 * ```json
 * { "success": true }
 * ```
 * 
 * ---
 *
 * **GET | POST /api/v1/stream/finished**
 * 
 * Gets whether playback of the current audio stream has finished. **When looping, this never returns true**.
 * 
 * Response:
 * 
 * ```json
 * { "success": true, "value": true|false }
 * ```
 * 
 * **GET | POST /api/v1/player/state**
 * 
 * Gets a snapshot of the current player state and returns it.
 * 
 * Response:
 * 
 * ```json
 * {
 *  "success": true,
 *  "timeSec": 44.9281,
 *  "timePercent": 0.0963531,
 *  "lengthSec": 466.286,
 *  "playing": true,
 *  "looping": true,
 *  "fileLoaded": true,
 *  "streamFinished": false,
 *  "gain": 1.0,
 *  "fileName": "Invokation.mixed.flac"
 * }
 * ```
 * 
 * When a playlist is present, the state will return playlist information as well.
 * 
 * ```json
 * {
 *  "success": true,
 *  "timeSec": 44.9281,
 *  "timePercent": 0.0963531,
 *  "lengthSec": 466.286,
 *  "playing": true,
 *  "looping": true,
 *  "fileLoaded": true,
 *  "streamFinished": false,
 *  "gain": 1.0,
 *  "fileName": "Invokation.mixed.flac",
 *  "playlist": { "index": 1, "numItems": 12, "isFinished": true|false }
 * }
 * ```
 * 
 * ---
 * 
 * **GET | POST /api/v1/playlist/state**
 * 
 * Gets a snapshot of the current playlist state and returns it.
 * 
 * Response:
 * 
 * ```json
 * { "success":true, "hasPlaylist": true|false, "isFinished": true|false, index:1, numItems: 10 }
 * ```
 * 
 * ---
 * 
 * **GET | POST /api/v1/playlist/items**
 * 
 * If a playlist is loaded, gets the names of the audio files in the playlist in an array and returns it.
 * 
 * Response:
 * 
 * ```json
 * { "success":true, "items":["file_name_1.flac", "file_name_2.wav", "file_name_3.ogg"] }
 * ```
 * 
 * ---
 * 
 * **POST /api/v1/load/playlist/item**
 * 
 * If a playlist is loaded, loads a file from the playlist given the file's index in the playlist (0-based).
 * 
 * Argument    | Type     | Required | Description
 * ------------|----------|----------|------------
 * **index**   | integer  | yes      | the index of the file in the playlist to load
 *
 * Request:
 * 
 * ```json
 * { "index": 3 }
 * ```  
 * 
 * Response:
 * 
 * ```json
 * { "success":true }
 * ``` 
 */
class LumasonicFilePlayer
{
public:
    //==============================================================================
    /** @brief Constructor
        
        @details
        Creates an instance of a @ref LumasonicFilePlayer, optionally providing
        a custom [decoder](@ref LumasonicStereoDecoder), [reader](@ref LumasonicStereoReader), and settings directory name to use.

        @param decoder          A pointer to an external decoder instance to use. If left NULL, an internal decoder will be created and managed. 
        @param reader           A pointer to an external reader instance to use. If left NULL, an internal reader will be created and managed.
        @param settingsDirName  When not NULL, specifies a custom directory name to use for file player user settings.
        @param webServerPort    When a non-zero value is used, enables a web server API on the specified port number.
    */
    LumasonicFilePlayer(LumasonicStereoDecoder* decoder = nullptr, LumasonicStereoReader* reader = nullptr, 
        const char* settingsDirName = nullptr, unsigned short webServerPort = 0);

    /** @brief Destructor*/
    ~LumasonicFilePlayer();

    //==============================================================================
    /** @brief Starts the host, passing in command line arguments from the process.
        @param argc             The number of arguments.
        @param argv             The argument array.
    */
    int start(int argc = 0, char* argv[] = nullptr);

    /** Initiates the shutdown process of the host including shutting down JUCE and any custom host thread routine.*/
    void shutdown();

    /** @brief Loads a file for playback given its absolute file path.
        @param newFilePath      The absolute path to the file to load and playback.
        @return                 True if the file was opened, False if it could not be opened or was not a supported format.
    */
    bool loadFile(const char* newFilePath);

    /** @brief Gets whether a valid file is currently loaded in the file player.
        @return                 True if a valid file is loaded, False if not.
    */
    bool isFileLoaded() const;

    /** @brief Gets the name of the loaded file, if any, and copies it into the provided buffer.
        @param buffer           The character buffer to copy the name into.
        @param maxLength        The maximum character length of the provided buffer.
    */
    void getFileName(char* buffer, int maxLength);

    /** @brief Plays the currently loaded file.*/
    void play();

    /** @brief Pauses the current file when playing and resumes from the current time when @ref play() is called again.*/
    void pause();

    /** @brief Stops the current file from playing and returns to the beginning of the track.*/
    void stop();

    /** @brief Gets the current file player output gain value.*/
    float getGain() const noexcept;

    /** @brief Sets the current file player output gain.*/
    void setGain(float newGain) noexcept;

    /** @brief Gets the current Lumasonic output brightness value.*/
    float getBrightness() const noexcept;

    /** @brief Sets the current file player output gain.*/
    void setBrightness(float newBrightness) noexcept;

    /** @brief Gets whether there is a file currently playing.*/
    bool isPlaying() const noexcept;

    /** @brief Whether the current file is set to loop playback.*/
    bool isLooping() const;

    /** @brief Sets whether playback should loop when the file reaches the end of the stream.
        @param shouldLoop       Whether file playback should loop or finish after the file reaches the end of the stream.
    */
    void setLooping(bool shouldLoop);

    /** @brief Returns true if the player has stopped because its input stream ran out of data.*/
    bool hasStreamFinished() const noexcept;

    /** @brief Whether the player currently has playlist data loaded instead of just a single file.*/
    bool hasPlaylist() const;

    /** @brief Returns true if the player has stopped because it has finished the last stream in its current playlist.*/
    bool hasPlaylistFinished() const noexcept;

    /** @brief Whether the web server API is currently running.*/
    bool hasWebServer() const;

    /** @brief Gets the play time in seconds of the current file.
        @return                 The play time of the current file, in seconds.
    */
    double getPlayTimeSeconds() const;

    /** @brief Sets the play time of playback for the current file in seconds.
        @param timeInSeconds    The new time in seconds to set the play time to.
    */
    void setPlayTimeSeconds(double timeInSeconds);

    /** @brief Gets the play time of the current file in percent normalized from 0.0 to 1.0.
        @return                 The play time of the current file in percent normalized from 0.0 to 1.0.
    */
    double getPlayTimePercent() const;

    /** @brief Sets the play time of playback for the current file in normalized percent of audio length (0.0 - 1.0).
        @param timeInPercent    The new time to set as a normalized percentage of total audio file length (0.0 - 1.0).
    */
    void setPlayTimePercent(double timeInPercent);

    /** @brief Gets the total length, in seconds, of the current file.
        @return                 The total duration of the current file in seconds.
    */
    double getLengthInSeconds() const;

private:
    //==============================================================================
    class Pimpl;
    Pimpl* pimpl = nullptr;

#if LS_USE_FILE_PLAYER_WWW
    class WebServer;
#endif
};

#endif // LS_USE_FILE_PLAYER