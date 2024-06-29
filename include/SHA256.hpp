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
/*  SHA256.hpp                                                      */
/*  Created: 26.06.2024                                             */
/*------------------------------------------------------------------*/

#ifndef SHA256_hpp
#define SHA256_hpp

namespace HashMe
{

// Dummy types for template
struct SOFTWARE;
struct SHA256;

// ***************************************************
// Forward declaration for hasher class
template <typename HashAlgorithm, typename HardwareSoftwareImplementation>
class Hasher;

// ***************************************************
// Hasher class for SHA256 using software implementation
template <>
class Hasher<SHA256, SOFTWARE> : public HasherBase
{
private:
    // Constants
    static constexpr uint32_t SHA256_BLOCK_LENGTH = 64;
    
    struct Context
    {
        uint32_t    state[8] = {0};
        
        uint8_t     bufferSize = 0;
        uint64_t    numOfBits = 0;
        uint8_t     buffer[SHA256_BLOCK_LENGTH] = {0};
    };
    
    // Our context
    Context*     m_Context = nullptr;

    // Methods
    virtual void Initialize() override;
    void Transform(const uint8_t* const data);
    
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

#endif /* SHA256_hpp */
