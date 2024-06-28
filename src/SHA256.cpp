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
const std::array<uint32_t, 64> K = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const std::array<uint32_t, 8> INITIAL_HASH_VALUES = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

// Macros (TODO: rework)
#define ROTATE_LEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTATE_RIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define EP0(x) (ROTATE_RIGHT(x,2) ^ ROTATE_RIGHT(x,13) ^ ROTATE_RIGHT(x,22))
#define EP1(x) (ROTATE_RIGHT(x,6) ^ ROTATE_RIGHT(x,11) ^ ROTATE_RIGHT(x,25))

#define SIG0(x) (ROTATE_RIGHT(x,7) ^ ROTATE_RIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTATE_RIGHT(x,17) ^ ROTATE_RIGHT(x,19) ^ ((x) >> 10))

void Hasher<SHA256, SOFTWARE>::Transform(const uint8_t* const data)
{
    uint32_t block[SHA256_BLOCK_LENGTH];
    
    // Copy 1-byte data to 4-byte array, convert to big endianness
    uint8_t x = 0;
    for(uint8_t i = 0; i < 16; i++)
    {
        block[i] = (data[x] << 24) | (data[x + 1] << 16) | (data[x + 2] << 8) | (data[x + 3]);
        x += 4;
    }
    
    for(uint8_t i = 16; i < 64; i++)
    {
        block[i] = SIG1(block[i - 2]) + block[i - 7] + SIG0(block[i - 15]) + block[i - 16];
    }
    
    // Initial values
    uint32_t a = m_Context.state[0];
    uint32_t b = m_Context.state[1];
    uint32_t c = m_Context.state[2];
    uint32_t d = m_Context.state[3];
    uint32_t e = m_Context.state[4];
    uint32_t f = m_Context.state[5];
    uint32_t g = m_Context.state[6];
    uint32_t h = m_Context.state[7];
    
    uint32_t t1 = 0;
    uint32_t t2 = 0;
    
    for(uint8_t i = 0; i < 64; i++)
    {
        t1 = h + EP1(e) + CH(e, f, g) + K[i] + block[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    
    m_Context.state[0] += a;
    m_Context.state[1] += b;
    m_Context.state[2] += c;
    m_Context.state[3] += d;
    m_Context.state[4] += e;
    m_Context.state[5] += f;
    m_Context.state[6] += g;
    m_Context.state[7] += h;
}

void Hasher<SHA256, SOFTWARE>::Initialize()
{
    // Set state to initial hash values
    std::copy(INITIAL_HASH_VALUES.begin(), INITIAL_HASH_VALUES.end(), m_Context.state);
    
    m_Context.bufferSize = 0;
    m_Context.numOfBits = 0;
}

void Hasher<SHA256, SOFTWARE>::Update(const uint8_t* const data, const uint64_t size)
{
    if(!data)
        throw std::invalid_argument("Data cannot be nullptr.");
    
    if(size == 0)
        throw std::invalid_argument("Data size cannot be zero.");
    
    // Fill buffer with 64 bytes
    for(uint64_t i = 0; i < size; i++)
    {
        m_Context.buffer[m_Context.bufferSize] = data[i];
        m_Context.bufferSize++;
        
        if(m_Context.bufferSize == SHA256_BLOCK_LENGTH)
        {
            Transform(m_Context.buffer);
            
            m_Context.bufferSize = 0;
            m_Context.numOfBits += 512;
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
    uint8_t bufferSize = m_Context.bufferSize;
    
    // Pad last block
    if(m_Context.bufferSize < 56)
    {
        m_Context.buffer[bufferSize] = 0x80;
        bufferSize++;
        
        for(; bufferSize < 56; bufferSize++)
            m_Context.buffer[bufferSize] = 0x00;
    }
    else
    {
        m_Context.buffer[bufferSize] = 0x80;
        bufferSize++;
        
        for(; bufferSize < 64; bufferSize++)
            m_Context.buffer[bufferSize] = 0x00;
        
        Transform(m_Context.buffer);
        
        std::memset(m_Context.buffer, 0, 56);
    }
    
    // Append message length in bits
    m_Context.numOfBits += m_Context.bufferSize * 8;
    
    m_Context.buffer[63] = m_Context.numOfBits;
    m_Context.buffer[62] = m_Context.numOfBits >> 8;
    m_Context.buffer[61] = m_Context.numOfBits >> 16;
    m_Context.buffer[60] = m_Context.numOfBits >> 24;
    
    m_Context.buffer[59] = m_Context.numOfBits >> 32;
    m_Context.buffer[58] = m_Context.numOfBits >> 40;
    m_Context.buffer[57] = m_Context.numOfBits >> 48;
    m_Context.buffer[56] = m_Context.numOfBits >> 56;
    
    // Transform
    Transform(m_Context.buffer);
    
    std::vector<uint8_t> hash(32); // 256 bit hash
    
    // Transform SHA big endian to host little endian
    for(uint8_t i = 0; i < 4; i++)
    {
        hash[i]      = (m_Context.state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4]  = (m_Context.state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8]  = (m_Context.state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (m_Context.state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (m_Context.state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (m_Context.state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (m_Context.state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (m_Context.state[7] >> (24 - i * 8)) & 0x000000ff;
    }
    
    return hash;
}
