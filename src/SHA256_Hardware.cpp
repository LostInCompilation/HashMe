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

#if defined(HM_SIMD_ARM) || defined(HM_SIMD_X86)

using namespace HashMe;

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
    const uint64_t preparedDataSize = (1ULL + paddingSize + 8ULL) + size; // paddingSize + size
    
    // Allocate memory for data and padding
    *preparedData = new uint8_t[preparedDataSize]{0}; // Zero fill
    
    // Copy data to new array
    std::copy(data, data + size, *preparedData);
    
    // Append padding after data. Add a 1 at beginning and append n zero bits
    (*preparedData)[size] = 0x80;
    
    // Append data size (uint64_t as big endian) to the end of array
    const uint64_t index = 1ULL + paddingSize + size;
    (*preparedData)[index + 7] = static_cast<uint8_t>(dataSizeInBits); // Unsigned integer cast discards unused bits
    (*preparedData)[index + 6] = static_cast<uint8_t>(dataSizeInBits >> 8);
    (*preparedData)[index + 5] = static_cast<uint8_t>(dataSizeInBits >> 16);
    (*preparedData)[index + 4] = static_cast<uint8_t>(dataSizeInBits >> 24);
    (*preparedData)[index + 3] = static_cast<uint8_t>(dataSizeInBits >> 32);
    (*preparedData)[index + 2] = static_cast<uint8_t>(dataSizeInBits >> 40);
    (*preparedData)[index + 1] = static_cast<uint8_t>(dataSizeInBits >> 48);
    (*preparedData)[index] = static_cast<uint8_t>(dataSizeInBits >> 56);
    
    return preparedDataSize;
}

#ifdef HM_SIMD_ARM
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

#ifdef HM_LITTLE_ENDIAN // Reverse byte order if host is little endian to match SHA256 big endian requirement
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
#elif defined(HM_SIMD_X86)
void Hasher<SHA256, HARDWARE>::ProcessX86(const uint8_t* preparedData, uint64_t size)
{
    __m128i STATE0, STATE1;
    __m128i MSG, TMP;
    __m128i MSG0, MSG1, MSG2, MSG3;
    __m128i ABEF_SAVE, CDGH_SAVE;
    const __m128i MASK = _mm_set_epi64x(0x0c0d0e0f08090a0bULL, 0x0405060700010203ULL);

    // Load initial values from state
    TMP = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&m_State[0]));
    STATE1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&m_State[4]));

    TMP = _mm_shuffle_epi32(TMP, 0xB1);          // CDAB
    STATE1 = _mm_shuffle_epi32(STATE1, 0x1B);    // EFGH
    STATE0 = _mm_alignr_epi8(TMP, STATE1, 8);    // ABEF
    STATE1 = _mm_blend_epi16(STATE1, TMP, 0xF0); // CDGH

    while (size >= 64)
    {
        // Save current state
        ABEF_SAVE = STATE0;
        CDGH_SAVE = STATE1;

        // Load data
        MSG = _mm_loadu_si128(reinterpret_cast<const __m128i*>(preparedData + 0));
        MSG1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(preparedData + 16));
        MSG2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(preparedData + 32));
        MSG3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(preparedData + 48));
        
        // Rounds 0-3
        //MSG = _mm_loadu_si128((const __m128i*) (data+0));
        MSG0 = _mm_shuffle_epi8(MSG, MASK);
        MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0xE9B5DBA5B5C0FBCFULL, 0x71374491428A2F98ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        // Rounds 4-7
        //MSG1 = _mm_loadu_si128((const __m128i*) (data+16));
        MSG1 = _mm_shuffle_epi8(MSG1, MASK);
        MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0xAB1C5ED5923F82A4ULL, 0x59F111F13956C25BULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

        // Rounds 8-11
        //MSG2 = _mm_loadu_si128((const __m128i*) (data+32));
        MSG2 = _mm_shuffle_epi8(MSG2, MASK);
        MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0x550C7DC3243185BEULL, 0x12835B01D807AA98ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

        // Rounds 12-15
        //MSG3 = _mm_loadu_si128((const __m128i*) (data+48));
        MSG3 = _mm_shuffle_epi8(MSG3, MASK);
        MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0xC19BF1749BDC06A7ULL, 0x80DEB1FE72BE5D74ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
        MSG0 = _mm_add_epi32(MSG0, TMP);
        MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

        // Rounds 16-19
        MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0x240CA1CC0FC19DC6ULL, 0xEFBE4786E49B69C1ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

        // Rounds 20-23
        MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
        MSG2 = _mm_add_epi32(MSG2, TMP);
        MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

        // Rounds 24-27
        MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0xBF597FC7B00327C8ULL, 0xA831C66D983E5152ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
        MSG3 = _mm_add_epi32(MSG3, TMP);
        MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

        // Rounds 28-31
        MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0x1429296706CA6351ULL,  0xD5A79147C6E00BF3ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
        MSG0 = _mm_add_epi32(MSG0, TMP);
        MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

        // Rounds 32-35
        MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0x53380D134D2C6DFCULL, 0x2E1B213827B70A85ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

        // Rounds 36-39
        MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x92722C8581C2C92EULL, 0x766A0ABB650A7354ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
        MSG2 = _mm_add_epi32(MSG2, TMP);
        MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

        // Rounds 40-43
        MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0xC76C51A3C24B8B70ULL, 0xA81A664BA2BFE8A1ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
        MSG3 = _mm_add_epi32(MSG3, TMP);
        MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

        // Rounds 44-47
        MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0x106AA070F40E3585ULL, 0xD6990624D192E819ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
        MSG0 = _mm_add_epi32(MSG0, TMP);
        MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

        // Rounds 48-51
        MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0x34B0BCB52748774CULL, 0x1E376C0819A4C116ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

        // Rounds 52-55
        MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x682E6FF35B9CCA4FULL, 0x4ED8AA4A391C0CB3ULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
        MSG2 = _mm_add_epi32(MSG2, TMP);
        MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        // Rounds 56-59
        MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0x8CC7020884C87814ULL, 0x78A5636F748F82EEULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
        MSG3 = _mm_add_epi32(MSG3, TMP);
        MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        // Rounds 60-63
        MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0xC67178F2BEF9A3F7ULL, 0xA4506CEB90BEFFFAULL));
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        // Combine states
        STATE0 = _mm_add_epi32(STATE0, ABEF_SAVE);
        STATE1 = _mm_add_epi32(STATE1, CDGH_SAVE);

        preparedData += 64;
        size -= 64;
    }

    TMP = _mm_shuffle_epi32(STATE0, 0x1B);       // FEBA
    STATE1 = _mm_shuffle_epi32(STATE1, 0xB1);    // DCHG
    STATE0 = _mm_blend_epi16(TMP, STATE1, 0xF0); // DCBA
    STATE1 = _mm_alignr_epi8(STATE1, TMP, 8);    // ABEF

    // Save the new state
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&m_State[0]), STATE0);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&m_State[4]), STATE1);
}
#endif

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
#if defined(HM_SIMD_ARM)
    ProcessARM(preparedData, preparedDataSize);
#elif defined(HM_SIMD_X86)
    ProcessX86(preparedData, preparedDataSize);
#endif
    
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
    
    // Assemble hash
    for(uint32_t i = 0; i < 8; i++) // Use uint32_t to avoid implicit sign conversion by left shift operator
    {        
#ifdef HM_LITTLE_ENDIAN
        Utils::U32toU8<Utils::REVERSE_ENDIANNESS>(m_State[i], &hash[i << 2]); // Transform SHA big endian to host little endian
#else
        Utils::U32toU8<Utils::KEEP_ENDIANNESS>(m_State[i], &hash[i << 2]);
#endif
    }
    
    return hash;
}

#endif /* HM_SIMD_ARM || HM_SIMD_X86 */
