/*
    The zlib License

    Copyright (C) 2024 Marc Schöndorf
 
This software is provided 'as-is', without any express or implied warranty. In
no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject to
the following restrictions:

1.  The origin of this software must not be misrepresented; you must not claim
    that you wrote the original software. If you use this software in a product,
    an acknowledgment in the product documentation would be appreciated but is
    not required.

2.  Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

3.  This notice may not be removed or altered from any source distribution.
*/

/*------------------------------------------------------------------*/
/*                                                                  */
/*                      (C) 2024 Marc Schöndorf                     */
/*                            See license                           */
/*                                                                  */
/*  Utils.hpp                                                       */
/*  Created: 29.06.2024                                             */
/*------------------------------------------------------------------*/

#ifndef Utils_hpp
#define Utils_hpp

namespace HashMe::Utils
{

constexpr inline uint32_t U8toU32(const uint8_t data[4], const bool reverseEndianness)
{
    if(reverseEndianness)
        return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3]);
    else
        return (data[0]) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}

constexpr inline void U32toU8(const uint32_t data, const bool reverseEndianness, uint8_t out[4])
{
    if(reverseEndianness)
    {
        out[0] = static_cast<uint8_t>((data >> 24) & 0xFF);
        out[1] = static_cast<uint8_t>((data >> 16) & 0xFF);
        out[2] = static_cast<uint8_t>((data >> 8) & 0xFF);
        out[3] = static_cast<uint8_t>(data & 0xFF);
    }
    else
    {
        out[0] = static_cast<uint8_t>(data & 0xFF);
        out[1] = static_cast<uint8_t>((data >> 8) & 0xFF);
        out[2] = static_cast<uint8_t>((data >> 16) & 0xFF);
        out[3] = static_cast<uint8_t>((data >> 24) & 0xFF);
    }
}

}

#endif /* Utils_hpp */
