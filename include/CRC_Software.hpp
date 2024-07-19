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
/*  CRC_Software.hpp                                                */
/*  Created: 14.07.2024                                             */
/*------------------------------------------------------------------*/

#ifndef CRC_Software_hpp
#define CRC_Software_hpp

namespace HashMe
{

// Dummy types for template
struct SOFTWARE;
struct CRC16;
struct CRC32;
struct CRC64;

// ***************************************************
// Base class for CRC algorithm and lookup table creation
template <typename CRCSize>
class CRCBase
{
private:
    // Constants for configuring the CRC algorithm
    const CRCSize m_Polynomial;
    const CRCSize m_FirstRemainder;
    const CRCSize m_FinalXOR;
    
    // Methods
    template <typename WordSize>
    WordSize Reflect(WordSize data, const uint8_t numOfBits) const;
    
protected:
    // The precomputed lookup table
    std::array<CRCSize, 256> m_LookupTable;
    
    // Ctor, Dtor are protected
    CRCBase(const CRCSize polynomial, const CRCSize firstRemainder, const CRCSize finalXOR);
    virtual ~CRCBase() = default;
    
    // Allow copy but no assign
    CRCBase(const CRCBase& other);
    CRCBase& operator=(const CRCBase& other) = delete;
    const CRCBase& operator=(const CRCBase& other) const = delete;
    
    // Methods
    void CreateLookupTable();
    [[nodiscard]] CRCSize Compute(const CRCSize currentCRC, const uint8_t* const data, const uint64_t size);
};

// ***************************************************
// Forward declaration for hasher class
template <typename HashAlgorithm, typename HardwareSoftwareImplementation>
class Hasher;

// ***************************************************
// Hasher class for CRC16
template <>
class Hasher<CRC16, SOFTWARE> : public HasherBase, public CRCBase<uint16_t>
{
private:
    uint16_t    m_CRC16Result = 0;
    
    virtual void Initialize() override {}
    
public:
    Hasher();
    virtual ~Hasher() = default;
    
    // Allow copy but no assign
    Hasher(const Hasher& other);
    Hasher& operator=(const Hasher& other) = delete;
    const Hasher& operator=(const Hasher& other) const = delete;
    
    // Methods
    virtual void Reset() override;
    
    virtual void Update(const uint8_t* const data, const uint64_t size) override;
    virtual void Update(const std::vector<uint8_t>& data) override;
    virtual void Update(const std::string& str) override;
    
    [[nodiscard]] virtual std::vector<uint8_t> End() override;
    [[nodiscard]] uint16_t End_GetAsInteger16();
};

// ***************************************************
// Hasher class for CRC32
template <>
class Hasher<CRC32, SOFTWARE> : public HasherBase, public CRCBase<uint32_t>
{
private:
    uint32_t    m_CRC32Result = 0;
    
    virtual void Initialize() override {}
    
public:
    Hasher();
    virtual ~Hasher() = default;
    
    // Allow copy but no assign
    Hasher(const Hasher& other);
    Hasher& operator=(const Hasher& other) = delete;
    const Hasher& operator=(const Hasher& other) const = delete;
    
    // Methods
    virtual void Reset() override;
    
    virtual void Update(const uint8_t* const data, const uint64_t size) override;
    virtual void Update(const std::vector<uint8_t>& data) override;
    virtual void Update(const std::string& str) override;
    
    [[nodiscard]] virtual std::vector<uint8_t> End() override;
    [[nodiscard]] uint32_t End_GetAsInteger32();
};

// ***************************************************
// Hasher class for CRC64
template <>
class Hasher<CRC64, SOFTWARE> : public HasherBase, public CRCBase<uint64_t>
{
private:
    uint64_t    m_CRC64Result = 0;
    
    virtual void Initialize() override {}
    
public:
    Hasher();
    virtual ~Hasher() = default;
    
    // Allow copy but no assign
    Hasher(const Hasher& other);
    Hasher& operator=(const Hasher& other) = delete;
    const Hasher& operator=(const Hasher& other) const = delete;
    
    // Methods
    virtual void Reset() override;
    
    virtual void Update(const uint8_t* const data, const uint64_t size) override;
    virtual void Update(const std::vector<uint8_t>& data) override;
    virtual void Update(const std::string& str) override;
    
    [[nodiscard]] virtual std::vector<uint8_t> End() override;
    [[nodiscard]] uint64_t End_GetAsInteger64();
};

}
#endif /* CRC_Software_hpp */
