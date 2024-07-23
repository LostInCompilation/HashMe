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
    uint64x2_t STATE0 = vld1q_u64(&m_Context->state[0]); // AB
    uint64x2_t STATE1 = vld1q_u64(&m_Context->state[2]); // CD
    uint64x2_t STATE2 = vld1q_u64(&m_Context->state[4]); // EF
    uint64x2_t STATE3 = vld1q_u64(&m_Context->state[6]); // GH
    
    // Save current state
    const uint64x2_t AB_SAVE = STATE0;
    const uint64x2_t CD_SAVE = STATE1;
    const uint64x2_t EF_SAVE = STATE2;
    const uint64x2_t GH_SAVE = STATE3;
    
    // Load data
    const uint64_t* const data64 = reinterpret_cast<const uint64_t*>(data);
    uint64x2_t MSG0 = vld1q_u64(data64 + 0);
    uint64x2_t MSG1 = vld1q_u64(data64 + 2);
    uint64x2_t MSG2 = vld1q_u64(data64 + 4);
    uint64x2_t MSG3 = vld1q_u64(data64 + 6);
    uint64x2_t MSG4 = vld1q_u64(data64 + 8);
    uint64x2_t MSG5 = vld1q_u64(data64 + 10);
    uint64x2_t MSG6 = vld1q_u64(data64 + 12);
    uint64x2_t MSG7 = vld1q_u64(data64 + 14);
    
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
    
    uint64x2_t MSG_K, TMP0, TMP1;

    // Rounds 0-1
    MSG_K = vaddq_u64(MSG0, vld1q_u64(&K[0]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE3);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE2, STATE3, 1), vextq_u64(STATE1, STATE2, 1));
    STATE3 = vsha512h2q_u64(TMP1, STATE1, STATE0);
    STATE1 = vaddq_u64(STATE1, TMP1);
    MSG0 = vsha512su1q_u64(vsha512su0q_u64(MSG0, MSG1), MSG7, vextq_u64(MSG4, MSG5, 1));

    // Rounds 2-3
    MSG_K = vaddq_u64(MSG1, vld1q_u64(&K[2 * 1]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE2);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE1, STATE2, 1), vextq_u64(STATE0, STATE1, 1));
    STATE2 = vsha512h2q_u64(TMP1, STATE0, STATE3);
    STATE0 = vaddq_u64(STATE0, TMP1);
    MSG1 = vsha512su1q_u64(vsha512su0q_u64(MSG1, MSG2), MSG0, vextq_u64(MSG5, MSG6, 1));

    // Rounds 4-5
    MSG_K = vaddq_u64(MSG2, vld1q_u64(&K[2 * 2]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE1);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE0, STATE1, 1), vextq_u64(STATE3, STATE0, 1));
    STATE1 = vsha512h2q_u64(TMP1, STATE3, STATE2);
    STATE3 = vaddq_u64(STATE3, TMP1);
    MSG2 = vsha512su1q_u64(vsha512su0q_u64(MSG2, MSG3), MSG1, vextq_u64(MSG6, MSG7, 1));

    // Rounds 6-7
    MSG_K = vaddq_u64(MSG3, vld1q_u64(&K[2 * 3]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE0);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE3, STATE0, 1), vextq_u64(STATE2, STATE3, 1));
    STATE0 = vsha512h2q_u64(TMP1, STATE2, STATE1);
    STATE2 = vaddq_u64(STATE2, TMP1);
    MSG3 = vsha512su1q_u64(vsha512su0q_u64(MSG3, MSG4), MSG2, vextq_u64(MSG7, MSG0, 1));

    // Rounds 8-9
    MSG_K = vaddq_u64(MSG4, vld1q_u64(&K[2 * 4]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE3);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE2, STATE3, 1), vextq_u64(STATE1, STATE2, 1));
    STATE3 = vsha512h2q_u64(TMP1, STATE1, STATE0);
    STATE1 = vaddq_u64(STATE1, TMP1);
    MSG4 = vsha512su1q_u64(vsha512su0q_u64(MSG4, MSG5), MSG3, vextq_u64(MSG0, MSG1, 1));

    // Rounds 10-11
    MSG_K = vaddq_u64(MSG5, vld1q_u64(&K[2 * 5]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE2);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE1, STATE2, 1), vextq_u64(STATE0, STATE1, 1));
    STATE2 = vsha512h2q_u64(TMP1, STATE0, STATE3);
    STATE0 = vaddq_u64(STATE0, TMP1);
    MSG5 = vsha512su1q_u64(vsha512su0q_u64(MSG5, MSG6), MSG4, vextq_u64(MSG1, MSG2, 1));

    // Rounds 12-13
    MSG_K = vaddq_u64(MSG6, vld1q_u64(&K[2 * 6]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE1);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE0, STATE1, 1), vextq_u64(STATE3, STATE0, 1));
    STATE1 = vsha512h2q_u64(TMP1, STATE3, STATE2);
    STATE3 = vaddq_u64(STATE3, TMP1);
    MSG6 = vsha512su1q_u64(vsha512su0q_u64(MSG6, MSG7), MSG5, vextq_u64(MSG2, MSG3, 1));

    // Rounds 14-15
    MSG_K = vaddq_u64(MSG7, vld1q_u64(&K[2 * 7]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE0);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE3, STATE0, 1), vextq_u64(STATE2, STATE3, 1));
    STATE0 = vsha512h2q_u64(TMP1, STATE2, STATE1);
    STATE2 = vaddq_u64(STATE2, TMP1);
    MSG7 = vsha512su1q_u64(vsha512su0q_u64(MSG7, MSG0), MSG6, vextq_u64(MSG3, MSG4, 1));

    // Rounds 16-17
    MSG_K = vaddq_u64(MSG0, vld1q_u64(&K[2 * 8]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE3);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE2, STATE3, 1), vextq_u64(STATE1, STATE2, 1));
    STATE3 = vsha512h2q_u64(TMP1, STATE1, STATE0);
    STATE1 = vaddq_u64(STATE1, TMP1);
    MSG0 = vsha512su1q_u64(vsha512su0q_u64(MSG0, MSG1), MSG7, vextq_u64(MSG4, MSG5, 1));

    // Rounds 18-19
    MSG_K = vaddq_u64(MSG1, vld1q_u64(&K[2 * 9]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE2);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE1, STATE2, 1), vextq_u64(STATE0, STATE1, 1));
    STATE2 = vsha512h2q_u64(TMP1, STATE0, STATE3);
    STATE0 = vaddq_u64(STATE0, TMP1);
    MSG1 = vsha512su1q_u64(vsha512su0q_u64(MSG1, MSG2), MSG0, vextq_u64(MSG5, MSG6, 1));

    // Rounds 20-21
    MSG_K = vaddq_u64(MSG2, vld1q_u64(&K[2 * 10]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE1);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE0, STATE1, 1), vextq_u64(STATE3, STATE0, 1));
    STATE1 = vsha512h2q_u64(TMP1, STATE3, STATE2);
    STATE3 = vaddq_u64(STATE3, TMP1);
    MSG2 = vsha512su1q_u64(vsha512su0q_u64(MSG2, MSG3), MSG1, vextq_u64(MSG6, MSG7, 1));

    // Rounds 22-23
    MSG_K = vaddq_u64(MSG3, vld1q_u64(&K[2 * 11]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE0);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE3, STATE0, 1), vextq_u64(STATE2, STATE3, 1));
    STATE0 = vsha512h2q_u64(TMP1, STATE2, STATE1);
    STATE2 = vaddq_u64(STATE2, TMP1);
    MSG3 = vsha512su1q_u64(vsha512su0q_u64(MSG3, MSG4), MSG2, vextq_u64(MSG7, MSG0, 1));

    // Rounds 24-25
    MSG_K = vaddq_u64(MSG4, vld1q_u64(&K[2 * 12]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE3);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE2, STATE3, 1), vextq_u64(STATE1, STATE2, 1));
    STATE3 = vsha512h2q_u64(TMP1, STATE1, STATE0);
    STATE1 = vaddq_u64(STATE1, TMP1);
    MSG4 = vsha512su1q_u64(vsha512su0q_u64(MSG4, MSG5), MSG3, vextq_u64(MSG0, MSG1, 1));

    // Rounds 26-27
    MSG_K = vaddq_u64(MSG5, vld1q_u64(&K[2 * 13]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE2);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE1, STATE2, 1), vextq_u64(STATE0, STATE1, 1));
    STATE2 = vsha512h2q_u64(TMP1, STATE0, STATE3);
    STATE0 = vaddq_u64(STATE0, TMP1);
    MSG5 = vsha512su1q_u64(vsha512su0q_u64(MSG5, MSG6), MSG4, vextq_u64(MSG1, MSG2, 1));

    // Rounds 28-29
    MSG_K = vaddq_u64(MSG6, vld1q_u64(&K[2 * 14]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE1);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE0, STATE1, 1), vextq_u64(STATE3, STATE0, 1));
    STATE1 = vsha512h2q_u64(TMP1, STATE3, STATE2);
    STATE3 = vaddq_u64(STATE3, TMP1);
    MSG6 = vsha512su1q_u64(vsha512su0q_u64(MSG6, MSG7), MSG5, vextq_u64(MSG2, MSG3, 1));

    // Rounds 30-31
    MSG_K = vaddq_u64(MSG7, vld1q_u64(&K[2 * 15]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE0);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE3, STATE0, 1), vextq_u64(STATE2, STATE3, 1));
    STATE0 = vsha512h2q_u64(TMP1, STATE2, STATE1);
    STATE2 = vaddq_u64(STATE2, TMP1);
    MSG7 = vsha512su1q_u64(vsha512su0q_u64(MSG7, MSG0), MSG6, vextq_u64(MSG3, MSG4, 1));

    // Rounds 32-33
    MSG_K = vaddq_u64(MSG0, vld1q_u64(&K[2 * 16]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE3);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE2, STATE3, 1), vextq_u64(STATE1, STATE2, 1));
    STATE3 = vsha512h2q_u64(TMP1, STATE1, STATE0);
    STATE1 = vaddq_u64(STATE1, TMP1);
    MSG0 = vsha512su1q_u64(vsha512su0q_u64(MSG0, MSG1), MSG7, vextq_u64(MSG4, MSG5, 1));

    // Rounds 34-35
    MSG_K = vaddq_u64(MSG1, vld1q_u64(&K[2 * 17]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE2);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE1, STATE2, 1), vextq_u64(STATE0, STATE1, 1));
    STATE2 = vsha512h2q_u64(TMP1, STATE0, STATE3);
    STATE0 = vaddq_u64(STATE0, TMP1);
    MSG1 = vsha512su1q_u64(vsha512su0q_u64(MSG1, MSG2), MSG0, vextq_u64(MSG5, MSG6, 1));

    // Rounds 36-37
    MSG_K = vaddq_u64(MSG2, vld1q_u64(&K[2 * 18]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE1);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE0, STATE1, 1), vextq_u64(STATE3, STATE0, 1));
    STATE1 = vsha512h2q_u64(TMP1, STATE3, STATE2);
    STATE3 = vaddq_u64(STATE3, TMP1);
    MSG2 = vsha512su1q_u64(vsha512su0q_u64(MSG2, MSG3), MSG1, vextq_u64(MSG6, MSG7, 1));

    // Rounds 38-39
    MSG_K = vaddq_u64(MSG3, vld1q_u64(&K[2 * 19]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE0);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE3, STATE0, 1), vextq_u64(STATE2, STATE3, 1));
    STATE0 = vsha512h2q_u64(TMP1, STATE2, STATE1);
    STATE2 = vaddq_u64(STATE2, TMP1);
    MSG3 = vsha512su1q_u64(vsha512su0q_u64(MSG3, MSG4), MSG2, vextq_u64(MSG7, MSG0, 1));

    // Rounds 40-41
    MSG_K = vaddq_u64(MSG4, vld1q_u64(&K[2 * 20]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE3);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE2, STATE3, 1), vextq_u64(STATE1, STATE2, 1));
    STATE3 = vsha512h2q_u64(TMP1, STATE1, STATE0);
    STATE1 = vaddq_u64(STATE1, TMP1);
    MSG4 = vsha512su1q_u64(vsha512su0q_u64(MSG4, MSG5), MSG3, vextq_u64(MSG0, MSG1, 1));

    // Rounds 42-43
    MSG_K = vaddq_u64(MSG5, vld1q_u64(&K[2 * 21]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE2);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE1, STATE2, 1), vextq_u64(STATE0, STATE1, 1));
    STATE2 = vsha512h2q_u64(TMP1, STATE0, STATE3);
    STATE0 = vaddq_u64(STATE0, TMP1);
    MSG5 = vsha512su1q_u64(vsha512su0q_u64(MSG5, MSG6), MSG4, vextq_u64(MSG1, MSG2, 1));

    // Rounds 44-45
    MSG_K = vaddq_u64(MSG6, vld1q_u64(&K[2 * 22]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE1);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE0, STATE1, 1), vextq_u64(STATE3, STATE0, 1));
    STATE1 = vsha512h2q_u64(TMP1, STATE3, STATE2);
    STATE3 = vaddq_u64(STATE3, TMP1);
    MSG6 = vsha512su1q_u64(vsha512su0q_u64(MSG6, MSG7), MSG5, vextq_u64(MSG2, MSG3, 1));

    // Rounds 46-47
    MSG_K = vaddq_u64(MSG7, vld1q_u64(&K[2 * 23]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE0);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE3, STATE0, 1), vextq_u64(STATE2, STATE3, 1));
    STATE0 = vsha512h2q_u64(TMP1, STATE2, STATE1);
    STATE2 = vaddq_u64(STATE2, TMP1);
    MSG7 = vsha512su1q_u64(vsha512su0q_u64(MSG7, MSG0), MSG6, vextq_u64(MSG3, MSG4, 1));

    // Rounds 48-49
    MSG_K = vaddq_u64(MSG0, vld1q_u64(&K[2 * 24]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE3);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE2, STATE3, 1), vextq_u64(STATE1, STATE2, 1));
    STATE3 = vsha512h2q_u64(TMP1, STATE1, STATE0);
    STATE1 = vaddq_u64(STATE1, TMP1);
    MSG0 = vsha512su1q_u64(vsha512su0q_u64(MSG0, MSG1), MSG7, vextq_u64(MSG4, MSG5, 1));

    // Rounds 50-51
    MSG_K = vaddq_u64(MSG1, vld1q_u64(&K[2 * 25]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE2);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE1, STATE2, 1), vextq_u64(STATE0, STATE1, 1));
    STATE2 = vsha512h2q_u64(TMP1, STATE0, STATE3);
    STATE0 = vaddq_u64(STATE0, TMP1);
    MSG1 = vsha512su1q_u64(vsha512su0q_u64(MSG1, MSG2), MSG0, vextq_u64(MSG5, MSG6, 1));

    // Rounds 52-53
    MSG_K = vaddq_u64(MSG2, vld1q_u64(&K[2 * 26]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE1);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE0, STATE1, 1), vextq_u64(STATE3, STATE0, 1));
    STATE1 = vsha512h2q_u64(TMP1, STATE3, STATE2);
    STATE3 = vaddq_u64(STATE3, TMP1);
    MSG2 = vsha512su1q_u64(vsha512su0q_u64(MSG2, MSG3), MSG1, vextq_u64(MSG6, MSG7, 1));

    // Rounds 54-55
    MSG_K = vaddq_u64(MSG3, vld1q_u64(&K[2 * 27]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE0);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE3, STATE0, 1), vextq_u64(STATE2, STATE3, 1));
    STATE0 = vsha512h2q_u64(TMP1, STATE2, STATE1);
    STATE2 = vaddq_u64(STATE2, TMP1);
    MSG3 = vsha512su1q_u64(vsha512su0q_u64(MSG3, MSG4), MSG2, vextq_u64(MSG7, MSG0, 1));

    // Rounds 56-57
    MSG_K = vaddq_u64(MSG4, vld1q_u64(&K[2 * 28]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE3);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE2, STATE3, 1), vextq_u64(STATE1, STATE2, 1));
    STATE3 = vsha512h2q_u64(TMP1, STATE1, STATE0);
    STATE1 = vaddq_u64(STATE1, TMP1);
    MSG4 = vsha512su1q_u64(vsha512su0q_u64(MSG4, MSG5), MSG3, vextq_u64(MSG0, MSG1, 1));

    // Rounds 58-59
    MSG_K = vaddq_u64(MSG5, vld1q_u64(&K[2 * 29]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE2);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE1, STATE2, 1), vextq_u64(STATE0, STATE1, 1));
    STATE2 = vsha512h2q_u64(TMP1, STATE0, STATE3);
    STATE0 = vaddq_u64(STATE0, TMP1);
    MSG5 = vsha512su1q_u64(vsha512su0q_u64(MSG5, MSG6), MSG4, vextq_u64(MSG1, MSG2, 1));

    // Rounds 60-61
    MSG_K = vaddq_u64(MSG6, vld1q_u64(&K[2 * 30]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE1);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE0, STATE1, 1), vextq_u64(STATE3, STATE0, 1));
    STATE1 = vsha512h2q_u64(TMP1, STATE3, STATE2);
    STATE3 = vaddq_u64(STATE3, TMP1);
    MSG6 = vsha512su1q_u64(vsha512su0q_u64(MSG6, MSG7), MSG5, vextq_u64(MSG2, MSG3, 1));

    // Rounds 62-63
    MSG_K = vaddq_u64(MSG7, vld1q_u64(&K[2 * 31]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE0);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE3, STATE0, 1), vextq_u64(STATE2, STATE3, 1));
    STATE0 = vsha512h2q_u64(TMP1, STATE2, STATE1);
    STATE2 = vaddq_u64(STATE2, TMP1);
    MSG7 = vsha512su1q_u64(vsha512su0q_u64(MSG7, MSG0), MSG6, vextq_u64(MSG3, MSG4, 1));

    // Rounds 64-65
    MSG_K = vaddq_u64(MSG0, vld1q_u64(&K[2 * 32]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE3);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE2, STATE3, 1), vextq_u64(STATE1, STATE2, 1));
    STATE3 = vsha512h2q_u64(TMP1, STATE1, STATE0);
    STATE1 = vaddq_u64(STATE1, TMP1);

    // Rounds 66-67
    MSG_K = vaddq_u64(MSG1, vld1q_u64(&K[2 * 33]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE2);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE1, STATE2, 1), vextq_u64(STATE0, STATE1, 1));
    STATE2 = vsha512h2q_u64(TMP1, STATE0, STATE3);
    STATE0 = vaddq_u64(STATE0, TMP1);

    // Rounds 68-69
    MSG_K = vaddq_u64(MSG2, vld1q_u64(&K[2 * 34]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE1);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE0, STATE1, 1), vextq_u64(STATE3, STATE0, 1));
    STATE1 = vsha512h2q_u64(TMP1, STATE3, STATE2);
    STATE3 = vaddq_u64(STATE3, TMP1);

    // Rounds 70-71
    MSG_K = vaddq_u64(MSG3, vld1q_u64(&K[2 * 35]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE0);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE3, STATE0, 1), vextq_u64(STATE2, STATE3, 1));
    STATE0 = vsha512h2q_u64(TMP1, STATE2, STATE1);
    STATE2 = vaddq_u64(STATE2, TMP1);

    // Rounds 72-73
    MSG_K = vaddq_u64(MSG4, vld1q_u64(&K[2 * 36]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE3);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE2, STATE3, 1), vextq_u64(STATE1, STATE2, 1));
    STATE3 = vsha512h2q_u64(TMP1, STATE1, STATE0);
    STATE1 = vaddq_u64(STATE1, TMP1);

    // Rounds 74-75
    MSG_K = vaddq_u64(MSG5, vld1q_u64(&K[2 * 37]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE2);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE1, STATE2, 1), vextq_u64(STATE0, STATE1, 1));
    STATE2 = vsha512h2q_u64(TMP1, STATE0, STATE3);
    STATE0 = vaddq_u64(STATE0, TMP1);

    // Rounds 76-77
    MSG_K = vaddq_u64(MSG6, vld1q_u64(&K[2 * 38]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE1);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE0, STATE1, 1), vextq_u64(STATE3, STATE0, 1));
    STATE1 = vsha512h2q_u64(TMP1, STATE3, STATE2);
    STATE3 = vaddq_u64(STATE3, TMP1);

    // Rounds 78-79
    MSG_K = vaddq_u64(MSG7, vld1q_u64(&K[2 * 39]));
    TMP0 = vaddq_u64(vextq_u64(MSG_K, MSG_K, 1), STATE0);
    TMP1 = vsha512hq_u64(TMP0, vextq_u64(STATE3, STATE0, 1), vextq_u64(STATE2, STATE3, 1));
    STATE0 = vsha512h2q_u64(TMP1, STATE2, STATE1);
    STATE2 = vaddq_u64(STATE2, TMP1);

    // Combine states
    STATE0 = vaddq_u64(STATE0, AB_SAVE);
    STATE1 = vaddq_u64(STATE1, CD_SAVE);
    STATE2 = vaddq_u64(STATE2, EF_SAVE);
    STATE3 = vaddq_u64(STATE3, GH_SAVE);
    
    // Save the new state
    vst1q_u64(&m_Context->state[0], STATE0);
    vst1q_u64(&m_Context->state[2], STATE1);
    vst1q_u64(&m_Context->state[4], STATE2);
    vst1q_u64(&m_Context->state[6], STATE3);
}
#elif defined(HM_SIMD_X86)
void Hasher<SHA512, HARDWARE>::TransformX86(const uint8_t* const data)
{
    //static_assert(0, "Not implemented");
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
            const uint64_t remainingBufferSpace = SHA512_BLOCK_LENGTH - m_Context->bufferSize;
            const uint64_t bytesToCopy = std::min(remainingBytes, remainingBufferSpace);

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
