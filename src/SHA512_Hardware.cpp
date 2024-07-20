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
/*  SHA512_Hardware.cpp                                             */
/*  Created: 20.07.2024                                             */
/*------------------------------------------------------------------*/

#include "HashMe.hpp"

#if defined(HM_SIMD_ARM) || defined(HM_SIMD_X86)

using namespace HashMe;

Hasher<SHA512, HARDWARE>::Hasher(const Hasher& other) : Hasher<SHA512, SOFTWARE>(other)
{
}

#if defined(HM_SIMD_ARM)
void Hasher<SHA512, HARDWARE>::TransformARM(const uint8_t* const data)
{
    // Load initial state
    uint64x2_t AB = vld1q_u64(&m_Context->state[0]);
    uint64x2_t CD = vld1q_u64(&m_Context->state[2]);
    uint64x2_t EF = vld1q_u64(&m_Context->state[4]);
    uint64x2_t GH = vld1q_u64(&m_Context->state[6]);
    
    // Save current state
    uint64x2_t AB_SAVE = AB;
    uint64x2_t CD_SAVE = CD;
    uint64x2_t EF_SAVE = EF;
    uint64x2_t GH_SAVE = GH;

    // Load data
    uint64x2_t MSG0 = (uint64x2_t) vld1q_u8(data + 16 * 0);
    uint64x2_t MSG1 = (uint64x2_t) vld1q_u8(data + 16 * 1);
    uint64x2_t MSG2 = (uint64x2_t) vld1q_u8(data + 16 * 2);
    uint64x2_t MSG3 = (uint64x2_t) vld1q_u8(data + 16 * 3);
    uint64x2_t MSG4 = (uint64x2_t) vld1q_u8(data + 16 * 4);
    uint64x2_t MSG5 = (uint64x2_t) vld1q_u8(data + 16 * 5);
    uint64x2_t MSG6 = (uint64x2_t) vld1q_u8(data + 16 * 6);
    uint64x2_t MSG7 = (uint64x2_t) vld1q_u8(data + 16 * 7);

#ifdef HM_LITTLE_ENDIAN // Reverse byte order if host is little endian to match SHA256 big endian requirement
    MSG0 = vreinterpretq_u64_u8(vrev64q_u8(vreinterpretq_u8_u64(MSG0)));
    MSG1 = vreinterpretq_u64_u8(vrev64q_u8(vreinterpretq_u8_u64(MSG1)));
    MSG2 = vreinterpretq_u64_u8(vrev64q_u8(vreinterpretq_u8_u64(MSG2)));
    MSG3 = vreinterpretq_u64_u8(vrev64q_u8(vreinterpretq_u8_u64(MSG3)));
    MSG4 = vreinterpretq_u64_u8(vrev64q_u8(vreinterpretq_u8_u64(MSG4)));
    MSG5 = vreinterpretq_u64_u8(vrev64q_u8(vreinterpretq_u8_u64(MSG5)));
    MSG6 = vreinterpretq_u64_u8(vrev64q_u8(vreinterpretq_u8_u64(MSG6)));
    MSG7 = vreinterpretq_u64_u8(vrev64q_u8(vreinterpretq_u8_u64(MSG7)));
#endif
    
    uint64x2_t TMP0, TMP1, TMP2;
    
    // Rounds 0-1
    TMP0 = vaddq_u64(MSG0, vld1q_u64(&K[0]));
    TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), GH);
    TMP2 = vsha512hq_u64(TMP1, vextq_u64(EF, GH, 1), vextq_u64(CD, EF, 1));
    GH = vsha512h2q_u64(TMP2, CD, AB);
    CD = vaddq_u64(CD, TMP2);

    // Rounds 2-3
    TMP0 = vaddq_u64(MSG1, vld1q_u64(&K[2]));
    TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), EF);
    TMP2 = vsha512hq_u64(TMP1, vextq_u64(CD, EF, 1), vextq_u64(AB, CD, 1));
    EF = vsha512h2q_u64(TMP2, AB, GH);
    AB = vaddq_u64(AB, TMP2);

    // Rounds 4-5
    TMP0 = vaddq_u64(MSG2, vld1q_u64(&K[4]));
    TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), CD);
    TMP2 = vsha512hq_u64(TMP1, vextq_u64(AB, CD, 1), vextq_u64(GH, AB, 1));
    CD = vsha512h2q_u64(TMP2, GH, EF);
    GH = vaddq_u64(GH, TMP2);

    // Rounds 6-7
    TMP0 = vaddq_u64(MSG3, vld1q_u64(&K[6]));
    TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), AB);
    TMP2 = vsha512hq_u64(TMP1, vextq_u64(GH, AB, 1), vextq_u64(EF, GH, 1));
    AB = vsha512h2q_u64(TMP2, EF, CD);
    EF = vaddq_u64(EF, TMP2);

    // Rounds 8-9
    TMP0 = vaddq_u64(MSG4, vld1q_u64(&K[8]));
    TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), GH);
    TMP2 = vsha512hq_u64(TMP1, vextq_u64(EF, GH, 1), vextq_u64(CD, EF, 1));
    GH = vsha512h2q_u64(TMP2, CD, AB);
    CD = vaddq_u64(CD, TMP2);

    // Rounds 10-11
    TMP0 = vaddq_u64(MSG5, vld1q_u64(&K[10]));
    TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), EF);
    TMP2 = vsha512hq_u64(TMP1, vextq_u64(CD, EF, 1), vextq_u64(AB, CD, 1));
    EF = vsha512h2q_u64(TMP2, AB, GH);
    AB = vaddq_u64(AB, TMP2);

    // Rounds 12-13
    TMP0 = vaddq_u64(MSG6, vld1q_u64(&K[12]));
    TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), CD);
    TMP2 = vsha512hq_u64(TMP1, vextq_u64(AB, CD, 1), vextq_u64(GH, AB, 1));
    CD = vsha512h2q_u64(TMP2, GH, EF);
    GH = vaddq_u64(GH, TMP2);

    // Rounds 14-15
    TMP0 = vaddq_u64(MSG7, vld1q_u64(&K[14]));
    TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), AB);
    TMP2 = vsha512hq_u64(TMP1, vextq_u64(GH, AB, 1), vextq_u64(EF, GH, 1));
    AB = vsha512h2q_u64(TMP2, EF, CD);
    EF = vaddq_u64(EF, TMP2);

    for (uint8_t t = 16; t < 80; t += 16)
    {
        /* Rounds t and t + 1 */
        MSG0 = vsha512su1q_u64(vsha512su0q_u64(MSG0, MSG1), MSG7, vextq_u64(MSG4, MSG5, 1));
        TMP0 = vaddq_u64(MSG0, vld1q_u64(&K[t]));
        TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), GH);
        TMP2 = vsha512hq_u64(TMP1, vextq_u64(EF, GH, 1), vextq_u64(CD, EF, 1));
        GH = vsha512h2q_u64(TMP2, CD, AB);
        CD = vaddq_u64(CD, TMP2);

        /* Rounds t + 2 and t + 3 */
        MSG1 = vsha512su1q_u64(vsha512su0q_u64(MSG1, MSG2), MSG0, vextq_u64(MSG5, MSG6, 1));
        TMP0 = vaddq_u64(MSG1, vld1q_u64(&K[t + 2]));
        TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), EF);
        TMP2 = vsha512hq_u64(TMP1, vextq_u64(CD, EF, 1), vextq_u64(AB, CD, 1));
        EF = vsha512h2q_u64(TMP2, AB, GH);
        AB = vaddq_u64(AB, TMP2);

        /* Rounds t + 4 and t + 5 */
        MSG2 = vsha512su1q_u64(vsha512su0q_u64(MSG2, MSG3), MSG1, vextq_u64(MSG6, MSG7, 1));
        TMP0 = vaddq_u64(MSG2, vld1q_u64(&K[t + 4]));
        TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), CD);
        TMP2 = vsha512hq_u64(TMP1, vextq_u64(AB, CD, 1), vextq_u64(GH, AB, 1));
        CD = vsha512h2q_u64(TMP2, GH, EF);
        GH = vaddq_u64(GH, TMP2);

        /* Rounds t + 6 and t + 7 */
        MSG3 = vsha512su1q_u64(vsha512su0q_u64(MSG3, MSG4), MSG2, vextq_u64(MSG7, MSG0, 1));
        TMP0 = vaddq_u64(MSG3, vld1q_u64(&K[t + 6]));
        TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), AB);
        TMP2 = vsha512hq_u64(TMP1, vextq_u64(GH, AB, 1), vextq_u64(EF, GH, 1));
        AB = vsha512h2q_u64(TMP2, EF, CD);
        EF = vaddq_u64(EF, TMP2);

        /* Rounds t + 8 and t + 9 */
        MSG4 = vsha512su1q_u64(vsha512su0q_u64(MSG4, MSG5), MSG3, vextq_u64(MSG0, MSG1, 1));
        TMP0 = vaddq_u64(MSG4, vld1q_u64(&K[t + 8]));
        TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), GH);
        TMP2 = vsha512hq_u64(TMP1, vextq_u64(EF, GH, 1), vextq_u64(CD, EF, 1));
        GH = vsha512h2q_u64(TMP2, CD, AB);
        CD = vaddq_u64(CD, TMP2);

        /* Rounds t + 10 and t + 11 */
        MSG5 = vsha512su1q_u64(vsha512su0q_u64(MSG5, MSG6), MSG4, vextq_u64(MSG1, MSG2, 1));
        TMP0 = vaddq_u64(MSG5, vld1q_u64(&K[t + 10]));
        TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), EF);
        TMP2 = vsha512hq_u64(TMP1, vextq_u64(CD, EF, 1), vextq_u64(AB, CD, 1));
        EF = vsha512h2q_u64(TMP2, AB, GH);
        AB = vaddq_u64(AB, TMP2);

        /* Rounds t + 12 and t + 13 */
        MSG6 = vsha512su1q_u64(vsha512su0q_u64(MSG6, MSG7), MSG5, vextq_u64(MSG2, MSG3, 1));
        TMP0 = vaddq_u64(MSG6, vld1q_u64(&K[t + 12]));
        TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), CD);
        TMP2 = vsha512hq_u64(TMP1, vextq_u64(AB, CD, 1), vextq_u64(GH, AB, 1));
        CD = vsha512h2q_u64(TMP2, GH, EF);
        GH = vaddq_u64(GH, TMP2);

        /* Rounds t + 14 and t + 15 */
        MSG7 = vsha512su1q_u64(vsha512su0q_u64(MSG7, MSG0), MSG6, vextq_u64(MSG3, MSG4, 1));
        TMP0 = vaddq_u64(MSG7, vld1q_u64(&K[t + 14]));
        TMP1 = vaddq_u64(vextq_u64(TMP0, TMP0, 1), AB);
        TMP2 = vsha512hq_u64(TMP1, vextq_u64(GH, AB, 1), vextq_u64(EF, GH, 1));
        AB = vsha512h2q_u64(TMP2, EF, CD);
        EF = vaddq_u64(EF, TMP2);
    }
    
    // Combine states
    AB = vaddq_u64(AB, AB_SAVE);
    CD = vaddq_u64(CD, CD_SAVE);
    EF = vaddq_u64(EF, EF_SAVE);
    GH = vaddq_u64(GH, GH_SAVE);
    
    // Save the new state
    vst1q_u64(&m_Context->state[0], AB);
    vst1q_u64(&m_Context->state[2], CD);
    vst1q_u64(&m_Context->state[4], EF);
    vst1q_u64(&m_Context->state[6], GH);
}
#elif defined(HM_SIMD_X86)
void Hasher<SHA512, HARDWARE>::TransformX86(const uint8_t* const data)
{
    static_assert(0, "Not implemented");
}
#endif

void Hasher<SHA512, HARDWARE>::Transform(const uint8_t* const data)
{
#if defined(HM_SIMD_ARM)
    TransformARM(data);
#elif defined(HM_SIMD_X86)
    TransformX86(data);
#endif
}

void Hasher<SHA512, HARDWARE>::Update(const uint8_t* const data, const uint64_t size)
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

void Hasher<SHA512, HARDWARE>::Update(const std::vector<uint8_t>& data)
{
    Update(data.data(), data.size());
}

void Hasher<SHA512, HARDWARE>::Update(const std::string& str)
{
    Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

std::vector<uint8_t> Hasher<SHA512, HARDWARE>::End()
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

#endif /* HM_SIMD_ARM || HM_SIMD_X86 */
