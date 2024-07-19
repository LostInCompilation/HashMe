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
/*  Utils.hpp                                                       */
/*  Created: 29.06.2024                                             */
/*------------------------------------------------------------------*/

#ifndef Utils_hpp
#define Utils_hpp

namespace HashMe::Utils
{

// ***************************************************
// Reverse endianness of integer
template <typename T>
concept IsInteger = std::is_integral_v<T> and !std::is_same_v<T, bool>;

#ifdef __cpp_lib_byteswap // Is std::byteswap supported (C++23)?
template <IsInteger Integer>
constexpr inline Integer ReverseEndianness(const Integer integer)
{
    if constexpr (sizeof(Integer) > 1)
        return std::byteswap(integer);
    else
        return integer;
}
#else
template <IsInteger Integer>
constexpr inline Integer ReverseEndianness(const Integer integer)
{
#ifdef HASH_PREDEF_COMP_MSVC_AVAILABLE // MSVC compiler
    if constexpr (sizeof(Integer) == 2)
        return _byteswap_ushort(integer);
    else if constexpr (sizeof(Integer) == 4)
        return _byteswap_ulong(integer);
    else if constexpr (sizeof(Integer) == 8)
        return _byteswap_uint64(integer);
#else
    if constexpr (sizeof(Integer) == 2)
        return __builtin_bswap16(integer);
    else if constexpr (sizeof(Integer) == 4)
        return __builtin_bswap32(integer);
    else if constexpr (sizeof(Integer) == 8)
        return __builtin_bswap64(integer);
#endif
    // For single byte types
    return integer;
}
#endif

// Dummy types for conversion setting
struct REVERSE_ENDIANNESS;
struct KEEP_ENDIANNESS;

// ***************************************************
// Convert four bytes to 32 bit unsigned integer with optional endianness reversal
template <typename EndiannessSetting>
constexpr inline uint32_t U8toU32(const uint8_t data[4]);

template <>
constexpr inline uint32_t U8toU32<REVERSE_ENDIANNESS>(const uint8_t data[4])
{
    return (static_cast<uint32_t>(data[0]) << 24UL) | (static_cast<uint32_t>(data[1]) << 16UL) | (static_cast<uint32_t>(data[2]) << 8UL) | (data[3]);
}
                                                                      
template <>
constexpr inline uint32_t U8toU32<KEEP_ENDIANNESS>(const uint8_t data[4])
{
    return (data[0]) | (static_cast<uint32_t>(data[1]) << 8UL) | (static_cast<uint32_t>(data[2]) << 16UL) | (static_cast<uint32_t>(data[3]) << 24UL);
}

// ***************************************************
// Convert eight bytes to 64 bit unsigned integer with optional endianness reversal
template <typename EndiannessSetting>
constexpr inline uint64_t U8toU64(const uint8_t data[8]);

template <>
constexpr inline uint64_t U8toU64<REVERSE_ENDIANNESS>(const uint8_t data[8])
{
    return (static_cast<uint64_t>(data[0]) << 56ULL) | (static_cast<uint64_t>(data[1]) << 48ULL) | (static_cast<uint64_t>(data[2]) << 40ULL) |
        (static_cast<uint64_t>(data[3]) << 32ULL) | (static_cast<uint64_t>(data[4]) << 24ULL) | (static_cast<uint64_t>(data[5]) << 16ULL) |
        (static_cast<uint64_t>(data[6]) << 8ULL) | (data[7]);
}
                                                                      
template <>
constexpr inline uint64_t U8toU64<KEEP_ENDIANNESS>(const uint8_t data[8])
{
    return (data[0]) | (static_cast<uint64_t>(data[1]) << 8ULL) | (static_cast<uint64_t>(data[2]) << 16ULL) | (static_cast<uint64_t>(data[3]) << 24ULL) |
        (static_cast<uint64_t>(data[4]) << 32ULL) | (static_cast<uint64_t>(data[5]) << 40ULL) | (static_cast<uint64_t>(data[6]) << 48ULL) |
        (static_cast<uint64_t>(data[7]) << 56ULL);
}

// ***************************************************
// Convert 16 bit unsigned integer to two bytes with optional endianness reversal
template <typename EndiannessSetting>
constexpr inline void U16toU8(const uint16_t data, uint8_t out[2]);

template <>
constexpr inline void U16toU8<REVERSE_ENDIANNESS>(const uint16_t data, uint8_t out[2])
{
    out[0] = static_cast<uint8_t>((data >> 8) & 0xFF);
    out[1] = static_cast<uint8_t>(data & 0xFF);
}
                                                                      
template <>
constexpr inline void U16toU8<KEEP_ENDIANNESS>(const uint16_t data, uint8_t out[2])
{
    out[0] = static_cast<uint8_t>(data & 0xFF);
    out[1] = static_cast<uint8_t>((data >> 8) & 0xFF);
}

// ***************************************************
// Convert 32 bit unsigned integer to four bytes with optional endianness reversal
template <typename EndiannessSetting>
constexpr inline void U32toU8(const uint32_t data, uint8_t out[4]);

template <>
constexpr inline void U32toU8<REVERSE_ENDIANNESS>(const uint32_t data, uint8_t out[4])
{
    out[0] = static_cast<uint8_t>((data >> 24) & 0xFF);
    out[1] = static_cast<uint8_t>((data >> 16) & 0xFF);
    out[2] = static_cast<uint8_t>((data >> 8) & 0xFF);
    out[3] = static_cast<uint8_t>(data & 0xFF);
}
                                                                      
template <>
constexpr inline void U32toU8<KEEP_ENDIANNESS>(const uint32_t data, uint8_t out[4])
{
    out[0] = static_cast<uint8_t>(data & 0xFF);
    out[1] = static_cast<uint8_t>((data >> 8) & 0xFF);
    out[2] = static_cast<uint8_t>((data >> 16) & 0xFF);
    out[3] = static_cast<uint8_t>((data >> 24) & 0xFF);
}

// ***************************************************
// Convert 64 bit unsigned integer to eight bytes with optional endianness reversal
template <typename EndiannessSetting>
constexpr inline void U64toU8(const uint64_t data, uint8_t out[8]);

template <>
constexpr inline void U64toU8<REVERSE_ENDIANNESS>(const uint64_t data, uint8_t out[8])
{
    out[0] = static_cast<uint8_t>((data >> 56) & 0xFF);
    out[1] = static_cast<uint8_t>((data >> 48) & 0xFF);
    out[2] = static_cast<uint8_t>((data >> 40) & 0xFF);
    out[3] = static_cast<uint8_t>((data >> 32) & 0xFF);
    
    out[4] = static_cast<uint8_t>((data >> 24) & 0xFF);
    out[5] = static_cast<uint8_t>((data >> 16) & 0xFF);
    out[6] = static_cast<uint8_t>((data >> 8) & 0xFF);
    out[7] = static_cast<uint8_t>(data & 0xFF);
}
                                                                      
template <>
constexpr inline void U64toU8<KEEP_ENDIANNESS>(const uint64_t data, uint8_t out[8])
{
    out[0] = static_cast<uint8_t>(data & 0xFF);
    out[1] = static_cast<uint8_t>((data >> 8) & 0xFF);
    out[2] = static_cast<uint8_t>((data >> 16) & 0xFF);
    out[3] = static_cast<uint8_t>((data >> 24) & 0xFF);
    
    out[4] = static_cast<uint8_t>((data >> 32) & 0xFF);
    out[5] = static_cast<uint8_t>((data >> 40) & 0xFF);
    out[6] = static_cast<uint8_t>((data >> 48) & 0xFF);
    out[7] = static_cast<uint8_t>((data >> 56) & 0xFF);
}

// ***************************************************
// Helper function to convert the hash to a string of hex values with fixed size (two digits)
std::string HashToHexString(const std::vector<uint8_t>& hash);

}

#endif /* Utils_hpp */
