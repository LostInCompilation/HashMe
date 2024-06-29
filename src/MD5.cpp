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
/*  MD5.cpp                                                         */
/*  Created: 29.06.2024                                             */
/*------------------------------------------------------------------*/

#include "HashMe.hpp"

using namespace HashMe;

// Constants
constexpr std::array<uint32_t, 4> INITIAL_HASH_VALUES = {
    0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476
};

constexpr std::array<uint8_t, 64> PADDING = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// MD5 transform constants
constexpr uint32_t S11 = 7;
constexpr uint32_t S12 = 12;
constexpr uint32_t S13 = 17;
constexpr uint32_t S14 = 22;
constexpr uint32_t S21 = 5;
constexpr uint32_t S22 = 9;
constexpr uint32_t S23 = 14;
constexpr uint32_t S24 = 20;
constexpr uint32_t S31 = 4;
constexpr uint32_t S32 = 11;
constexpr uint32_t S33 = 16;
constexpr uint32_t S34 = 23;
constexpr uint32_t S41 = 6;
constexpr uint32_t S42 = 10;
constexpr uint32_t S43 = 15;
constexpr uint32_t S44 = 21;

// Basic MD5 functions
constexpr inline uint32_t F(const uint32_t x, const uint32_t y, const uint32_t z) { return (x & y) | (~x & z); }
constexpr inline uint32_t G(const uint32_t x, const uint32_t y, const uint32_t z) { return (x & z) | (y & ~z); }
constexpr inline uint32_t H(const uint32_t x, const uint32_t y, const uint32_t z) { return (x ^ y ^ z); }
constexpr inline uint32_t I(const uint32_t x, const uint32_t y, const uint32_t z) { return y ^ (x | ~z); }

// Transformations for rounds 1-4
constexpr inline void FF(uint32_t& a, const uint32_t b, const uint32_t c, const uint32_t d,
                      const uint32_t x, const uint32_t s, const uint32_t ac)
{
    a = std::rotl(a + F(b, c, d) + x + ac, s) + b;
}

constexpr inline void GG(uint32_t& a, const uint32_t b, const uint32_t c, const uint32_t d,
                      const uint32_t x, const uint32_t s, const uint32_t ac)
{
    a = std::rotl(a + G(b, c, d) + x + ac, s) + b;
}

constexpr inline void HH(uint32_t& a, const uint32_t b, const uint32_t c, const uint32_t d,
                      const uint32_t x, const uint32_t s, const uint32_t ac)
{
    a = std::rotl(a + H(b, c, d) + x + ac, s) + b;
}

constexpr inline void II(uint32_t& a, const uint32_t b, const uint32_t c, const uint32_t d,
                      const uint32_t x, const uint32_t s, const uint32_t ac)
{
    a = std::rotl(a + I(b, c, d) + x + ac, s) + b;
}

Hasher<MD5, SOFTWARE>::Hasher()
{
    m_Context = new Context;
    
    Initialize();
}

Hasher<MD5, SOFTWARE>::~Hasher()
{
    delete m_Context;
}

Hasher<MD5, SOFTWARE>::Hasher(const Hasher& other)
{
    m_Context = new Context;
    
    std::copy(other.m_Context->count, other.m_Context->count + 2, m_Context->count);
    std::copy(other.m_Context->state, other.m_Context->state + 4, m_Context->state);
    std::copy(other.m_Context->buffer, other.m_Context->buffer + MD5_BLOCK_LENGTH, m_Context->buffer);
}

void Hasher<MD5, SOFTWARE>::Initialize()
{
    // Set state to initial hash values
    std::copy(INITIAL_HASH_VALUES.begin(), INITIAL_HASH_VALUES.end(), m_Context->state);
}

void Hasher<MD5, SOFTWARE>::Reset()
{
    Initialize();
    
    m_Context->count[0] = 0;
    m_Context->count[1] = 0;
}

void Hasher<MD5, SOFTWARE>::Encode(uint8_t* const output, const uint32_t* const input, const uint32_t size)
{
    uint32_t i = 0;
    uint32_t j = 0;
    
    for(; j < size; i++, j += 4)
    {
        output[j] = static_cast<uint8_t>(input[i] & 0xFF);
        output[j + 1] = static_cast<uint8_t>((input[i] >> 8) & 0xFF);
        output[j + 2] = static_cast<uint8_t>((input[i] >> 16) & 0xFF);
        output[j + 3] = static_cast<uint8_t>((input[i] >> 24) & 0xFF);
    }
}

void Hasher<MD5, SOFTWARE>::Decode(uint32_t* const output, const uint8_t* const input, const uint32_t size)
{
    uint32_t i = 0;
    uint32_t j = 0;
    
    for(; j < size; i++, j += 4)
        output[i] = input[j] | (input[j + 1] << 8) | (input[j + 2] << 16) | (input[j + 3] << 24);
}

void Hasher<MD5, SOFTWARE>::Transform(const uint8_t block[MD5_BLOCK_LENGTH])
{
    uint32_t x[16];
    uint32_t a = m_Context->state[0];
    uint32_t b = m_Context->state[1];
    uint32_t c = m_Context->state[2];
    uint32_t d = m_Context->state[3];
    
    Decode(x, block, MD5_BLOCK_LENGTH);
    
    /* Round 1 */
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */
   
    /* Round 2 */
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */
   
    /* Round 3 */
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */
   
    /* Round 4 */
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */
    
    // Update state
    m_Context->state[0] += a;
    m_Context->state[1] += b;
    m_Context->state[2] += c;
    m_Context->state[3] += d;
    
    // Cleanup sensitive data for security
    std::memset(x, 0, MD5_BLOCK_LENGTH);
}

void Hasher<MD5, SOFTWARE>::Update(const uint8_t* const data, const uint64_t size)
{
    if(!data)
        throw std::invalid_argument("Data cannot be nullptr.");
    
    if(size == 0)
        throw std::invalid_argument("Data size cannot be zero.");
    
    uint64_t i;
    uint32_t index = (m_Context->count[0] >> 3) & (MD5_BLOCK_LENGTH - 1); // mod MD5_BLOCK_LENGTH
    const uint32_t partSize = MD5_BLOCK_LENGTH - index;
    
    if ((m_Context->count[0] += (size << 3)) < (size << 3))
        m_Context->count[1]++;
    
    m_Context->count[1] += (size >> 29);
    
    // Transform as often as possible
    if(size >= partSize)
    {
        std::memcpy(&m_Context->buffer[index], data, partSize);
        
        Transform(m_Context->buffer);
        
        for(i = partSize; i + MD5_BLOCK_LENGTH <= size; i += MD5_BLOCK_LENGTH)
            Transform(&data[i]);
        
        index = 0;
    }
    else
    {
        i = 0;
    }
    
    // Remaining input data
    std::memcpy(&m_Context->buffer[index], &data[i], size - i);
}

void Hasher<MD5, SOFTWARE>::Update(const std::vector<uint8_t>& data)
{
    Update(data.data(), data.size());
}

void Hasher<MD5, SOFTWARE>::Update(const std::string& str)
{
    Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

std::vector<uint8_t> Hasher<MD5, SOFTWARE>::End()
{
    // Number of bits
    uint8_t numOfBits[8];
    Encode (numOfBits, m_Context->count, 8);
    
    // Pad out to 56 mod 64
    uint32_t index = (m_Context->count[0] >> 3) & 0x3F;
    uint32_t paddingSize = (index < 56) ? (56 - index) : (120 - index);
    
    Update(PADDING.data(), paddingSize);
    
    // Append size before padding
    Update(numOfBits, 8);
    
    // Get hash from state
    std::vector<uint8_t> hash(16); // 128 bit hash
    Encode(hash.data(), m_Context->state, 16);
    
    // Cleanup sensitive data for security
    std::memset(m_Context, 0, sizeof(Context));
    
    return hash;
}
