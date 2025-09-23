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
#include <cstddef>

//==============================================================================
/**
 * @brief Listener class used to relay decoded @ref StereoColorSample values over
 * a UDP socket (once received from a @ref LumasonicStereoReader).
 *
 * @details
 * > [!NOTE]
 * > Values are sent on the reader's thread. The listener will not send values
 * > over UDP until you start the connection using @ref startUdp(UdpListenerConfig).
 * 
 * ### Configuring the Socket Connection
 * 
 * ```c++
 * 
 * auto* udpListener = new LumasonicStereoUdpListener();
 * 
 * // Create the UDP connection
 * UdpListenerConfig cfg;
 * const char* loopbackStr = "127.0.0.1";   // local loopback address as a IPv4 string
 * unsigned int loopbackAddr = LumasonicStereoUdpListener::ipv4AddressToStr(loopbackStr);
 * 
 * cfg.localAddress = loopbackAddr;         // local interface = loopback
 * cfg.localPort = 0;                       // let the system pick an unused port number
 * cfg.remoteAddress = loopbackAddr;        // remote address = loopback
 * cfg.remotePort = 8000;                   // loopback receive port = 8000
 * cfg.exclusive = false;                   // let the socket/adapter be shared with other processes
 * 
 * udpListener->startUdp(cfg);              // start and bind the UDP socket
 * ```
 * 
 * ### Checking the Connection Status
 * 
 * To see if the socket is currently open/connected call @ref isSocketOpen().
 * 
 * ```c++
 * 
 * bool socketOpen = udpListener->isSocketOpen();
 * 
 * ```
 * 
 * ### Stopping the Connection

 * To stop the socket, call @ref stopUdp().
 * 
 * ```c++
 * 
 * udpListener->stopUdp();                  // this will stop data relay; use startUdp() to reconfigure the socket for a new connection
 * 
 * ```
 * ### Checking Packets Sent
 * 
 * To see how many stereo color sample packets have been sent, call @ref getNumPacketsSent().
 * 
 * ```c++
 * 
 * size_t numPackets = updListener->getNumPacketsSent();
 * 
 * ```
 * 
 * To reset the total number of packets sent to 0, call @ref resetNumPacketsSent().
 * 
 * ```c++
 * 
 * updListener->resetNumPacketsSent();      // this resets the packet counter to 0
 * 
 * ```
 * 
 * ### Listing Network Interface Names
 * 
 * Network names are currently listed as the IPv4 address associated with the adapter.
 * 
 * To see how many network interfaces currently exist, use the @ref LumasonicCodec::get_num_network_interfaces() method.
 * 
 * ```c++
 * 
 * auto numInterfaces = LumasonicCodec::get_num_network_interfaces();
 * 
 * ```
 * 
 * To get the interface name of an interface at a specific index in the list, use the @ref LumasonicCodec::get_network_interface_name() method.
 * 
 * ```c++
 * 
 * auto numInterfaces = LumasonicCodec::get_num_network_interfaces();
 * std::cout << "Number of network interfaces found: " << numInterfaces << std::endl;
 * char name[LS_NET_INTERFACE_NAME_SIZE];
 * 
 * for (int i = 0; i < numInterfaces; ++i)
 * {
 *      if (LumasonicCodec::get_network_interface_name(i, name))
 *          std::cout << "  => " << name << std::endl;
 * }
 * 
 * ```
 * 
 * ### UDP Data Payload
 * 
 * This listener serializes the bytes of a @ref StereoColorSample and sends
 * the binary payload over the UDP socket.
 * 
 * Here is the structure of the **32 byte** packet:
 * 
 * Bytes| Type                    | Value
 * -----|-------------------------|-----------
 * 8    | 64-bit unsigned int     | timestamp
 * 4    | 32-bit float            | red level left
 * 4    | 32-bit float            | green level left
 * 4    | 32-bit float            | blue level left
 * 4    | 32-bit float            | red level right
 * 4    | 32-bit float            | green level right
 * 4    | 32-bit float            | blue level right
 * 
 * Values with a **timestamp** of **0** indicate **no data** was available.
 */
class LumasonicStereoUdpListener : public LumasonicStereoColorListener
{
public:
    //==============================================================================
    /** @brief Constructor*/
    LumasonicStereoUdpListener();

    /** @brief Destructor.*/
    ~LumasonicStereoUdpListener() override;

    //==============================================================================
    /** @brief The unique ID of the instance.*/
    int id = -1;

    /** @brief Configures the UDP settings for the listener.
        @param config               The UDP socket configuration to use for sending.
        @return                     True if starting UDP succeeded, False if it failed.
    */
    bool startUdp(UdpListenerConfig config);

    /** @brief Stops UDP networking and releases the current bound UDP socket if one exists.*/
    void stopUdp();

    /** @brief Whether the socket is currently bound and open.*/
    bool isSocketOpen();

    /** @brief Gets the total number of packets sent. Use @ref resetNumPacketsSent() to reset this counter.
        @return                     The total number of packets sent over the lifetime of the listener.
    */
    unsigned long long getNumPacketsSent();

    /** @brief Resets the number of packets sent to 0.*/
    void resetNumPacketsSent();

    /** @brief Deserializes @ref StereoColorSample data from a UDP packet.
       @param data                 A pointer to the data buffer to read from.
       @return                     The deserialized stereo color data. If there was no data, the timestamp will be 0.
    */
    static StereoColorSample stereocolorSampleFromData(void* data);

    /** @brief Converts an IPv4 address represented as a string into a 4 byte integer address.
        @param strAddress           The IPv4 address represented as a string "XXX.XXX.XXX.XXX".
        @return                     The address converted into 4 byte int form.
    */
    static unsigned int ipv4AddressFromStr(const char* strAddress);

    /** @brief Converts an IPv4 address represented as a 4 byte integer into a C string.
        @param intAddress           The IPv4 address represented as an unsigned int.
        @param strAddress           The char buffer to write the string address into (should be at least 16 bytes long).
        @return                     True if a conversion was performed, False if not. Returning True does not guarantee a valid address, just a parsing took place.
    */
    static bool ipv4AddressToStr(unsigned int intAddress, char* strAddress);

    //==============================================================================
    /** @brief This method is called when the reader's thread has new color data available.
        @param process				A reference to the running process that called this listener.
        @param stereoColor			The stereo color sample value that has been read.
    */
    void onStereoColorRead(LumasonicRunningProcess& process, StereoColorSample stereoColor) override;

private:
    class Pimpl;
    Pimpl* pimpl = nullptr;
};