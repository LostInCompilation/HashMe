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
    std::copy(INITIAL_HASH_VALUES.begin(), INITIAL_HASH_VALUES.end(), m_State);
}

void Hasher<SHA224, HARDWARE>::Reset()
{
    Initialize();
}

std::vector<uint8_t> Hasher<SHA224, HARDWARE>::End()
{
    std::vector<uint8_t> hash(28); // 224 bit hash
    
    // Assemble hash
    for(uint32_t i = 0; i < 7; i++) // Use uint32_t to avoid implicit sign conversion by left shift operator
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
