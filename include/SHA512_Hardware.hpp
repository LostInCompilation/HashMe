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
/*  SHA512_Hardware.hpp                                             */
/*  Created: 20.07.2024                                             */
/*------------------------------------------------------------------*/

#ifndef SHA512_Hardware_hpp
#define SHA512_Hardware_hpp

#if defined(HM_SIMD_ARM) || defined(HM_SIMD_X86)

namespace HashMe
{

// Dummy types for template
struct HARDWARE;
struct SHA512;

// ***************************************************
// Forward declaration for hasher class
template <typename HashAlgorithm, typename HardwareSoftwareImplementation>
class Hasher;

// ***************************************************
// Hasher class for SHA512 with SIMD support
template <>
class Hasher<SHA512, HARDWARE> : public Hasher<SHA512, SOFTWARE>
{
private:
#if defined(HM_SIMD_ARM)
    void         TransformARM(const uint8_t* data);
#elif defined(HM_SIMD_X86)
    void         TransformX86(const uint8_t* data);
#endif
    
protected:
    virtual void Transform(const uint8_t* const data) override;
    
public:
    Hasher() = default;
    virtual ~Hasher() = default;
    
    // Allow copy but no assign
    Hasher(const Hasher& other);
    Hasher& operator=(const Hasher& other) = delete;
    const Hasher& operator=(const Hasher& other) const = delete;
    
    // Methods
    virtual void Update(const uint8_t* const data, const uint64_t size) override;
    virtual void Update(const std::vector<uint8_t>& data) override;
    virtual void Update(const std::string& str) override;
    
    [[nodiscard]] virtual std::vector<uint8_t> End() override;
};

}

#endif /* HM_SIMD_ARM || HM_SIMD_X86 */

#endif /* SHA512_Hardware_hpp */
