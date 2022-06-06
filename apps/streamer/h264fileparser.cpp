/*
 * libdatachannel streamer example
 * Copyright (c) 2020 Filip Klembara (in2core)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#include "h264fileparser.hpp"
#include "rtc/rtc.hpp"

#include <fstream>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

using namespace std;

H264FileParser::H264FileParser(string directory, uint32_t fps, bool loop): FileParser(directory, ".h264", fps, loop) {
    this->sock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(10000);
    std::cout << "Creating the socket for RTP connection" << std::endl;
    int flag = 1;  
    if (-1 == setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {  
        throw std::runtime_error("setsockopt fail");  
    }  

    if (bind(this->sock, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error("Failed to bind UDP socket on 127.0.0.1:10000");
    int rcvBufSize = 212992;
    setsockopt(this->sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char *>(&rcvBufSize),
                sizeof(rcvBufSize));
 }

void H264FileParser::loadNextFrame() {

    // Receive from UDP
    const int BUFFER_SIZE = 2048;

    char buffer[BUFFER_SIZE];
    //std::cout<< "Load next frame ...." << std::endl;
    int len = recv(this->sock, buffer, BUFFER_SIZE, 0);

    if (len >= 0 && len > sizeof(rtc::RtpHeader)) {
        auto rtp = reinterpret_cast<rtc::RtpHeader *>(buffer);
        const rtc::SSRC ssrc = 42;
		rtp->setSsrc(ssrc);
        //char *p = buffer;
        sample.resize(len);
        //std::transform(p, p + len, sample.begin(), [](char v) {return static_cast<byte>(v);});
        memcpy(&sample[0], &buffer[0], len*sizeof(byte));
        sampleTime_us += sampleDuration_us;
    }
}

void H264FileParser::loadNextSample() {
    //FileParser::loadNextSample();
    H264FileParser::loadNextFrame();
    /*size_t i = 0;
    while (i < sample.size()) {
        assert(i + 4 < sample.size());
        auto lengthPtr = (uint32_t *) (sample.data() + i);
        uint32_t length = ntohl(*lengthPtr);
        auto naluStartIndex = i + 4;
        auto naluEndIndex = naluStartIndex + length;
        assert(naluEndIndex <= sample.size());
        auto header = reinterpret_cast<rtc::NalUnitHeader *>(sample.data() + naluStartIndex);
        auto type = header->unitType();
        switch (type) {
            case 7:
                previousUnitType7 = {sample.begin() + i, sample.begin() + naluEndIndex};
                break;
            case 8:
                previousUnitType8 = {sample.begin() + i, sample.begin() + naluEndIndex};;
                break;
            case 5:
                previousUnitType5 = {sample.begin() + i, sample.begin() + naluEndIndex};;
                break;
        }
        i = naluEndIndex;
    }*/
}

vector<byte> H264FileParser::initialNALUS() {
    vector<byte> units{};
    if (previousUnitType7.has_value()) {
        auto nalu = previousUnitType7.value();
        units.insert(units.end(), nalu.begin(), nalu.end());
    }
    if (previousUnitType8.has_value()) {
        auto nalu = previousUnitType8.value();
        units.insert(units.end(), nalu.begin(), nalu.end());
    }
    if (previousUnitType5.has_value()) {
        auto nalu = previousUnitType5.value();
        units.insert(units.end(), nalu.begin(), nalu.end());
    }
    return units;
}
