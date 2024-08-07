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
/*  Utils.cpp                                                       */
/*  Created: 29.06.2024                                             */
/*------------------------------------------------------------------*/

#include "HashMe.hpp"

using namespace HashMe;

// ***************************************************
// Helper function to convert the hash to a string of hex values with fixed size (two digits)
#ifdef HM_COMPILER_HAS_FMT_CAPABILITY
std::string Utils::HashToHexString(const std::vector<uint8_t>& hash)
{
    std::string result = "";
    
    for(auto i : hash)
        result.append(std::format("{:02x}", i));
    
    return result;
}
#else
std::string Utils::HashToHexString(const std::vector<uint8_t>& hash)
{
    const char lookup[] = "0123456789abcdef";
    std::string result = "";
    
    for(uint32_t i = 0; i < hash.size(); i++)
    {
        result += lookup[hash[i] >> 4];
        result += lookup[hash[i] & 0x0F];
    }
    
    return result;
}
#endif
