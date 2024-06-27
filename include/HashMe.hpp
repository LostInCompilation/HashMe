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

// Lib includes
#include "HasherBase.hpp"

// Lib settings
#define HM_ENABLE_NAMESPACE_ALIAS // Enable the "HM" namespace alias additionally to "HashMe"

namespace HashMe
{

// Supported hash algorithms. Use dummy structs
struct MD5;
struct SHA256;

// ***************************************************
// Forward declaration for hasher class
template <typename HashAlgorithm>
class Hasher;

// ***************************************************
// Hasher class for MD5
template <>
class Hasher<MD5> : public HasherBase
{
private:
    
public:
    Hasher() = default;
    
    virtual void foo() override;
};

// ***************************************************
// Hasher class for SHA256
template <>
class Hasher<SHA256> : public HasherBase
{
private:
    
public:
    Hasher() = default;
};
}

#ifdef HM_ENABLE_NAMESPACE_ALIAS
// Namespace alias
namespace HM = HashMe;
#endif

#endif /* HashMe_hpp */