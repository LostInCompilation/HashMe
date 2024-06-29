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
/*  MD5.hpp                                                         */
/*  Created: 28.06.2024                                             */
/*------------------------------------------------------------------*/

#ifndef MD5_hpp
#define MD5_hpp

namespace HashMe
{

// Dummy types for template
struct SOFTWARE;
struct MD5;

// ***************************************************
// Forward declaration for hasher class
template <typename HashAlgorithm, typename HardwareSoftwareImplementation>
class Hasher;

// ***************************************************
// Hasher class for MD5
template <>
class Hasher<MD5, SOFTWARE> : public HasherBase
{
private:
    // Constants
    static const uint8_t MD5_BLOCK_LENGTH = 64; // TODO: check if used everywhere
    
    struct Context
    {
        uint32_t    state[4] = {0}; // ABCD
        uint8_t     buffer[MD5_BLOCK_LENGTH] = {0};
        
        uint32_t    count[2] = {0};
    };
    
    // Our context
    Context*     m_Context = nullptr;
    
    // Methods
    virtual void Initialize() override;
    void Transform(const uint8_t block[MD5_BLOCK_LENGTH]);
    
public:
    Hasher();
    ~Hasher();
    
    // Allow copy but no assign
    Hasher(const Hasher& other);
    Hasher& operator=(const Hasher& other) = delete;
    const Hasher& operator=(const Hasher& other) const = delete;
    
    // Methods
    virtual void Reset() override;
    
    virtual void Update(const uint8_t* const data, const uint64_t size) override;
    virtual void Update(const std::vector<uint8_t>& data) override;
    virtual void Update(const std::string& str) override;
    
    virtual std::vector<uint8_t> End() override;
};

}

#endif /* MD5_hpp */
