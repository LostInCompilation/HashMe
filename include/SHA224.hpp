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
/*  SHA224.hpp                                                      */
/*  Created: 13.07.2024                                             */
/*------------------------------------------------------------------*/

#ifndef SHA224_hpp
#define SHA224_hpp

namespace HashMe
{

// Dummy types for template
struct SOFTWARE;
struct SHA224;

// ***************************************************
// Forward declaration for hasher class
template <typename HashAlgorithm, typename HardwareSoftwareImplementation>
class Hasher;

// ***************************************************
// Hasher class for SHA224 using software implementation
template <>
class Hasher<SHA224, SOFTWARE> : public Hasher<SHA256, SOFTWARE>
{
private:
    // ***************************************************
    // Constants
    inline static constexpr std::array<uint32_t, 8> INITIAL_HASH_VALUES = {
        0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
        0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
    };
    
    // Methods
    virtual void Initialize() override;
    
public:
    Hasher();
    ~Hasher() = default;
    
    // Allow copy but no assign
    Hasher(const Hasher& other);
    Hasher& operator=(const Hasher& other) = delete;
    const Hasher& operator=(const Hasher& other) const = delete;
    
    // Methods
    virtual void Reset() override;
    
    //virtual void Update(const uint8_t* const data, const uint64_t size) override;
    //virtual void Update(const std::vector<uint8_t>& data) override;
    //virtual void Update(const std::string& str) override;
    
    virtual std::vector<uint8_t> End() override;
};

}

#endif /* SHA224_hpp */
