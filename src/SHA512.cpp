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
/*  SHA512.cpp                                                      */
/*  Created: 18.07.2024                                             */
/*------------------------------------------------------------------*/

#include "HashMe.hpp"

using namespace HashMe;

// SHA512 logic functions
inline static constexpr uint64_t Sigma0(const uint64_t x)     { return std::rotr(x, 1) ^ std::rotr(x, 8) ^ (x >> 7); }
inline static constexpr uint64_t Sigma1(const uint64_t x)     { return std::rotr(x, 19) ^ std::rotr(x, 61) ^ (x >> 6); }
inline static constexpr uint64_t Epsilon0(const uint64_t x)   { return std::rotr(x, 28) ^ std::rotr(x, 34) ^ std::rotr(x, 39); }
inline static constexpr uint64_t Epsilon1(const uint64_t x)   { return std::rotr(x, 14) ^ std::rotr(x, 18) ^ std::rotr(x, 41); }
inline static constexpr uint64_t Majority(const uint64_t x, const uint64_t y, const uint64_t z)   { return (x & y) ^ (x & z) ^ (y & z); }
inline static constexpr uint64_t Choose(const uint64_t x, const uint64_t y, const uint64_t z)     { return (x & y) ^ (~x & z); }

Hasher<SHA512, SOFTWARE>::Hasher()
{
    m_Context = std::make_unique<Context>();
    
    Initialize();
}

Hasher<SHA512, SOFTWARE>::Hasher(const Hasher& other)
{
    m_Context = std::make_unique<Context>();
    
    m_Context->bufferSize = other.m_Context->bufferSize;
    m_Context->numOfBits = other.m_Context->numOfBits;
    
    std::copy(other.m_Context->state, other.m_Context->state + 8, m_Context->state);
    std::copy(other.m_Context->buffer, other.m_Context->buffer + SHA512_BLOCK_LENGTH, m_Context->buffer);
}

void Hasher<SHA512, SOFTWARE>::Initialize()
{
    // Set state to initial hash values
    std::copy(INITIAL_HASH_VALUES.begin(), INITIAL_HASH_VALUES.end(), m_Context->state);
    
    m_Context->bufferSize = 0;
    m_Context->numOfBits = 0;
}

void Hasher<SHA512, SOFTWARE>::Reset()
{
    Initialize();
    
    // Zero out buffer
    std::memset(m_Context->buffer, 0, SHA512_BLOCK_LENGTH);
}

// Optimized transform function
void Hasher<SHA512, SOFTWARE>::Transform(const uint8_t* const data)
{
    uint64_t block[SHA512_BLOCK_LENGTH];
    
    // Initial values
    uint64_t a = m_Context->state[0];
    uint64_t b = m_Context->state[1];
    uint64_t c = m_Context->state[2];
    uint64_t d = m_Context->state[3];
    uint64_t e = m_Context->state[4];
    uint64_t f = m_Context->state[5];
    uint64_t g = m_Context->state[6];
    uint64_t h = m_Context->state[7];
    uint64_t t1;
    uint64_t t2;
    
    uint8_t x = 0;
    uint8_t i = 0;
    for(; i < 16; i++)
    {
        // Copy 1-byte data to 8-byte array
#ifdef HM_LITTLE_ENDIAN
        block[i] = Utils::U8toU64<Utils::REVERSE_ENDIANNESS>(&data[x]); // Convert to big endian
#else
        block[i] = Utils::U8toU64<Utils::KEEP_ENDIANNESS>(&data[x]);
#endif
        x += 8;
        
        t1 = h + Epsilon1(e) + Choose(e, f, g) + K[i] + block[i];
        t2 = Epsilon0(a) + Majority(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    
    for(; i < 80; i++)
    {
        block[i & 0xF] += block[(i + 9) & 0xF] + Sigma0(block[(i + 1) & 0x0F]) + Sigma1(block[(i + 14) & 0x0F]);
        
        t1 = h + Epsilon1(e) + Choose(e, f, g) + K[i] + block[i & 0xF];
        t2 = Epsilon0(a) + Majority(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    
    m_Context->state[0] += a;
    m_Context->state[1] += b;
    m_Context->state[2] += c;
    m_Context->state[3] += d;
    m_Context->state[4] += e;
    m_Context->state[5] += f;
    m_Context->state[6] += g;
    m_Context->state[7] += h;
    
    // Cleanup sensitive data for security
#ifdef HM_CLEANUP_TMP_MEMORY_AFTER_HASHING_FOR_SECURITY
    a = b = c = d = e = f = g = h = t1 = t2 = 0;
#endif
}

// Optimized update function
void Hasher<SHA512, SOFTWARE>::Update(const uint8_t* const data, const uint64_t size)
{
    if(!data)
        throw std::invalid_argument("Data cannot be nullptr.");
    
    if(size == 0)
        throw std::invalid_argument("Data size cannot be zero.");
    
    uint64_t dataIndex = 0;
    uint64_t remainingBytes = size;
    
    while(remainingBytes > 0)
    {
        if(m_Context->bufferSize > 0)
        {
            // Buffer is already partially filled and untransformed
            const uint32_t remainingBufferSpace = SHA512_BLOCK_LENGTH - m_Context->bufferSize;
            const uint32_t bytesToCopy = (remainingBytes > remainingBufferSpace) ? remainingBufferSpace : static_cast<uint32_t>(remainingBytes);
            
            // Copy
            std::copy(data + dataIndex, data + dataIndex + bytesToCopy, m_Context->buffer + m_Context->bufferSize);
            m_Context->bufferSize += bytesToCopy;
            m_Context->numOfBits += (bytesToCopy << 3);
            dataIndex += bytesToCopy;
            remainingBytes -= bytesToCopy;
            
            // Check if buffer is completely filled now
            if(m_Context->bufferSize == SHA512_BLOCK_LENGTH)
            {
                // Buffer is full, transform it now
                Transform(m_Context->buffer);
                m_Context->bufferSize = 0;
            }
        }
        else
        {
            // Buffer is empty
            if(remainingBytes >= SHA512_BLOCK_LENGTH)
            {
                // Copy one whole block in empty buffer and transform it
                std::copy(data + dataIndex, data + dataIndex + SHA512_BLOCK_LENGTH, m_Context->buffer);
                m_Context->numOfBits += 1024;
                dataIndex += SHA512_BLOCK_LENGTH;
                remainingBytes -= SHA512_BLOCK_LENGTH;
                
                // Transform
                Transform(m_Context->buffer);
            }
            else
            {
                // Copy partial (last) block to buffer, transform it inside the End() method
                std::copy(data + dataIndex, data + dataIndex + remainingBytes, m_Context->buffer);
                m_Context->bufferSize += remainingBytes;
                m_Context->numOfBits += (m_Context->bufferSize << 3);
                //dataIndex += remainingBytes;
                remainingBytes = 0; // Terminate loop
            }
        }
    }
}

void Hasher<SHA512, SOFTWARE>::Update(const std::vector<uint8_t>& data)
{
    Update(data.data(), data.size());
}

void Hasher<SHA512, SOFTWARE>::Update(const std::string& str)
{
    Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

std::vector<uint8_t> Hasher<SHA512, SOFTWARE>::End()
{
#ifndef NDEBUG
    if(m_Context->bufferSize == SHA512_BLOCK_LENGTH) // Check for bug
        throw std::runtime_error("Buffer should never be completely filled and untransformed when entering the End() function.");
#endif
    
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
    
    // Assemble hash
    std::vector<uint8_t> hash(64); // 512 bit hash
    for(uint32_t i = 0; i < 8; i++)
    {
#ifdef HM_LITTLE_ENDIAN
        Utils::U64toU8<Utils::REVERSE_ENDIANNESS>(m_Context->state[i], &hash[i << 3]); // Transform SHA big endian to host little endian
#else
        Utils::U64toU8<Utils::KEEP_ENDIANNESS>(m_Context->state[i], &hash[i << 3]);
#endif
    }
    
    return hash;
}