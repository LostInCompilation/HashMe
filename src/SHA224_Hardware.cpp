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
/*  SHA224.cpp                                                      */
/*  Created: 14.07.2024                                             */
/*------------------------------------------------------------------*/

#include "HashMe.hpp"

#if defined(HM_SIMD_ARM) || defined(HM_SIMD_X86)

using namespace HashMe;

Hasher<SHA224, HARDWARE>::Hasher()
{
    Initialize();
}

Hasher<SHA224, HARDWARE>::Hasher(const Hasher& other) : Hasher<SHA256, HARDWARE>(other)
{
}

void Hasher<SHA224, HARDWARE>::Initialize()
{
    // Set state to initial hash values
    std::copy(INITIAL_HASH_VALUES.begin(), INITIAL_HASH_VALUES.end(), m_Context->state);
    
    m_Context->bufferSize = 0;
    m_Context->numOfBits = 0;
}

void Hasher<SHA224, HARDWARE>::Reset()
{
    Initialize();
    
    // Zero out buffer
    std::memset(m_Context->buffer, 0, SHA256_BLOCK_LENGTH);
}

std::vector<uint8_t> Hasher<SHA224, HARDWARE>::End()
{
#ifndef NDEBUG
    if(m_Context->bufferSize == SHA256_BLOCK_LENGTH) // Check for bug
        throw std::runtime_error("Buffer should never be completely filled and untransformed when entering the End() function.");
#endif
    
    // Pad last block
    if(m_Context->bufferSize < SHA256_BLOCK_LENGTH - 8)
    {
        std::memset(m_Context->buffer + m_Context->bufferSize, 0x00, SHA256_BLOCK_LENGTH - 8 - m_Context->bufferSize);
        m_Context->buffer[m_Context->bufferSize] = 0x80;
    }
    else
    {
        std::memset(m_Context->buffer + m_Context->bufferSize, 0x00, SHA256_BLOCK_LENGTH - m_Context->bufferSize);
        m_Context->buffer[m_Context->bufferSize] = 0x80;
        
        Transform(m_Context->buffer);
        
        std::memset(m_Context->buffer, 0x00, SHA256_BLOCK_LENGTH - 8);
    }
    
    // Append full message length to padding block
    m_Context->buffer[SHA256_BLOCK_LENGTH - 1] = static_cast<uint8_t>(m_Context->numOfBits);
    m_Context->buffer[SHA256_BLOCK_LENGTH - 2] = static_cast<uint8_t>(m_Context->numOfBits >> 8);
    m_Context->buffer[SHA256_BLOCK_LENGTH - 3] = static_cast<uint8_t>(m_Context->numOfBits >> 16);
    m_Context->buffer[SHA256_BLOCK_LENGTH - 4] = static_cast<uint8_t>(m_Context->numOfBits >> 24);
    m_Context->buffer[SHA256_BLOCK_LENGTH - 5] = static_cast<uint8_t>(m_Context->numOfBits >> 32);
    m_Context->buffer[SHA256_BLOCK_LENGTH - 6] = static_cast<uint8_t>(m_Context->numOfBits >> 40);
    m_Context->buffer[SHA256_BLOCK_LENGTH - 7] = static_cast<uint8_t>(m_Context->numOfBits >> 48);
    m_Context->buffer[SHA256_BLOCK_LENGTH - 8] = static_cast<uint8_t>(m_Context->numOfBits >> 56);
    
    // Transform
    Transform(m_Context->buffer);
    
    // Assemble hash
    std::vector<uint8_t> hash(28); // 256 bit hash
    for(uint32_t i = 0; i < 7; i++)
    {
#ifdef HM_LITTLE_ENDIAN
        Utils::U32toU8<Utils::REVERSE_ENDIANNESS>(m_Context->state[i], &hash[i << 2]); // Transform SHA big endian to host little endian
#else
        Utils::U32toU8<Utils::KEEP_ENDIANNESS>(m_Context->state[i], &hash[i << 2]);
#endif
    }
    
    return hash;
}

#endif /* HM_SIMD_ARM || HM_SIMD_X86 */
