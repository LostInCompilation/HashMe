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
/*  SHA256.cpp                                                      */
/*  Created: 26.06.2024                                             */
/*------------------------------------------------------------------*/

#include "HashMe.hpp"

using namespace HashMe;

// Constants
constexpr std::array<uint32_t, 64> K = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

constexpr std::array<uint32_t, 8> INITIAL_HASH_VALUES = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

// SHA256 logic functions
constexpr inline uint32_t Sigma0(const uint32_t x)     { return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3); }
constexpr inline uint32_t Sigma1(const uint32_t x)     { return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10); }
constexpr inline uint32_t Epsilon0(const uint32_t x)   { return std::rotr(x, 2) ^ std::rotr(x, 13) ^ std::rotr(x, 22); }
constexpr inline uint32_t Epsilon1(const uint32_t x)   { return std::rotr(x, 6) ^ std::rotr(x, 11) ^ std::rotr(x, 25); }
constexpr inline uint32_t Majority(const uint32_t x, const uint32_t y, const uint32_t z)   { return (x & y) ^ (x & z) ^ (y & z); }
constexpr inline uint32_t Choose(const uint32_t x, const uint32_t y, const uint32_t z)     { return (x & y) ^ (~x & z); }

Hasher<SHA256, SOFTWARE>::Hasher()
{
    m_Context = new Context;
    
    Initialize();
}

Hasher<SHA256, SOFTWARE>::~Hasher()
{
    delete m_Context;
}

Hasher<SHA256, SOFTWARE>::Hasher(const Hasher& other)
{
    m_Context = new Context;
    
    m_Context->bufferSize = other.m_Context->bufferSize;
    m_Context->numOfBits = other.m_Context->numOfBits;
    
    std::copy(other.m_Context->state, other.m_Context->state + 8, m_Context->state);
    std::copy(other.m_Context->buffer, other.m_Context->buffer + SHA256_BLOCK_LENGTH, m_Context->buffer);
}

void Hasher<SHA256, SOFTWARE>::Initialize()
{
    // Set state to initial hash values
    std::copy(INITIAL_HASH_VALUES.begin(), INITIAL_HASH_VALUES.end(), m_Context->state);
    
    m_Context->bufferSize = 0;
    m_Context->numOfBits = 0;
}

void Hasher<SHA256, SOFTWARE>::Reset()
{
    Initialize();
    
    // Zero out buffer
    std::memset(m_Context->buffer, 0, SHA256_BLOCK_LENGTH);
}

// Optimized transform function
void Hasher<SHA256, SOFTWARE>::Transform(const uint8_t* const data)
{
    uint32_t block[SHA256_BLOCK_LENGTH];
    
    // Initial values
    uint32_t a = m_Context->state[0];
    uint32_t b = m_Context->state[1];
    uint32_t c = m_Context->state[2];
    uint32_t d = m_Context->state[3];
    uint32_t e = m_Context->state[4];
    uint32_t f = m_Context->state[5];
    uint32_t g = m_Context->state[6];
    uint32_t h = m_Context->state[7];
    uint32_t t1;
    uint32_t t2;
    
    uint8_t x = 0;
    uint8_t i = 0;
    for(; i < 16; i++)
    {
        // Copy 1-byte data to 4-byte array
#ifdef HM_LITTLE_ENDIAN
        block[i] = Utils::U8toU32(&data[x], true); // Convert to big endian
#else
        block[i] = Utils::U8toU32(&data[x], false);
#endif
        
        x += 4;
        
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
    
    uint32_t s0;
    uint32_t s1;
    for(; i < 64; i++)
    {
        s0 = Sigma0(block[(i + 1) & 0x0F]);
        s1 = Sigma1(block[(i + 14) & 0x0F]);
        
        const uint32_t tmp = (block[i & 0x0F] += s1 + block[(i + 9) & 0x0F] + s0);
        
        t1 = h + Epsilon1(e) + Choose(e, f, g) + K[i] + tmp;
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
    //a = b = c = d = e = f = g = h = t1 = t2 = 0;
}

// Optimized update function
void Hasher<SHA256, SOFTWARE>::Update(const uint8_t* const data, const uint64_t size)
{
    if(!data)
        throw std::invalid_argument("Data cannot be nullptr.");
    
    if(size == 0)
        throw std::invalid_argument("Data size cannot be zero.");
    
    // Copy data size to use it as our counter
    const uint64_t lastBlockSize = size & (SHA256_BLOCK_LENGTH - 1); // size % SHA256_BLOCK_LENGTH
    uint64_t dataIndex = 0;
    
    // Fill buffer with SHA256_BLOCK_LENGTH bytes
    while(dataIndex < size)
    {
        if((size - dataIndex) >= SHA256_BLOCK_LENGTH)
        {
            // Copy one whole block and transform it
            std::copy(data + dataIndex, data + dataIndex + SHA256_BLOCK_LENGTH, m_Context->buffer);
            m_Context->bufferSize += SHA256_BLOCK_LENGTH;
            
            Transform(m_Context->buffer);
            
            m_Context->bufferSize = 0;
            m_Context->numOfBits += 512;
            
            dataIndex += SHA256_BLOCK_LENGTH;
        }
        else
        {
            // Copy partial (last) block to buffer, transform it inside the End() method
            std::copy(data + dataIndex, data + dataIndex + lastBlockSize, m_Context->buffer);
            m_Context->bufferSize += lastBlockSize;
            
            dataIndex += lastBlockSize; // End loop
        }
    }
}

void Hasher<SHA256, SOFTWARE>::Update(const std::vector<uint8_t>& data)
{
    Update(data.data(), data.size());
}

void Hasher<SHA256, SOFTWARE>::Update(const std::string& str)
{
    Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

std::vector<uint8_t> Hasher<SHA256, SOFTWARE>::End()
{
    uint8_t bufferSize = m_Context->bufferSize;
    
    // Pad last block
    if(bufferSize < 56)
    {
        m_Context->buffer[bufferSize++] = 0x80;
        std::memset(&m_Context->buffer[bufferSize], 0x00, 56 - bufferSize);
    }
    else
    {
        m_Context->buffer[bufferSize++] = 0x80;
        std::memset(&m_Context->buffer[bufferSize], 0x00, 64 - bufferSize);
        
        Transform(m_Context->buffer);
        
        std::memset(m_Context->buffer, 0x00, 56);
    }
    
    // Append message length in bits
    m_Context->numOfBits += m_Context->bufferSize * 8;
    
    m_Context->buffer[63] = m_Context->numOfBits;
    m_Context->buffer[62] = m_Context->numOfBits >> 8;
    m_Context->buffer[61] = m_Context->numOfBits >> 16;
    m_Context->buffer[60] = m_Context->numOfBits >> 24;
    
    m_Context->buffer[59] = m_Context->numOfBits >> 32;
    m_Context->buffer[58] = m_Context->numOfBits >> 40;
    m_Context->buffer[57] = m_Context->numOfBits >> 48;
    m_Context->buffer[56] = m_Context->numOfBits >> 56;
    
    // Transform
    Transform(m_Context->buffer);
    
    std::vector<uint8_t> hash(32); // 256 bit hash
    for(uint8_t i = 0; i < 8; i++)
    {
#ifdef HM_LITTLE_ENDIAN
        Utils::U32toU8(m_Context->state[i], true, &hash[i * 4]); // Transform SHA big endian to host little endian
#else
        Utils::U32toU8(m_Context->state[i], false, &hash[i * 4]);
#endif
    }
    
    return hash;
}
