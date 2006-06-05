/*
 *  Copyright 2005-2006 The Apache Software Foundation or its licensors, as applicable.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/** 
 * @author Pavel Pervov
 * @version $Revision: 1.1.2.2.4.4 $
 */

#ifndef _BYTEREADER_H_
#define _BYTEREADER_H_

#include "open/types.h"
#include "cxxlog.h"

class ByteReader
{
    unsigned offset;
    unsigned len;
    const uint8* bytebuffer;
    const uint8* start;
    const uint8* end;
    const uint8* curr;
    bool byte_array_owner;
public:
    ByteReader(const uint8* bytecode, unsigned _offset, unsigned _len)
        : offset(_offset), len(_len), bytebuffer(bytecode),
          start(bytecode + offset), end(start + len),
          curr(bytecode + offset), byte_array_owner(false) {}
    ~ByteReader() {
        if(byte_array_owner)
            delete[] bytebuffer;
    } // ~ByteReader

    int get_offset()
    {
        return (unsigned)(offset + (curr - start)); // ?: casting for IPF
    } // get_offset
    void dump(unsigned num_bytes);

    // The following functions return true if there was no error.
    // "_be" suffix means "big endian".
    // "_le" suffix means "little endian".
    bool go_to_offset(unsigned _offset)
    {
        int num_bytes_to_skip = _offset - get_offset();
        return skip(num_bytes_to_skip);
    } // go_to_offset

    bool skip(int num_bytes)
    {
        if(!have(num_bytes)) return false;

        curr += num_bytes;
        return true;
    } // skip

    bool parse_u4_be(uint32 * val)
    {
        if(!have(4)) return false;

        uint32 result = 0;
        for (int i = 0; i < 4; i++) {
            uint32 x = (uint32) * curr++;
            result = (result << 8) + x;
        }
        *val = result;
        return true;
    } // parse_u4_be

    bool parse_u4_le(uint32 * val)
    {
        if(!have(4)) return false;

        uint32 result = 0;
        curr += 4;
        const uint8* curr_byte = curr;
        for (int i = 0; i < 4; i++) {
            uint32 x = (uint32) * (--curr_byte);
            result = (result << 8) + x;
        }
        *val = result;
        return true;
    } // parse_u4_le

    bool parse_u2_be(uint16 * val)
    {
        if(!have(2)) return false;

        uint16 result = 0;
        for (int i = 0; i < 2; i++) {
            uint16 x = (uint16) * curr++;
            result = (uint16) ((result << 8) + x);
        }
        *val = result;
        return true;
    } // parse_u2_be

    bool parse_u2_le(uint16 * val)
    {
        if(!have(2)) return false;

        uint16 result = 0;
        curr += 2;
        const uint8* curr_byte = curr;
        for (int i = 0; i < 2; i++) {
            uint16 x = (uint16) * (--curr_byte);
            result = (uint16) ((result << 8) + x);
        }
        *val = result;
        return true;
    } // parse_u2_le

    bool parse_u1(uint8 * val)
    {
        if(!have(1)) return false;

        *val = *curr++;
        return true;
    } // parse_u1

    const uint8* get_and_skip(unsigned len) {
        const uint8* _curr = curr;
        if(!skip(len)) return NULL;
        return _curr;
    } // get_and_skip

    bool have(int num_bytes) {
        if(curr + num_bytes > end) {
            TRACE2("bytereader", "ByteReader::have: EOB");
            return false;
        }
        if(curr + num_bytes < start) {
            TRACE2("bytereader", "ByteReader::have: BOB");
            return false;
        }
        return true;
    } // have
}; //ByteReader

#endif
