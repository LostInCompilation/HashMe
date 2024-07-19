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
/*  SHA384.cpp                                                      */
/*  Created: 19.07.2024                                             */
/*------------------------------------------------------------------*/

#include "HashMe.hpp"

using namespace HashMe;

Hasher<SHA384, SOFTWARE>::Hasher()
{
    Initialize();
}

Hasher<SHA384, SOFTWARE>::Hasher(const Hasher& other) : Hasher<SHA512, SOFTWARE>(other)
{
}

void Hasher<SHA384, SOFTWARE>::Initialize()
{
    // Set state to initial hash values
    std::copy(INITIAL_HASH_VALUES.begin(), INITIAL_HASH_VALUES.end(), m_Context->state);
    
    m_Context->bufferSize = 0;
    m_Context->numOfBits = 0;
}

void Hasher<SHA384, SOFTWARE>::Reset()
{
    Initialize();
    
    // Zero out buffer
    std::memset(m_Context->buffer, 0, SHA512_BLOCK_LENGTH);
}

std::vector<uint8_t> Hasher<SHA384, SOFTWARE>::End()
{
    // Pad last block
    if(m_Context->bufferSize < SHA512_BLOCK_LENGTH - 8)
    {
        std::memset(m_Context->buffer + m_Context->bufferSize, 0x00, SHA512_BLOCK_LENGTH - 8 - m_Context->bufferSize);
        m_Context->buffer[m_Context->bufferSize] = 0x80;
    }
    else
    {
        std::memset(m_Context->buffer + m_Context->bufferSize, 0x00, SHA512_BLOCK_LENGTH - m_Context->bufferSize);
        m_Context->buffer[m_Context->bufferSize] = 0x80;
        
        Transform(m_Context->buffer);
        
        std::memset(m_Context->buffer, 0x00, SHA512_BLOCK_LENGTH - 8);
    }
    
    // Append full message length to padding block
    m_Context->buffer[SHA512_BLOCK_LENGTH - 1] = static_cast<uint8_t>(m_Context->numOfBits);
    m_Context->buffer[SHA512_BLOCK_LENGTH - 2] = static_cast<uint8_t>(m_Context->numOfBits >> 8);
    m_Context->buffer[SHA512_BLOCK_LENGTH - 3] = static_cast<uint8_t>(m_Context->numOfBits >> 16);
    m_Context->buffer[SHA512_BLOCK_LENGTH - 4] = static_cast<uint8_t>(m_Context->numOfBits >> 24);
    m_Context->buffer[SHA512_BLOCK_LENGTH - 5] = static_cast<uint8_t>(m_Context->numOfBits >> 32);
    m_Context->buffer[SHA512_BLOCK_LENGTH - 6] = static_cast<uint8_t>(m_Context->numOfBits >> 40);
    m_Context->buffer[SHA512_BLOCK_LENGTH - 7] = static_cast<uint8_t>(m_Context->numOfBits >> 48);
    m_Context->buffer[SHA512_BLOCK_LENGTH - 8] = static_cast<uint8_t>(m_Context->numOfBits >> 56);
    
    // Transform
    Transform(m_Context->buffer);
    
    // Assemble hash, omit last 16 bytes for SHA384
    std::vector<uint8_t> hash(48); // 384 bit hash
    for(uint32_t i = 0; i < 6; i++)
    {
#ifdef HM_LITTLE_ENDIAN
        Utils::U64toU8<Utils::REVERSE_ENDIANNESS>(m_Context->state[i], &hash[i << 3]); // Transform SHA big endian to host little endian
#else
        Utils::U64toU8<Utils::KEEP_ENDIANNESS>(m_Context->state[i], &hash[i << 3]);
#endif
    }
    
    return hash;
}
