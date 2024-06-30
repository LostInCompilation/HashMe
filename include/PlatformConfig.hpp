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
/*  PlatformConfig.hpp                                              */
/*  Created: 28.06.2024                                             */
/*------------------------------------------------------------------*/

#ifndef PlatformConfig_hpp
#define PlatformConfig_hpp

// ***************************************************
// Compiler detection
#if !defined(HASH_PREDEF_COMP_MSVC_AVAILABLE) && !defined(HASH_PREDEF_COMP_CLANG_AVAILABLE) && !defined(HASH_PREDEF_COMP_GNUC_AVAIABLE)
#error Currently only GCC, CLANG and MSVC compilers are tested. You can compile with a different compiler by commenting out this error.
#endif

// ***************************************************
// Endianness
#ifdef HASH_PREDEF_ENDIAN_BIG_BYTE_AVAILABLE
    #define HM_BIG_ENDIAN
#elif defined(HASH_PREDEF_ENDIAN_LITTLE_BYTE_AVAILABLE)
    #define HM_LITTLE_ENDIAN
#elif define(HASH_PREDEF_ENDIAN_BIG_WORD_AVAILABLE) || defined(HASH_PREDEF_ENDIAN_LITTLE_WORD_AVAILABLE)
    #error Middle endian is not supported.
#endif

// ***************************************************
// SIMD
#ifdef HASH_PREDEF_HW_SIMD_AVAILABLE
    #if HASH_PREDEF_HW_SIMD_ARM
        #if (HASH_PREDEF_HW_SIMD_ARM >= HASH_PREDEF_HW_SIMD_ARM_NEON_VERSION) // ARM NEON available (ARMv8)
            #define HM_SIMD_ARM
        #endif
    #elif HASH_PREDEF_HW_SIMD_X86
        #error Implement SIMD for x86
    #elif HASH_PREDEF_HW_SIMD_X86_AMD
        #error Implement SIMD for x86 AMD
    #endif
#else
#warning Hardware acceleration (SIMD) is not available on this platform. Did you forget a compiler switch?
#endif

#endif /* PlatformConfig_hpp */
