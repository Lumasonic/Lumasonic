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
 * @brief Manages a list of decoder and reader instances using an ID system.
 * 
 * @details
 * This class implements the C API version of the decoder library. It allows
 * for the simultaneous operation of one or more decoder and reader instances.
 * 
 * For more information, see the [section for Using the C API](README.md).
 */
class LumasonicCodec
{
public:
    //==============================================================================
    /** @brief Constructor*/
    LumasonicCodec();

    /** @brief Destructor*/
    ~LumasonicCodec();

    //==============================================================================
    /** @brief Connects a decoder instance to a reader instance.
        @param decoderId            The ID of the decoder instance to connect.
        @param readerId             The ID of the reader instance to connect.
        @return                     True if the connection is successfully, False if not.
    */
    bool connect(int decoderId, int readerId);

    /** @brief Disconnects a decoder instance and a reader instance.
        @param decoderId            The ID of the decoder instance to disconnect.
        @param readerId             The ID of the reader instance to disconnect.
        @return                     True if unconnection successfully, False if not.
    */
    bool disconnect(int decoderId, int readerId);

    /** @brief Shuts down and destroys all created instances. Call this when you are finished using the manager.*/
    void shutdown();

    /** @brief Utility function that completely clears the console.*/
    static void clear_console();

    /** @brief Takes Left/Right interleaved audio and de-interleaves it into separate left and right sample buffers.
        
        @details
        For stereo, if the total interleaved input buffer length is 512 samples, then **samplesPerChannel** will be 256 => 512 (input buffer length) / 2 (channels) = 256 samples.
    
        @param input                The left/right interleaved stereo audio samples.
        @param out0                 The pointer to the buffer that will contained the de-interleaved left samples.
        @param out1                 The pointer to the buffer that will contained the de-interleaved right samples.
        @param samplesPerChannel    The total number of de-interleaved audio samples per channel (for stereo this is half of the total interleaved samples in the input).
        @return                     True if the process was successful, False it not.
    */
    static bool deinterleave_audio(const float* input, float* out0, float* out1, int samplesPerChannel);

    /** @brief Resets the static Lumasonic encoder to the given sample rate.
        @param sampleRate           The sample rate to reset the encoder to.
    */
    void static_encoder_reset(float sampleRate);

    /** @brief Sets the stereo color values of the static encoder.
        @param r0                   The red value for the left channel to set.
        @param g0                   The green value for the left channel to set.
        @param b0                   The blue value for the left channel to set.
        @param r1                   The red value for the right channel to set.
        @param g1                   The green value for the right channel to set.
        @param b1                   The blue value for the right channel to set.
    */
    void static_encoder_set_stereo_color(float r0, float g0, float b0, float r1, float g1, float b1);

    /** @brief Encodes the current static stereo color values into two audio floating point buffers for a given number of samples.
        @param in0              The left audio buffer to encode into.
        @param in1              The right audio buffer to encode into.
        @param numSamples       The number of samples to encode into the buffers.
        @param replace          When True, samples in the buffer will be replaced, when False encoded samples will be mixed with the existing buffer data.
    */
    void static_encoder_process_block(float* in0, float* in1, int numSamples, bool replace);

    /** @brief Gets the total number of network interfaces detected on the system.
        @return                     The total number of available network interface.
    */
    static unsigned int get_num_network_interfaces();

    /** @brief Gets the name of a network interface at the given index of the list of total interfaces.

        > [!NOTE]
        > If This method returns False because no network interface was found at the given index,
        > the contents of the name buffer will be unaltered.

        @param index                The index in the list of interfaces to get the name for.
        @param name                 The character buffer that will have the interface name copied to it. The size of the buffer should be at least @ref LS_NET_INTERFACE_NAME_SIZE characters in size.
        @return                     True if an interface name was found for the given index, False if not.
    */
    static bool get_network_interface_name(int index, char* name);

    /** @brief Deserializes @ref StereoColorSample data from a UDP packet.
        @param data                 A pointer to the data buffer to read from.
        @return                     The deserialized stereo color data. If there was no data, the timestamp will be 0.
    */
    static StereoColorSample get_udp_stereo_color(void* data);

    /** @brief Converts an IPv4 address represented as a string into a 4 byte integer address.
        @param strAddress           The IPv4 address represented as a string "XXX.XXX.XXX.XXX".
        @return                     The address converted into 4 byte int form.
    */
    static unsigned int convert_str_to_ipv4_address(const char* strAddress);

    /** @brief Converts an IPv4 address represented as a 4 byte integer into a C string.
        @param intAddress           The IPv4 address represented as an unsigned int.
        @param strAddress           The char buffer to write the string address into (should be at least 16 bytes long).
        @return                     True if a conversion was performed, False if not. Returning True does not guarantee a valid address, just a parsing took place.
    */
    static bool convert_ipv4_address_to_str(unsigned int intAddress, char* strAddress);

    //==============================================================================
    // Decoder API
    //==============================================================================
    /** @brief Creates a new @ref LumasonicStereoDecoder instance and returns its unique ID.
        @param logPath              The absolute path to a log file to write to.
        @param maxFileSize          The maximum size in bytes the log file can grow before being truncated.
        @return                     The unique ID of the decoder created. If the ID value returns negative, an error occurred creating the decoder.
    */
    int decoder_create(const char* logPath = nullptr, int maxFileSize = 1024 * 128);

    /** @brief Gets the current count of decoder instances managed by the codec.
        @return                     The  current count of decoder instances managed by the codec.
    */
    int decoder_get_count();

    /** @brief Gets the ID of the main decoder (first index in the decoder list).
        @return                     The index of the first decoder in the list if one exists, otherwise -1.
    */
    int decoder_get_main_id();

    /** @brief Whether the current stream is detected as AudioStrobe. This method is thread-safe/atomic.
        @param id                   The ID of the decoder instance.
        @return                     True if the current stream is detected as AudioStrobe, False if Lumasonic is detected.
    */
    bool decoder_is_audiostrobe(int id);

    /** @brief Gets the current output brightness level (0.0 - 1.0). This method is thread-safe/atomic.
        @param id                   The ID of the decoder instance.
        @return                     The normalized global output brightness level in the range from 0.0 to 1.0.
    */
    float decoder_get_brightness(int id);

    /** @brief Sets the current output brightness level (0.0 - 1.0). This method is thread-safe/atomic.
        @param id                   The ID of the decoder instance.
        @param newBrightness        The new brightness value to set. This value should be normalized from 0.0 - 1.0.
    */
    void decoder_set_brightness(int id, float newBrightness);

    /** @brief Initializes the decoder with the given sample rate and audio sample buffer size.
        @param id                   The ID of the decoder instance.
        @param sampleRate           The number of samples per second of the audio signal.
        @param bufferSize           The maximum number of audio samples per audio buffer processed.
    */
    void decoder_reset(int id, float sampleRate, int bufferSize);

    /** @brief Processes a block of audio through the decoder.
        @param id                   The ID of the decoder instance.
        @param in0                  A pointer to the left/channel 0 input sample buffer to decode.
        @param in1                  A pointer to the right/channel 1 input sample buffer to decode.
        @param numSamples           The number of samples in each audio buffer to process.
    */
    void decoder_process_block(int id, const float* in0, const float* in1, int numSamples);

    /** @brief Gets the current red parameter normalized level for the left channel. This method is thread-safe/atomic.
        @param id                   The ID of the decoder instance.
        @return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
    */
    float decoder_get_level_r_0(int id);

    /** @brief Gets the current red parameter normalized level for the right channel. This method is thread-safe/atomic.
        @param id                   The ID of the decoder instance.
        @return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
    */
    float decoder_get_level_r_1(int id);

    /** @brief Gets the current green parameter normalized level for the left channel. This method is thread-safe/atomic.
        @param id                   The ID of the decoder instance.
        @return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
    */
    float decoder_get_level_g_0(int id);

    /** @brief Gets the current green parameter normalized level for the right channel. This method is thread-safe/atomic.
        @param id                   The ID of the decoder instance.
        @return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
    */
    float decoder_get_level_g_1(int id);

    /** @brief Gets the current blue parameter normalized level for the left channel. This method is thread-safe/atomic.
        @param id                   The ID of the decoder instance.
        @return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
    */
    float decoder_get_level_b_0(int id);

    /** @brief Gets the current blue parameter normalized level for the right channel. This method is thread-safe/atomic.
        @param id                   The ID of the decoder instance.
        @return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
    */
    float decoder_get_level_b_1(int id);

    /** @brief Pops the next available stereo color sample from the decoded buffer. This method is thread-safe/atomic.
        @param id                   The ID of the decoder instance.
        @param sample               The sample reference that will be assigned the loaded values if any are available.
        @return                     True if a new sample was available, False if not.
    */
    bool decoder_pop_color_sample(int id, StereoColorSample& sample);

    /** @brief Clears all decoded stereo color samples from the internal buffer. This method is thread-safe.
        @param id                   The ID of the decoder instance.
        @return                     The number of stereo color samples cleared.
    */
    int decoder_clear_color_samples(int id);

    //==============================================================================
    // Reader API
    //==============================================================================

    /** @brief Creates a new @ref LumasonicStereoReader instance and returns its unique ID.
        @return                     The unique ID of the reader created. If the ID value returns negative, an error occurred creating the reader.
    */
    int reader_create();

    /** @brief Gets the current count of reader instances managed by the codec.
        @return                     The  current count of reader instances managed by the codec.
    */
    int reader_get_count();

    /** @brief Gets the ID of the main reader (first index in the reader list).
        @return                     The index of the first reader in the list if one exists, otherwise -1.
    */
    int reader_get_main_id();

    /** @brief Gets the current reading thread mode. This method is thread-safe/atomic.
        @param id                   The ID of the reader instance.
        @return					    The current thread mode enumeration value.
    */
    LumasonicThreadModes reader_get_thread_mode(int id);

    /** @brief Sets the current reading thread mode. This method is thread-safe/atomic.
        @param id                   The ID of the reader instance.
        @param newMode			    The new thread mode to use for reading.
     */
    void reader_set_thread_mode(int id, LumasonicThreadModes newMode);

    /** @brief Starts the Lumasonic reading thread.
        @param id                   The ID of the reader instance.
    */
    void reader_start(int id);

    /** @brief Stops the Lumasonic reading thread.
        @param id                   The ID of the reader instance.
    */
    void reader_stop(int id);

    /** @brief Whether the reader/dispatcher is currently running. This method is thread-safe.
        @param id                   The ID of the reader instance.
        @return						True if the reader is currently running/reading, False if not.
    */
    bool reader_is_running(int id);

    /** @brief Adds a new listener to be notified when a new stereo color sample is ready. This method is thread-safe.
        @param readerId             The ID of the reader instance.
        @param listenerId           The ID of the listener instance.
        @return                     True if the listener was successfully added, False if not.
    */
    bool reader_add_listener(int readerId, int listenerId);

    /** @brief Removes an existing listener from notifications. This method is thread-safe.
        @param readerId             The ID of the reader instance.
        @param listenerId           The ID of the listener instance.
        @return                     True if the listener was successfully removed, False if not.
    */
    bool reader_remove_listener(int readerId, int listenerId);

    /** @brief Clears the current listener list. This method is thread-safe.
        @param id                   The ID of the reader instance.
    */
    void reader_clear_listeners(int id);

    /** @brief Reads and pops the next performance information from the buffer. This method is thread-safe.
        @param id                   The ID of the reader instance.
        @param info                 The performance information reference that will have its values assigned.
        @return						True if data was available and assigned, False if not.
    */
    bool reader_pop_perf_info(int id, LumasonicReadPerfInfo& info);

    /** @brief Clears the current buffer of performance information.
        @param id                   The ID of the reader instance.
    */
    void reader_clear_perf_info(int id);

    //==============================================================================
    // Listener API
    //==============================================================================

    /** @brief Creates a new [listener](@ref LumasonicStereoColorListener) instance of a given type.
        @param type                 The type of listener to create.
        @return                     The ID of the created listener, or a negative value if an error occurred.
    */
    int listener_create(LumasonicListenerTypes type);

    /** @brief Configures the network settings for an existing [UDP listener](@ref LumasonicStereoUdpListener) instance.
        @param id                   The ID of the listener instance.
        @param config               The network configuration to use with the listener.
        @return                     True if the operation was successful, False if not.
    */
    bool listener_udp_config(int id, UdpListenerConfig config);

    /** @brief Configures an existing [UDP listener](@ref LumasonicStereoUdpListener) instance to send data to a given local loopback port.
        @param id                   The ID of the listener instance.
        @param port                 The local loopback port to send to.
        @return                     True if the operation was successful, False if not.
    */
    bool listener_udp_loopback(int id, unsigned short port);

    /** @brief Gets the total number of packets sent. Use @ref listener_udp_reset_num_packets_sent() to reset this counter.
        @param id                   The ID of the listener instance.
        @return                     The total number of packets sent over the lifetime of the listener.
    */
    unsigned long long listener_udp_get_num_packets_sent(int id);

    /** @brief Resets the number of packets sent to 0.
        @param id                   The ID of the listener instance.
    */
    void listener_udp_reset_num_packets_sent(int id);

    //==============================================================================
    // Player API
    //==============================================================================

    /** @brief Creates a global instance of the @ref LumasonicFilePlayer.
        @param decoderId            The ID of the decoder instance to use with the player.
        @param readerId             The ID of the reader instance to use with the player.
        @param settingsDirName      A custom settings directory name to use for storing settings data.
        @param debug                When True the player will control the console and output live debug information to it while playing the file.
        @return                     If the player was successfully created 0. Any non-zero value indicates an error code.
    */
    int player_create(int decoderId, int readerId, const char* settingsDirName, bool debug);

    /** @brief Creates a global instance of the @ref LumasonicFilePlayer and passes it command line arguments on start.
        @param decoderId            The ID of the decoder instance to use with the player.
        @param readerId             The ID of the reader instance to use with the player.
        @param argc                 The length of the argument value array.
        @param argv                 The argument value array from command line arguments to pass to the player.
        @return                     If the player was successfully created 0. Any non-zero value indicates an error code.
    */
    int player_create_with_args(int decoderId, int readerId, int argc, char* argv[]);

    /** @brief Checks to see if the global instance of the @ref LumasonicFilePlayer has been created.
        @return                     True if the player has been created, False if not.
    */
    bool player_is_created();

    /** @brief Loads a file to play in the global file player, if created.
        @param filePath             The absolute path to the file to loadFromDirectory.
        @return                     0 if the file loaded successfully for playback, or a non-zero error code if not.
    */
    int player_load_file(const char* filePath);

    /** @brief Gets whether a valid file was successfully loaded and is ready for playback in the global file player.*/
    bool player_is_file_loaded();

    /** @brief If a file is loaded in the global file player, plays the file stream.*/
    void player_play();

    /** @brief If a file is loaded in the global file player, pauses the file stream at the current time point.*/
    void player_pause();

    /** @brief If a file is loaded in the global file player and is playing, stops playing the file stream.*/
    void player_stop();

    /** @brief Gets the output gain of the default global file player instance if it exists.*/
    float player_get_gain();

    /** @brief Sets the output gain of the default global file player instance if it exists (from 0.0, to 1.0).*/
    void player_set_gain(float newGain);

    /** @brief Gets the Lumasonic output brightness of the default global file player instance if it exists.*/
    float player_get_brightness();

    /** @brief Sets the Lumasonic output brightness of the default global file player instance if it exists (0.0 - 1.0).*/
    void player_set_brightness(float newBrightness);

    /** @brief Checks to see if the global instance of the @ref LumasonicFilePlayer is currently playing a file.
        @return                     True if the player is currently playing, False if not.
    */
    bool player_is_playing();

    /** @brief Checks to see if the global instance of the @ref LumasonicFilePlayer is currently looping the file's playback.
        @return                     True if the player is currently looping, False if not.
    */
    bool player_is_looping();

    /** @brief Sets whether global file player playback should loop when the file reaches the end of the stream.
        @param shouldLoop       Whether file playback should loop or finish after the file reaches the end of the stream.
    */
    void player_set_looping(bool shouldLoop);

    /** @brief Checks to see if the global instance of the @ref LumasonicFilePlayer has finished the playback of a loaded file.
        @return                     True if the player is finished with the loaded stream, False if not.
    */
    bool player_has_stream_finished();

    /** @brief Checks to see if the global instance of the @ref LumasonicFilePlayer has a web server API running.
        @return                     True if the player has a web server API running, False if not.
    */
    bool player_has_web_server();

    /** @brief Gets whether the player currently has a playlist of content loaded.
        @return                     True if a playlist of files is loaded, False if not.
    */
    bool player_has_playlist();

    /** @brief Gets whether the player's playlist has currently finished playing.
        @return                     True if a playlist has finished playing, False if not or there is no playlist.
    */
    bool player_has_playlist_finished();

    /** @brief Gets the global file player's current play time in total seconds.*/
    double player_get_play_time_seconds();

    /** @brief Sets the global file player's current play time in seconds.*/
    void player_set_play_time_seconds(double timeInSeconds);

    /** @brief Gets the global file player's current play time as a normalized percent 0.0 to 1.0.*/
    double player_get_play_time_percent();

    /** @brief Sets the global file player's current play time as a normalized percent 0.0 to 1.0.*/
    void player_set_play_time_percent(double timeInPercent);

    /** @brief Gets the global file player's current file duration in total seconds.*/
    double player_get_length_in_seconds();

private:
    //==============================================================================
    class Pimpl;
    Pimpl* pimpl = nullptr;
};
