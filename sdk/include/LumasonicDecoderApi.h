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

// Define export macro
#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#elif defined _WIN64
#define EXPORT_API __declspec(dllexport)
#else 
#define EXPORT_API 
#endif

// Define the buffer size for strings for host name/IP address
#ifndef LS_NET_HOST_NAME_SIZE
#define LS_NET_HOST_NAME_SIZE       256
#endif

// Define the buffer size for strings for the network interface
#ifndef LS_NET_INTERFACE_NAME_SIZE
#define LS_NET_INTERFACE_NAME_SIZE  64
#endif

// Define the number of bytes of a serialized stereo color sample
#define LS_STEREO_COLOR_SAMPLE_SIZE	32

/** @brief The local loopback address (127.0.0.1) as a 4 byte unsigned integer.*/
const unsigned int LS_LOOPBACK_IPV4 = 2130706433;

//==============================================================================
/** The @ref StereoColorSample struct exported for the C API.*/
typedef struct stereo_color_sample 
{
	unsigned long long ts;				///< timestamp
	float r0;					        ///< red channel 0
	float g0;					        ///< green channel 0
	float b0;					        ///< blue channel 0
	float r1;					        ///< red channel 1
	float g1;					        ///< green channel 1
	float b1;					        ///< blue channel 1
} stereo_color_sample;

//==============================================================================
/** The @ref LumasonicReadPerfInfo struct exported for the C API.*/
typedef struct ls_read_perf_info
{
	double thread_current_min_time_ms;	///< The current minimum delta time between calls to the decoder read thread (in milliseconds).
	double thread_current_max_time_ms;	///< The current maximum delta time between calls to the decoder read thread (in milliseconds).
	double thread_average_time_ms;		///< The current average delta time between calls to the decoder read thread (in milliseconds).
	double thread_total_min_time_ms;	///< The shortest delta time between calls to the decoder read thread since performance monitoring started (in milliseconds).
	double thread_total_max_time_ms;	///< The longest delta time between calls to the decoder read thread since performance monitoring started (in milliseconds).
	double data_current_min_time_ms;	///< The current minimum delta time between the current data output availablity and the previous data output availability (in milliseconds).
	double data_current_max_time_ms;	///< The current maximum delta time between the current data output availablity and the previous data output availability (in milliseconds).
	double data_average_time_ms;		///< The current average delta time between data output availablity cycles.
	double data_total_min_time_ms;		///< The shortest delta time between data output availability cycles since performance monitoring started (in milliseconds).
	double data_total_max_time_ms;		///< The shortest delta time between data output availability cycles since performance monitoring started (in milliseconds).
	bool has_data;                     	///< When False, this means that no data was available
} ls_read_perf_info;

/** The @ref UdpListenerConfig struct exported for the C API.*/
typedef struct ls_udp_listener_config
{
	unsigned int local_address;			///< The character buffer for the network interaface to use (usually an IPv4 string the local IP of the interface).
	unsigned int remote_address;		///< The character buffer for the remote IP address (usually IPv4) to send to.
	unsigned short remote_port;         ///< The remote port number to send to.
	unsigned short local_port;          ///< The local port number to bind to. Use 0 to automatically select an open local port.
	bool exclusive;						///< Whether the socket will bind in exclusive mode or share socket with other processes.
} ls_udp_listener_config;

//==============================================================================
// Decoder API
//==============================================================================

//==============================================================================
/** @brief Creates a new @ref LumasonicStereoDecoder instance and returns its unique ID.
	@return						The unique ID of the decoder created. If the ID value returns negative, an error occurred creating the decoder.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_decoder_create();
#else
int ls_decoder_create();
#endif

//==============================================================================
/** @brief Gets the current count of decoder instances managed by the codec.
	@return                     The  current count of decoder instances managed by the codec.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_decoder_get_count();
#else
int ls_decoder_get_count();
#endif

//==============================================================================
/** @brief Gets the ID of the main decoder (typically 0).
	@return						This method returns a positive ID number if a default decoder exists, otherwise -1 if one does not exist.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_decoder_get_main_id();
#else
int ls_decoder_get_main_id();
#endif

//==============================================================================
/** @brief Determines whether the product license was successfully authorized or not. This method is thread-safe/atomic.
	@param id                   The ID of the decoder to check.
	@return                     True if the decoder SDK license is authorized, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_decoder_is_authorized(int id);
#else
bool ls_decoder_is_authorized(int id);
#endif

//==============================================================================
/** @brief Whether the current stream is detected as AudioStrobe. This method is thread-safe/atomic.
	@param id                   The ID of the decoder instance.
	@return                     True if the current stream is detected as AudioStrobe, False if Lumasonic is detected.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_decoder_is_audiostrobe(int id);
#else
bool ls_decoder_is_audiostrobe(int id);
#endif

//==============================================================================
/** @brief Gets the current output brightness level (0.0 - 1.0). This method is thread-safe/atomic.
	@param id                   The ID of the decoder instance.
	@return                     The normalized global output brightness level in the range from 0.0 to 1.0.
*/
#ifdef __cplusplus
extern "C" EXPORT_API float ls_decoder_get_brightness(int id);
#else
float ls_decoder_get_brightness(int id);
#endif

//==============================================================================
/** @brief Sets the current output brightness level (0.0 - 1.0). This method is thread-safe/atomic.
	@param id                   The ID of the decoder instance.
	@param brightness			The new brightness value to set. This value should be normalized from 0.0 - 1.0.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_decoder_set_brightness(int id, float brightness);
#else
void ls_decoder_set_brightness(int id, float brightness);
#endif

//==============================================================================
/** @brief Initializes the decoder with the given sample rate and audio sample buffer size.
	@param id                   The ID of the decoder instance.
	@param sample_rate          The number of samples per second of the audio signal.
	@param buffer_size          The maximum number of audio samples per audio buffer processed.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_decoder_reset(int id, float sample_rate, int buffer_size);
#else
void ls_decoder_reset(int id, float sample_rate, int buffer_size);
#endif

//==============================================================================
/** @brief Processes a block of audio through the decoder.
	@param id                   The ID of the decoder instance.
	@param in0                  A pointer to the left/channel 0 input sample buffer to decode.
	@param in1                  A pointer to the right/channel 1 input sample buffer to decode.
	@param num_samples          The number of samples in each audio buffer to process.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_decoder_process_block(int id, const float* in0, const float* in1, int num_samples);
#else
void ls_decoder_process_block(int id, const float* in0, const float* in1, int num_samples);
#endif

//==============================================================================
/** @brief Gets the current red parameter normalized level for the left channel. This method is thread-safe/atomic.
	@param id                   The ID of the decoder instance.
	@return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
*/
#ifdef __cplusplus
extern "C" EXPORT_API float ls_decoder_get_level_r_0(int id);
#else
float ls_decoder_get_level_r_0(int id);
#endif

//==============================================================================
/** @brief Gets the current red parameter normalized level for the right channel. This method is thread-safe/atomic.
	@param id                   The ID of the decoder instance.
	@return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
*/
#ifdef __cplusplus
extern "C" EXPORT_API float ls_decoder_get_level_r_1(int id);
#else
float ls_decoder_get_level_r_1(int id);
#endif

//==============================================================================
/** @brief Gets the current green parameter normalized level for the left channel. This method is thread-safe/atomic.
	@param id                   The ID of the decoder instance.
	@return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
*/
#ifdef __cplusplus
extern "C" EXPORT_API float ls_decoder_get_level_g_0(int id);
#else
float ls_decoder_get_level_g_0(int id);
#endif

//==============================================================================
/** @brief Gets the current green parameter normalized level for the right channel. This method is thread-safe/atomic.
	@param id                   The ID of the decoder instance.
	@return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
*/
#ifdef __cplusplus
extern "C" EXPORT_API float ls_decoder_get_level_g_1(int id);
#else
float ls_decoder_get_level_g_1(int id);
#endif

//==============================================================================
/** @brief Gets the current blue parameter normalized level for the left channel. This method is thread-safe/atomic.
	@param id                   The ID of the decoder instance.
	@return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
*/
#ifdef __cplusplus
extern "C" EXPORT_API float ls_decoder_get_level_b_0(int id);
#else
float ls_decoder_get_level_b_0(int id);
#endif

//==============================================================================
/** @brief Gets the current blue parameter normalized level for the right channel. This method is thread-safe/atomic.
	@param id                   The ID of the decoder instance.
	@return                     The value if it is available (0.0 - 1.0), otherwise -1.f;
*/
#ifdef __cplusplus
extern "C" EXPORT_API float ls_decoder_get_level_b_1(int id);
#else
float ls_decoder_get_level_b_1(int id);
#endif

//==============================================================================
/** @brief Pops the next available stereo color sample from the decoded buffer. This method is thread-safe/atomic.
	@param id                   The ID of the decoder instance.
	@return                     The stereo color sample value. The timestamp (ts) property will be 0 if no data was available.
*/
#ifdef __cplusplus
extern "C" EXPORT_API stereo_color_sample ls_decoder_pop_color_sample(int id);
#else
stereo_color_sample ls_decoder_pop_color_sample(int id);
#endif

//==============================================================================
/** @brief Clears all decoded stereo color samples from the internal buffer. This method is thread-safe.
	@param id                   The ID of the decoder instance.
	@return                     The number of stereo color samples cleared.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_decoder_clear_color_samples(int id);
#else
bool ls_decoder_clear_color_samples(int id);
#endif

//==============================================================================
// Reader API
//==============================================================================

//==============================================================================
/** @brief Creates a new @ref LumasonicStereoReader instance and returns its unique ID.
	@return                     The unique ID of the reader created. If the ID value returns negative, an error occurred creating the reader.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_reader_create();
#else
int ls_reader_create();
#endif

//==============================================================================
/** @brief Gets the current count of reader instances managed by the codec.
	@return                     The  current count of reader instances managed by the codec.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_reader_get_count();
#else
int ls_reader_get_count();
#endif

//==============================================================================
/** @brief Gets the ID of the main reader (first index in the reader list).
	@return                     The index of the first reader in the list if one exists, otherwise -1.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_reader_get_main_id();
#else
int ls_reader_get_main_id();
#endif

//==============================================================================
/** @brief Gets the current reading thread mode. This method is thread-safe/atomic.
	@param id                   The ID of the reader instance.
	@return						The current thread mode enumeration value.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_reader_get_thread_mode(int id);
#else
int ls_reader_get_thread_mode(int id);
#endif

//==============================================================================
/** @brief Sets the current reading thread mode. This method is thread-safe/atomic.
	@param id					The ID of the reader instance.
	@param new_mode				The new thread mode to use for reading.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_reader_set_thread_mode(int id, int new_mode);
#else
void ls_reader_set_thread_mode(int id, int new_mode);
#endif

//==============================================================================
/** @brief Starts the Lumasonic reading thread.
	@param id					The ID of the reader instance.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_reader_start(int id);
#else
void ls_reader_start(int id);
#endif

//==============================================================================
/** @brief Stops the Lumasonic reading thread.
	@param id                   The ID of the reader instance.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_reader_stop(int id);
#else
void ls_reader_stop(int id);
#endif

//==============================================================================
/** @brief Whether the reader/dispatcher is currently running. This method is thread-safe.
	@param id                   The ID of the reader instance.
	@return						True if the reader is currently running/reading, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_reader_is_running(int id);
#else
bool ls_reader_is_running(int id);
#endif

//==============================================================================
/** @brief Adds a new listener to be notified when a new stereo color sample is ready. This method is thread-safe.
	@param reader_id            The ID of the reader instance.
	@param listener_id          The ID of the listener instance.
	@return						True if the listener was successfully added, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_reader_add_listener(int reader_id, int listener_id);
#else
bool ls_reader_add_listener(int reader_id, int listener_id);
#endif

//==============================================================================
/** @brief Removes an existing listener from notifications. This method is thread-safe.
	@param reader_id            The ID of the reader instance.
	@param listener_id          The ID of the listener instance.
	@return                     True if the listener was successfully removed, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_reader_remove_listener(int reader_id, int listener_id);
#else
bool ls_reader_remove_listener(int reader_id, int listener_id);
#endif

//==============================================================================
/** @brief Clears the current listener list. This method is thread-safe.
	@param id                   The ID of the reader instance.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_reader_clear_listeners(int id);
#else
void ls_reader_clear_listeners(int id);
#endif

//==============================================================================
/** @brief Clears the current listener list. This method is thread-safe.
	@param id                   The ID of the reader instance.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_reader_clear_listeners(int id);
#else
void ls_reader_clear_listeners(int id);
#endif

//==============================================================================
/** @brief Reads and pops the next performance information from the buffer. This method is thread-safe.
	@param id                   The ID of the reader instance.
	@return						The current performance information data. A false value for the has_data property indicates no data was available.
*/
#ifdef __cplusplus
extern "C" EXPORT_API ls_read_perf_info ls_reader_pop_perf_info(int id);
#else
ls_read_perf_info ls_reader_pop_perf_info(int id);
#endif

//==============================================================================
/** @brief Clears the current buffer of performance information.
	@param id                   The ID of the reader instance.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_reader_clear_perf_info(int id);
#else
void ls_reader_clear_perf_info(int id);
#endif

//==============================================================================
// Listener API
//==============================================================================

//==============================================================================
/** @brief Connects a decoder instance to a reader instance.
	@param type_id				The type ID of the listener to create.
	@return						The unique ID of the listener created. If the ID value returns negative, an error occurred creating the listener.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_listener_create(int type_id);
#else
int ls_listener_create(int type_id);
#endif

//==============================================================================
/** @brief Configures the network settings for an existing [UDP listener](@ref LumasonicStereoUdpListener) instance.
	@param id                   The ID of the listener instance.
	@param local_address        The local IPv4 address to use (as a 4 byte integer). This selects the local network interface. Use @ref ls_convert_str_to_ipv4_address() to convert from a string address.
	@param local_port			The local port number to use when binding to the socket for sending.
	@param remote_address		The remote IPv4 address to send to (as a 4 byte integer). Use @ref ls_convert_str_to_ipv4_address() to convert from a string address.
	@param remote_port			The remote port number to send packets to at the remote address.
	@param exclusive			Whether the socket should bind exclusively or let other processes share the socket.
	@return                     True if the operation was successful, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_listener_udp_config(int id, unsigned int local_address, unsigned short local_port,
													unsigned int remote_address, unsigned short remote_port, bool exclusive);
#else
bool ls_listener_udp_config(int id, unsigned int local_address, unsigned short local_port,
								unsigned int remote_address, unsigned short remote_port, bool exclusive);
#endif

//==============================================================================
/** @brief Configures an existing [UDP listener](@ref LumasonicStereoUdpListener) instance to send data to a given local loopback port.
	@param id                   The ID of the listener instance.
	@param port                 The local loopback port to send to.
	@return                     True if the operation was successful, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_listener_udp_loopback(int id, unsigned short port);
#else
bool ls_listener_udp_loopback(int id, unsigned short port);
#endif

//==============================================================================
/** @brief Gets the total number of packets sent. Use @ref ls_listener_udp_reset_num_packets_sent() to reset this counter.
	@param id                   The ID of the listener instance.
	@return                     The total number of packets sent over the lifetime of the listener.
*/
#ifdef __cplusplus
extern "C" EXPORT_API unsigned long long ls_listener_udp_get_num_packets_sent(int id);
#else
unsigned long long ls_listener_udp_get_num_packets_sent(int id);
#endif

//==============================================================================
/** @brief Resets the number of packets sent to 0.
	@param id                   The ID of the listener instance.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_listener_udp_reset_num_packets_sent(int id);
#else
void ls_listener_udp_reset_num_packets_sent(int id);
#endif

//==============================================================================
// Player API
//==============================================================================

#if LS_USE_FILE_PLAYER

//==============================================================================
/** @brief Creates the global instance of the @ref LumasonicFilePlayer.
	@param decoder_id           The ID of the decoder instance to use with the player.
	@param reader_id            The ID of the reader instance to use with the player.
	@param settings_dir_name	The custom settings directory name to use for storing and loading settings data.
	@param debug				When True the global file player will control the console and output live debug information to it while playing the file.
	@return                     True if the creation is successful, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_player_create(int decoder_id, int reader_id, const char* settings_dir_name, bool debug);
#else
int ls_player_create(int decoder_id, int reader_id, const char* settings_dir_name, bool debug);
#endif

//==============================================================================
/** @brief Creates a global instance of the @ref LumasonicFilePlayer and passes it command line arguments on start.
	@param decoder_id           The ID of the decoder instance to use with the player.
	@param reader_id            The ID of the reader instance to use with the player.
	@param argc                 The length of the argument value array.
	@param argv                 The argument value array from command line arguments to pass to the player.
	@return                     If the player was successfully created 0. Any non-zero value indicates an error code.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_player_create_with_args(int decoder_id, int reader_id, int argc, char* argv[]);
#else
int ls_player_create_with_args(int decoder_id, int reader_id, int argc, char* argv[]);
#endif

//==============================================================================
/** @brief Gets whether the global instance of the @ref LumasonicFilePlayer has been created using @ref ls_player_create().
	@return                     True if the player has been created, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_player_is_created();
#else
bool ls_player_is_created();
#endif

//==============================================================================
/** @brief Loads a file to play in the global file player, if created.
	@param file_path            The absolute path to the file to loadFromDirectory.
	@return                     0 if the file loaded successfully for playback, or a non-zero error code if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API int ls_player_load_file(const char* file_path);
#else
int ls_player_load_file(const char* file_path);
#endif

//==============================================================================
/** @brief Gets whether a valid file was successfully loaded and is ready for playback in the global file player.*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_player_is_file_loaded();
#else
bool ls_player_is_file_loaded();
#endif

//==============================================================================
/** @brief If a file is loaded in the global file player, plays the file stream.*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_player_play();
#else
void ls_player_play();
#endif

//==============================================================================
/** @brief If a file is loaded in the global file player, pauses the file stream at the current time.*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_player_pause();
#else
void ls_player_pause();
#endif

//==============================================================================
/** @brief If a file is loaded in the global file player and is playing, stops playing the file stream.*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_player_stop();
#else
void ls_player_stop();
#endif

//==============================================================================
/** @brief Gets the output gain of the default global file player instance if it exists.*/
#ifdef __cplusplus
extern "C" EXPORT_API float ls_player_get_gain();
#else
float ls_player_get_gain();
#endif

//==============================================================================
/** @brief Sets the output gain of the default global file player instance if it exists (from 0.0, to 1.0).*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_player_set_gain(float new_gain);
#else
void ls_player_set_gain(float new_gain);
#endif

//==============================================================================
/** @brief Gets the Lumasonic output brightness of the default global file player instance if it exists.*/
#ifdef __cplusplus
extern "C" EXPORT_API float ls_player_get_brightness();
#else
float ls_player_get_brightness();
#endif

//==============================================================================
/** @brief Sets the Lumasonic output brightness of the default global file player instance if it exists (from 0.0, to 1.0).*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_player_set_brightness(float new_brightness);
#else
void ls_player_set_brightness(float new_brightness);
#endif

//==============================================================================
/** @brief Checks to see if the global instance of the @ref LumasonicFilePlayer is currently playing a file.
	@return                     True if the player if currently playing, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_player_is_playing();
#else
bool ls_player_is_playing();
#endif

//==============================================================================
/** @brief Checks to see if the global instance of the @ref LumasonicFilePlayer is currently looping the file's playback.
	@return                     True if the player if currently looping, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_player_is_looping();
#else
bool ls_player_is_looping();
#endif

//==============================================================================
/** @brief Sets whether global file player playback should loop when the file reaches the end of the stream.
	@param should_loop			Whether file playback should loop or finish after the file reaches the end of the stream.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_player_set_looping(bool should_loop);
#else
void ls_player_set_looping(bool should_loop);
#endif

//==============================================================================
/** @brief Checks to see if the global instance of the @ref LumasonicFilePlayer has finished the playback of a loaded file.
	@return                     True if the player if finished with the loaded stream, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_player_has_stream_finished();
#else
bool ls_player_has_stream_finished();
#endif

//==============================================================================
/** @brief Checks to see if the global instance of the @ref LumasonicFilePlayer has a web server API running.
	@return                     True if the player has a web server API running, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_player_has_web_server();
#else
bool ls_player_has_web_server();
#endif

//==============================================================================
/** @brief Gets whether the global instance of the @ref LumasonicFilePlayer currently has a playlist of content loaded.
	@return                     True if a playlist of files is loaded, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_player_has_playlist();
#else
bool ls_player_has_playlist();
#endif

//==============================================================================
/** @brief Gets whether the global instance of the @ref LumasonicFilePlayer has a playlist that has currently finished playing.
	@return                     True if a playlist has finished playing, False if not or there is no playlist.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_player_has_playlist_finished();
#else
bool ls_player_has_playlist_finished();
#endif

//==============================================================================
/** @brief Gets the global file player's current play time in total seconds.*/
#ifdef __cplusplus
extern "C" EXPORT_API double ls_player_get_play_time_seconds();
#else
double ls_player_get_play_time_seconds();
#endif

//==============================================================================
/** @brief Sets the global file player's current play time in seconds.*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_player_set_play_time_seconds(double time_in_seconds);
#else
void ls_player_set_play_time_seconds(double time_in_seconds);
#endif

//==============================================================================
/** @brief Gets the global file player's current play time as a normalized percent 0.0 to 1.0.*/
#ifdef __cplusplus
extern "C" EXPORT_API double ls_player_get_play_time_percent();
#else
double ls_player_get_play_time_percent();
#endif

//==============================================================================
/** @brief Sets the global file player's current play time as a normalized percent 0.0 to 1.0.*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_player_set_play_time_percent(double time_in_percent);
#else
void ls_player_set_play_time_percent(double time_in_percent);
#endif

//==============================================================================
/** @brief Gets the global file player's current file duration in total seconds.*/
#ifdef __cplusplus
extern "C" EXPORT_API double ls_player_get_length_in_seconds();
#else
double ls_player_get_length_in_seconds();
#endif

#endif // LS_USE_FILE_PLAYER

//==============================================================================
// Codec API
//==============================================================================

//==============================================================================
/** @brief Connects a decoder instance to a reader instance.
	@param decoder_id           The ID of the decoder instance to connect.
	@param reader_id            The ID of the reader instance to connect.
	@return                     True if the connection is successfully, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_codec_connect(int decoder_id, int reader_id);
#else
bool ls_codec_connect(int decoder_id, int reader_id);
#endif

/** @brief Disconnects a decoder instance and a reader instance.
	@param decoder_id           The ID of the decoder instance to disconnect.
	@param reader_id            The ID of the reader instance to disconnect.
	@return                     True if unconnection successfully, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_codec_disconnect(int decoder_id, int reader_id);
#else
bool ls_codec_disconnect(int decoder_id, int reader_id);
#endif

//==============================================================================
/**
 * @brief Shuts down the global Lumasonic codec manager.
 *
 * @details
 *
 * This method should only be called once at the end of your application when
 * you are done with the codec.
 *
 * @return						This method returns 0 if shutdown was successful, otherwise an error code.
 */
#ifdef __cplusplus
extern "C" EXPORT_API int ls_codec_shutdown();
#else
int ls_codec_shutdown();
#endif

//==============================================================================
/**
 * @brief Initializes the global Lumasonic codec manager with a log file.
 *
 * @details
 *
 * This method should only be called once at the beginning of your application.
 *
 * @param log_path				The absolute path to a log file to write to.
 * @param max_file_size			The maximum size in bytes the log file will grow before being truncated.
 * @return						This method returns 0 if initialization was successful, otherwise the result will be negative.
 */
#ifdef __cplusplus
extern "C" EXPORT_API int ls_codec_init(const char* log_path, int max_file_size);
#else
int ls_codec_init(const char* log_path, int max_file_size);
#endif

//==============================================================================
/**
 * @brief Utility function that completely clears the console.
 *
 * @details
 *
 * This can be called for console applications and is useful for clearing the entire screen in console mode.
 */
#ifdef __cplusplus
extern "C" EXPORT_API void ls_codec_clear_console();
#else
void ls_codec_clear_console();
#endif

//==============================================================================
/**
 * @brief De-interleaves 32 bit PCM float audio, from a single interleaved stereo buffer, into two separate left and right channel buffers.
 * 
 * @details
 * For stereo, if the total interleaved input buffer length is 512 samples, then **samples_per_channel** will be 256 => 512 (input buffer length) / 2 (channels) = 256 samples.
 * 
 * @param input                The left/right interleaved stereo audio samples.
 * @param out0                 The pointer to the buffer that will contained the de-interleaved left samples.
 * @param out1                 The pointer to the buffer that will contained the de-interleaved right samples.
 * @param samples_per_channel  The total number of de-interleaved audio samples per channel (for stereo this is half of the total interleaved samples in the input).
 * @return                     True if the process was successful, False it not.
 */
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_codec_deinterleave_audio(const float* input, float* out0, float* out1, int samples_per_channel);
#else
bool ls_codec_deinterleave_audio(const float* input, float* out0, float* out1, int samples_per_channel);
#endif

//==============================================================================
/** @brief Resets the static Lumasonic encoder to the given sample rate.
	@param sample_rate           The sample rate to reset the encoder to.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_static_encoder_reset(float sample_rate);
#else
void ls_static_encoder_reset(float sample_rate);
#endif

//==============================================================================
/** @brief Sets the stereo color values of the static encoder.
	@param r0                   The red value for the left channel to set.
	@param g0                   The green value for the left channel to set.
	@param b0                   The blue value for the left channel to set.
	@param r1                   The red value for the right channel to set.
	@param g1                   The green value for the right channel to set.
	@param b1                   The blue value for the right channel to set.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_static_encoder_set_stereo_color(float r0, float g0, float b0, float r1, float g1, float b1);
#else
void ls_static_encoder_set_stereo_color(float r0, float g0, float b0, float r1, float g1, float b1);
#endif

//==============================================================================
/** @brief Encodes the current static stereo color values into two audio floating point buffers for a given number of samples.
	@param in0              The left audio buffer to encode into.
	@param in1              The right audio buffer to encode into.
	@param num_samples      The number of samples to encode into the buffers.
	@param replace          When True, samples in the buffer will be replaced, when False encoded samples will be mixed with the existing buffer data.
*/
#ifdef __cplusplus
extern "C" EXPORT_API void ls_static_encoder_process_block(float* in0, float* in1, int num_samples, bool replace);
#else
void ls_static_encoder_process_block(float* in0, float* in1, int num_samples, bool replace);
#endif

//==============================================================================
/** @brief Gets the total number of network interfaces detected on the system.
	@return                     The total number of available network interface.
*/
#ifdef __cplusplus
extern "C" EXPORT_API unsigned int ls_get_num_network_interfaces();
#else
unsigned int ls_get_num_network_interfaces();
#endif

//==============================================================================
/** @brief Gets the name of a network interface at the given index of the list of total interfaces.

	> [!NOTE]
	> If This method returns False because no network interface was found at the given index,
	> the contents of the name buffer will be unaltered.

	@param index                The index in the list of interfaces to get the name for.
	@param name                 The character buffer that will have the interface name copied to it. The size of the buffer should be at least @ref LS_NET_INTERFACE_NAME_SIZE characters in size.
	@return                     True if an interface name was found for the given index, False if not.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_get_network_interface_name(int index, char* name);
#else
bool ls_get_network_interface_name(int index, char* name);
#endif

//==============================================================================
/** @brief Gets the 4 byte integer address of a network interface at the given index of the list of total interfaces.
	@param index                The index in the list of interfaces to get the address for.
	@return                     The address value if a valid interface was found at the supplied index, otherwise 0.
*/
#ifdef __cplusplus
extern "C" EXPORT_API unsigned int ls_get_network_interface_address(int index);
#else
unsigned int ls_get_network_interface_address(int index);
#endif

//==============================================================================
/** @brief Deserializes @ref stereo_color_sample data from a UDP packet.
	@param data                 A pointer to the data buffer to read from.
	@return                     The deserialized stereo color data. If there was no data, the timestamp will be 0.
*/
#ifdef __cplusplus
extern "C" EXPORT_API stereo_color_sample ls_get_udp_stereo_color(void* data);
#else
stereo_color_sample ls_get_udp_stereo_color(void* data);
#endif

//==============================================================================
/** @brief Converts an IPv4 address represented as a string into a 4 byte integer address.
	@param str_address          The IPv4 address represented as a string "XXX.XXX.XXX.XXX".
	@return                     The address converted into 4 byte int form.
*/
#ifdef __cplusplus
extern "C" EXPORT_API unsigned int ls_convert_str_to_ipv4_address(const char* str_address);
#else
unsigned int ls_convert_str_to_ipv4_address(const char* str_address);
#endif

//==============================================================================
/** @brief Converts an IPv4 address represented as a 4 byte integer into a C string.
	@param int_address          The IPv4 address represented as an unsigned int.
	@param str_address          The char buffer to write the string address into (should be at least 16 bytes long).
	@return                     True if a conversion was performed, False if not. Returning True does not guarantee a valid address, just a parsing took place.
*/
#ifdef __cplusplus
extern "C" EXPORT_API bool ls_convert_ipv4_address_to_str(unsigned int int_address, char* str_address);
#else
bool ls_convert_ipv4_address_to_str(unsigned int int_address, char* str_address);
#endif
