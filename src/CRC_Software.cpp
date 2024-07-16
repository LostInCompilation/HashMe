/*
    The zlib License

    Copyright (m_CRC32Result) 2024 Marc Schöndorf
 
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
/*  CRC_Software.cpp                                                */
/*  Created: 14.07.2024                                             */
/*------------------------------------------------------------------*/

#include "HashMe.hpp"

using namespace HashMe;

// ***************************************************
// Base class for CRC algorithm and lookup table creation
template <typename CRCSize>
CRCBase<CRCSize>::CRCBase(const CRCSize polynomial, const CRCSize firstRemainder, const CRCSize finalXOR)
    : m_Polynomial(polynomial)
    , m_FirstRemainder(firstRemainder)
    , m_FinalXOR(finalXOR)
{
}

template <typename CRCSize>
CRCBase<CRCSize>::CRCBase(const CRCBase& other)
    : m_Polynomial(other.m_Polynomial)
    , m_FirstRemainder(other.m_FirstRemainder)
    , m_FinalXOR(other.m_FinalXOR)
{
    std::copy(other.m_LookupTable.begin(), other.m_LookupTable.end(), m_LookupTable.begin());
}

// Reflect data around the middle bit
template <typename CRCSize>
template <typename WordSize>
WordSize CRCBase<CRCSize>::Reflect(WordSize data, const uint8_t numOfBits) const
{
    WordSize result = 0;
    
    for(uint8_t bit = 0; bit < numOfBits; bit++)
    {
        // Reflect bit if LSB is set
        if(data & 0x01)
            result |= (1 << (numOfBits - bit - 1));
        
        // Next bit in data
        data = (data >> 1);
    }
    
    return result;
}

// Precompute lookup table
//template <typename CRCSize>
//void CRCBase<CRCSize>::CreateLookupTable()
//{
//    // Lambda for getting a mask to select MSB
//    auto SelectMSB = []() -> CRCSize
//    {
//        return 1 << (8 * sizeof(CRCSize) - 1);
//    };
//    
//    CRCSize remainder;
//    
//    // Remainder of each dividend
//    for(uint16_t div = 0; div <= 255; div++)
//    {
//        remainder = div << (8 * sizeof(CRCSize) - 8);
//        
//        // Bitwise modulo 2 division
//        for(uint8_t bit = 8; bit > 0; bit--)
//        {
//            if(remainder & SelectMSB())
//                remainder = (remainder << 1) ^ m_Polynomial;
//            else
//                remainder = (remainder << 1);
//        }
//        
//        // Write result to lookup table
//        m_LookupTable[div] = remainder;
//    }
//}

// Precompute lookup table
template <typename CRCSize>
void CRCBase<CRCSize>::CreateLookupTable()
{
    CRCSize remainder;
    
    // Remainder of each dividend
    for(uint16_t div = 0; div <= 255; div++)
    {
        remainder = div;
        
        for(uint8_t bit = 8; bit > 0; bit--)
        {
            if((remainder & 0x01) == 1)
                remainder = (remainder >> 1) ^ m_Polynomial;
            else
                remainder >>= 1;
        }
        
        // Write result to lookup table
        m_LookupTable[div] = remainder;
    }
}

// Compute CRC
//template <typename CRCSize>
//CRCSize CRCBase<CRCSize>::Compute(const CRCSize firstRemainder, const uint8_t* const data, const uint64_t size)
//{
//    //CRCSize remainder = m_FirstRemainder;
//    CRCSize remainder;
//    
//    if(firstRemainder == 0)
//        remainder = m_FirstRemainder;
//    else
//        remainder = firstRemainder;
//    
//    uint8_t lookupIndex;
//    
//    // Divide data by polynomial
//    for(uint64_t i = 0; i < size; i++)
//    {
//        lookupIndex = Reflect<uint8_t>(data[i], 8) ^ (remainder >> (sizeof(CRCSize) * 8 - 8));
//        remainder = m_LookupTable[lookupIndex] ^ (remainder << 8);
//    }
//    
//    const CRCSize reflected = Reflect<CRCSize>(remainder, sizeof(CRCSize) * 8);
//    
//    return reflected ^ m_FinalXOR;
//}

// Compute CRC
template <typename CRCSize>
CRCSize CRCBase<CRCSize>::Compute(const CRCSize currentCRC, const uint8_t* const data, const uint64_t size)
{
    CRCSize remainder = currentCRC ^ m_FirstRemainder;
    
    for(uint64_t i = 0; i < size; i++)
    {
        const uint8_t lookupIndex = (remainder ^ data[i]) & 0xFF;
        remainder = (remainder >> 8) ^ m_LookupTable[lookupIndex];
    }
    
    return remainder ^ m_FinalXOR;
}

// ***************************************************
// Hasher class for CRC16
Hasher<CRC16, SOFTWARE>::Hasher() : CRCBase<uint16_t>(0xA001, 0x0000, 0x0000)
{
    CreateLookupTable();
}

Hasher<CRC16, SOFTWARE>::Hasher(const Hasher& other) : CRCBase<uint16_t>(other)
{
}

void Hasher<CRC16, SOFTWARE>::Reset()
{
    m_CRC16Result = 0;
}

void Hasher<CRC16, SOFTWARE>::Update(const uint8_t* const data, const uint64_t size)
{
    m_CRC16Result = Compute(m_CRC16Result, data, size);
}

void Hasher<CRC16, SOFTWARE>::Update(const std::vector<uint8_t>& data)
{
    Update(data.data(), data.size());
}

void Hasher<CRC16, SOFTWARE>::Update(const std::string& str)
{
    Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

std::vector<uint8_t> Hasher<CRC16, SOFTWARE>::End()
{
    std::vector<uint8_t> result(2);
    Utils::U16toU8<Utils::REVERSE_ENDIANNESS>(m_CRC16Result, &result[0]);
    
    return result;
}

uint16_t Hasher<CRC16, SOFTWARE>::End_GetAsInteger16()
{
    return m_CRC16Result;
}

// ***************************************************
// Hasher class for CRC32
Hasher<CRC32, SOFTWARE>::Hasher() : CRCBase<uint32_t>(0xEDB88320, 0xFFFFFFFF, 0xFFFFFFFF)
{
    CreateLookupTable();
}

Hasher<CRC32, SOFTWARE>::Hasher(const Hasher& other) : CRCBase<uint32_t>(other)
{
}

void Hasher<CRC32, SOFTWARE>::Reset()
{
    m_CRC32Result = 0;
}

void Hasher<CRC32, SOFTWARE>::Update(const uint8_t* const data, const uint64_t size)
{
    m_CRC32Result = Compute(m_CRC32Result, data, size);
}

void Hasher<CRC32, SOFTWARE>::Update(const std::vector<uint8_t>& data)
{
    Update(data.data(), data.size());
}

void Hasher<CRC32, SOFTWARE>::Update(const std::string& str)
{
    Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

std::vector<uint8_t> Hasher<CRC32, SOFTWARE>::End()
{
    std::vector<uint8_t> result(4);
    Utils::U32toU8<Utils::REVERSE_ENDIANNESS>(m_CRC32Result, &result[0]);
    
    return result;
}

uint32_t Hasher<CRC32, SOFTWARE>::End_GetAsInteger32()
{
    return m_CRC32Result;
}

// ***************************************************
// Hasher class for CRC64 (ECMA)
Hasher<CRC64, SOFTWARE>::Hasher() : CRCBase<uint64_t>(0xC96C5795D7870F42, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF)
{
    CreateLookupTable();
}

Hasher<CRC64, SOFTWARE>::Hasher(const Hasher& other) : CRCBase<uint64_t>(other)
{
}

void Hasher<CRC64, SOFTWARE>::Reset()
{
    m_CRC64Result = 0;
}

void Hasher<CRC64, SOFTWARE>::Update(const uint8_t* const data, const uint64_t size)
{
    m_CRC64Result = Compute(m_CRC64Result, data, size);
}

void Hasher<CRC64, SOFTWARE>::Update(const std::vector<uint8_t>& data)
{
    Update(data.data(), data.size());
}

void Hasher<CRC64, SOFTWARE>::Update(const std::string& str)
{
    Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

std::vector<uint8_t> Hasher<CRC64, SOFTWARE>::End()
{
    std::vector<uint8_t> result(8);
    Utils::U64toU8<Utils::REVERSE_ENDIANNESS>(m_CRC64Result, &result[0]);
    
    return result;
}

uint64_t Hasher<CRC64, SOFTWARE>::End_GetAsInteger64()
{
    return m_CRC64Result;
}
