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
/*  HashMe.hpp                                                      */
/*  Created: 26.06.2024                                             */
/*------------------------------------------------------------------*/

#ifndef HashMe_hpp
#define HashMe_hpp

// ***************************************************
// predef library
#include "predef.h"

// HashMe platform specific config
#include "PlatformConfig.hpp"

// ***************************************************
// System includes
#include <cstdint>
#include <cstring>
#include <memory>
#include <array>
#include <vector>

#ifdef HM_COMPILER_HAS_FMT_CAPABILITY
#include <format>
#endif

// ***************************************************
// Lib includes
#include "HashMeVersion.hpp"
#include "Utils.hpp"
#include "HasherBase.hpp"
#include "SHA256.hpp"
#include "SHA256_Hardware.hpp"
#include "MD5.hpp"

// ***************************************************
// Platform specific includes for SIMD
#ifdef HM_SIMD_ARM
#include <arm_neon.h>
#endif

// x86 SHA SIMD
#if defined(HASH_PREDEF_COMP_GNUC_AVAILABLE) && defined(HM_SIMD_X86)
#include <x86intrin.h>
#endif

#if defined(HASH_PREDEF_COMP_MSVC_AVAILABLE) && defined(HM_SIMD_X86)
# include <immintrin.h>
#endif

// ***************************************************
// Lib settings
#define HM_ENABLE_NAMESPACE_ALIAS // Enable the "HM" namespace alias additionally to "HashMe"
//#define HM_CLEANUP_TMP_MEMORY_AFTER_HASHING_FOR_SECURITY // Sets temporary variables and hash states to zero after hash calculation for better security

namespace HashMe
{

// ***************************************************
// Dummy types for template
struct SOFTWARE;

// ***************************************************
// Forward declaration for hasher class
template <typename HashAlgorithm, typename HardwareSoftwareImplementation = SOFTWARE>
class Hasher;

// ***************************************************
// Version info
int32_t GetVersionMajor();
int32_t GetVersionMinor();
int32_t GetVersionPatch();
std::string GetVersionString();

std::string GetDescription();

}

// Namespace alias
#ifdef HM_ENABLE_NAMESPACE_ALIAS
namespace HM = HashMe;
#endif

#endif /* HashMe_hpp */
