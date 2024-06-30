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
/*  SHA256_Hardware.cpp                                             */
/*  Created: 27.06.2024                                             */
/*------------------------------------------------------------------*/

#include "HashMe.hpp"

#ifdef HM_SIMD_ARM

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

Hasher<SHA256, HARDWARE>::Hasher()
{
    Initialize();
}

Hasher<SHA256, HARDWARE>::Hasher(const Hasher& other)
{
    std::copy(other.m_State, other.m_State + 8, m_State);
}

void Hasher<SHA256, HARDWARE>::Initialize()
{
    // Set state to initial hash values
    std::copy(INITIAL_HASH_VALUES.begin(), INITIAL_HASH_VALUES.end(), m_State);
}

void Hasher<SHA256, HARDWARE>::Reset()
{
    Initialize();
}

// Prepare data with padding
uint64_t Hasher<SHA256, HARDWARE>::PrepareData(const uint8_t* const data, const uint64_t size, uint8_t** const preparedData)
{
    const uint64_t dataSizeInBits = size << 3; // size * 8
    const uint64_t paddingSize = (512ULL - ((dataSizeInBits + 1ULL + 64ULL) & 0x1FF)) >> 3; // (& 0x1FF) = (mod 512)
    const uint64_t preparedDataSize = (1 + paddingSize + 8) + size; // paddingSize + size
    
    // Allocate memory for data and padding
    *preparedData = new uint8_t[preparedDataSize]{0}; // Zero fill
    
    // Copy data to new array
    std::copy(data, data + size, *preparedData);
    
    // Append padding after data. Add a 1 at beginning and append n zero bits
    (*preparedData)[size] = 0x80;
    
    // Append data size (uint64_t as big endian) to the end of array
    const uint64_t index = 1ULL + paddingSize + size;
    (*preparedData)[index + 7] = dataSizeInBits;
    (*preparedData)[index + 6] = dataSizeInBits >> 8;
    (*preparedData)[index + 5] = dataSizeInBits >> 16;
    (*preparedData)[index + 4] = dataSizeInBits >> 24;
    (*preparedData)[index + 3] = dataSizeInBits >> 32;
    (*preparedData)[index + 2] = dataSizeInBits >> 40;
    (*preparedData)[index + 1] = dataSizeInBits >> 48;
    (*preparedData)[index] = dataSizeInBits >> 56;
    
    return preparedDataSize;
}

void Hasher<SHA256, HARDWARE>::ProcessARM(const uint8_t* preparedData, uint64_t size)
{
    uint32x4_t STATE0, STATE1, ABCD_SAVE, EFGH_SAVE;
    uint32x4_t MSG0, MSG1, MSG2, MSG3;
    uint32x4_t TMP0, TMP1, TMP2;
    
    // Load initial state
    STATE0 = vld1q_u32(&m_State[0]);
    STATE1 = vld1q_u32(&m_State[4]);
    
    while (size >= 64)
    {
        // Save current state
        ABCD_SAVE = STATE0;
        EFGH_SAVE = STATE1;

        // Load data
        MSG0 = vld1q_u32(reinterpret_cast<const uint32_t*>(preparedData +  0));
        MSG1 = vld1q_u32(reinterpret_cast<const uint32_t*>(preparedData + 16));
        MSG2 = vld1q_u32(reinterpret_cast<const uint32_t*>(preparedData + 32));
        MSG3 = vld1q_u32(reinterpret_cast<const uint32_t*>(preparedData + 48));

#ifdef HM_LITTLE_ENDIAN // Reverse byte order if host is little endian to match SHA256 big endian
        MSG0 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG0)));
        MSG1 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG1)));
        MSG2 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG2)));
        MSG3 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG3)));
#endif

        TMP0 = vaddq_u32(MSG0, vld1q_u32(&K[0x00]));

        // Rounds 0-3
        MSG0 = vsha256su0q_u32(MSG0, MSG1);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&K[0x04]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

        // Rounds 4-7
        MSG1 = vsha256su0q_u32(MSG1, MSG2);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&K[0x08]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

        // Rounds 8-11
        MSG2 = vsha256su0q_u32(MSG2, MSG3);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&K[0x0c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

        // Rounds 12-15
        MSG3 = vsha256su0q_u32(MSG3, MSG0);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG0, vld1q_u32(&K[0x10]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

        // Rounds 16-19
        MSG0 = vsha256su0q_u32(MSG0, MSG1);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&K[0x14]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

        // Rounds 20-23
        MSG1 = vsha256su0q_u32(MSG1, MSG2);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&K[0x18]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

        // Rounds 24-27
        MSG2 = vsha256su0q_u32(MSG2, MSG3);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&K[0x1c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

        // Rounds 28-31
        MSG3 = vsha256su0q_u32(MSG3, MSG0);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG0, vld1q_u32(&K[0x20]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

        // Rounds 32-35
        MSG0 = vsha256su0q_u32(MSG0, MSG1);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&K[0x24]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

        // Rounds 36-39
        MSG1 = vsha256su0q_u32(MSG1, MSG2);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&K[0x28]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

        // Rounds 40-43
        MSG2 = vsha256su0q_u32(MSG2, MSG3);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&K[0x2c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

        // Rounds 44-47
        MSG3 = vsha256su0q_u32(MSG3, MSG0);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG0, vld1q_u32(&K[0x30]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

        // Rounds 48-51
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&K[0x34]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);

        // Rounds 52-55
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&K[0x38]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);

        // Rounds 56-59
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&K[0x3c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);

        // Rounds 60-63
        TMP2 = STATE0;
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);

        // Combine states
        STATE0 = vaddq_u32(STATE0, ABCD_SAVE);
        STATE1 = vaddq_u32(STATE1, EFGH_SAVE);

        preparedData += 64;
        size -= 64;
    }
    
    // Save the new state
    vst1q_u32(&m_State[0], STATE0);
    vst1q_u32(&m_State[4], STATE1);
}

void Hasher<SHA256, HARDWARE>::Update(const uint8_t* const data, const uint64_t size)
{
    if(!data)
        throw std::invalid_argument("Data cannot be nullptr.");
    
    if(size == 0)
        throw std::invalid_argument("Data size cannot be zero.");
    
    // Prepare data
    uint8_t* preparedData = nullptr; // Will be allocated inside PrepareData
    const uint64_t preparedDataSize = PrepareData(data, size, &preparedData);
    
    // Process preparedData
    ProcessARM(preparedData, preparedDataSize);
    
    delete[] preparedData;
}

void Hasher<SHA256, HARDWARE>::Update(const std::vector<uint8_t>& data)
{
    Update(data.data(), data.size());
}

void Hasher<SHA256, HARDWARE>::Update(const std::string& str)
{
    Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

std::vector<uint8_t> Hasher<SHA256, HARDWARE>::End()
{
    std::vector<uint8_t> hash(32); // 256 bit hash
    uint8_t* pointerIndex;
    
    for(uint8_t i = 0; i < 8; i++)
    {
#ifdef HM_LITTLE_ENDIAN // Swap byte order if host is little endian
    #ifdef HASH_PREDEF_COMP_MSVC_AVAILABLE // MSVC compiler
        m_State[i] = _byteswap_ulong(m_State[i]);
    #else
        m_State[i] = __builtin_bswap32(m_State[i]);
    #endif
#endif /* HM_LITTLE_ENDIAN */
        
        pointerIndex = reinterpret_cast<uint8_t*>(&m_State[i]);
        
        hash[i * 4] = *pointerIndex++;
        hash[i * 4 + 1] = *pointerIndex++;
        hash[i * 4 + 2] = *pointerIndex++;
        hash[i * 4 + 3] = *pointerIndex;
    }
    
    return hash;
}

#endif /* HM_SIMD_ARM */
