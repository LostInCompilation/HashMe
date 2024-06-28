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
/*  HasherBase.hpp                                                  */
/*  Created: 26.06.2024                                             */
/*------------------------------------------------------------------*/

#ifndef HasherBase_hpp
#define HasherBase_hpp

namespace HashMe
{

// ***************************************************
// Abstract hasher base class
class HasherBase
{
protected:
    HasherBase() = default;
    
public:
    virtual void Initialize() = 0;
    
    virtual void Update(const uint8_t* const data, const uint64_t size) = 0;
    virtual void Update(const std::vector<uint8_t>& data) = 0;
    virtual void Update(const std::string& str) = 0;
    
    virtual std::vector<uint8_t> End() = 0;
};

}

#endif /* HasherBase_hpp */
