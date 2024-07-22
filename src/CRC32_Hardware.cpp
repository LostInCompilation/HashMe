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
/*  CRC32_Hardware.cpp                                              */
/*  Created: 13.07.2024                                             */
/*------------------------------------------------------------------*/

#include "HashMe.hpp"

#if defined(HM_SIMD_ARM) || defined(HM_SIMD_X86)

using namespace HashMe;

Hasher<CRC32, HARDWARE>::Hasher(const Hasher& other)
{
    m_CRC32Result = other.m_CRC32Result;
}

void Hasher<CRC32, HARDWARE>::Reset()
{
    m_CRC32Result = 0;
}

#if defined(HM_SIMD_ARM)
void Hasher<CRC32, HARDWARE>::Update(const uint8_t* const data, const uint64_t size)
{
    if(!data)
        throw std::invalid_argument("Data cannot be nullptr.");
    
    if(size == 0)
        throw std::invalid_argument("Data size cannot be zero.");
    
    const uint8_t* buffer8 = data;
    const uint64_t* buffer64 = reinterpret_cast<const uint64_t*>(data);
    
    uint32_t result = ~m_CRC32Result;
    uint64_t bytesToProcess = size;
    
    while(bytesToProcess > 0 && (reinterpret_cast<std::uintptr_t>(buffer8) & 7) > 0)
    {
        result = __crc32b(result, *buffer8++);
        bytesToProcess--;
    }
    
    while(bytesToProcess >= 64)
    {
        result = __crc32d(result, *buffer64++);
        result = __crc32d(result, *buffer64++);
        result = __crc32d(result, *buffer64++);
        result = __crc32d(result, *buffer64++);

        result = __crc32d(result, *buffer64++);
        result = __crc32d(result, *buffer64++);
        result = __crc32d(result, *buffer64++);
        result = __crc32d(result, *buffer64++);
        
        bytesToProcess -= 64;
    }
    
    while(bytesToProcess >= 8)
    {
        result = __crc32d(result, *buffer64++);
        bytesToProcess -= 8;
    }
    
    buffer8 = reinterpret_cast<const uint8_t*>(buffer64);
    
    while(bytesToProcess > 0)
    {
        result = __crc32b(result, *buffer8++);
        bytesToProcess--;
    }
    
    m_CRC32Result = ~result;
}
#elif defined(HM_SIMD_X86)
void Hasher<CRC32, HARDWARE>::Update(const uint8_t* const data, const uint64_t size)
{
    static_assert(0, "Not implemented");
}
#endif

void Hasher<CRC32, HARDWARE>::Update(const std::vector<uint8_t>& data)
{
    Update(data.data(), data.size());
}

void Hasher<CRC32, HARDWARE>::Update(const std::string& str)
{
    Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

std::vector<uint8_t> Hasher<CRC32, HARDWARE>::End()
{
    std::vector<uint8_t> result(4);
    Utils::U32toU8<Utils::REVERSE_ENDIANNESS>(m_CRC32Result, &result[0]);
    
    return result;
}

uint32_t Hasher<CRC32, HARDWARE>::End_GetAsInteger32()
{
    return m_CRC32Result;
}

#endif /* HM_SIMD_ARM || HM_SIMD_X86 */
