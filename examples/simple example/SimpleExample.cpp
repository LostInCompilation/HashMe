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
/*  SimpleExample.cpp                                               */
/*  Created: 26.06.2024                                             */
/*------------------------------------------------------------------*/

// System includes
#include <iostream>
#include <string>
#include <random>
#include <assert.h>

// Benchmarking lib
#define PICOBENCH_IMPLEMENT
#include "picobench.h"

// HashMe lib
#include "HashMe.hpp"

using namespace HashMe;

// ***************************************************
// Averager class for benchmarking
template <typename T>
class Averager
{
private:
    T        m_Sum = 0;
    uint64_t m_NumDataPoints = 0;
    
public:
    void AddDatapoint(const T datapoint)
    {
        m_NumDataPoints++;
        m_Sum += datapoint;
    }
    
    T GetAverage() const
    {
        if(m_NumDataPoints > 0)
            return static_cast<double>(m_Sum) / static_cast<double>(m_NumDataPoints);
        else
            return 0;
    }
};

// ***************************************************
// Test data
static constexpr uint64_t bigDataSize = 300 * 1024 * 1024; // 300MB
std::vector<uint8_t>* bigData = nullptr; // Allocate on heap to prevent "Compiler out of heap space error" in VisualStudio

const std::string testString = "123";
//const std::string testString = "12345678901234567890123456789012345678901234567890123456789012345"; // 65 chars

// Hash results for checking
static constexpr std::string_view testStringHashSHA224_expected = "78d8045d684abd2eece923758f3cd781489df3a48e1278982466017f";
static constexpr std::string_view testStringHashSHA256_expected = "a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3";
static constexpr std::string_view testStringHashMD5_expected = "202cb962ac59075b964b07152d234b70";
static constexpr std::string_view testStringHashCRC16_expected = "ba04";
static constexpr std::string_view testStringHashCRC32_expected = "884863d2";
static constexpr std::string_view testStringHashCRC64_expected = "30232844071cc561";

// Testing mode
#define TEST_BIG_DATA // Use big data for test

// ***************************************************
// SHA256 software implementation
std::string SHA256_Software_Hash = "";
Averager<double> SHA256_SoftwareAverager;
void SHA256_Software(picobench::state& s)
{
    std::vector<uint8_t> hashResult;
    Hasher<SHA256, SOFTWARE> hasher;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Reset();
        
#ifdef TEST_BIG_DATA
        hasher.Update(*bigData);
#else
        hasher.Update(testString);
        //hasher.Update(longTestString);
#endif
        hashResult = hasher.End();
        
        s.stop_timer();
    }
    
    SHA256_Software_Hash = Utils::HashToHexString(hashResult);
    SHA256_SoftwareAverager.AddDatapoint(static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0));
}
PICOBENCH(SHA256_Software);

// ***************************************************
// SHA256 hardware implementation
std::string SHA256_Hardware_Hash = "";
Averager<double> SHA256_HardwareAverager;
void SHA256_Hardware(picobench::state& s)
{
    std::vector<uint8_t> hashResult;
    Hasher<SHA256, HARDWARE> hasher;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Reset();
        
#ifdef TEST_BIG_DATA
        hasher.Update(*bigData);
#else
        hasher.Update(testString);
        //hasher.Update(longTestString);
#endif
        hashResult = hasher.End();
        
        s.stop_timer();
    }
    
    SHA256_Hardware_Hash = Utils::HashToHexString(hashResult);
    SHA256_HardwareAverager.AddDatapoint(static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0));
}
PICOBENCH(SHA256_Hardware).baseline();

// ***************************************************
// SHA224 software implementation
std::string SHA224_Software_Hash = "";
Averager<double> SHA224_SoftwareAverager;
void SHA224_Software(picobench::state& s)
{
    std::vector<uint8_t> hashResult;
    Hasher<SHA224, SOFTWARE> hasher;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Reset();
        
#ifdef TEST_BIG_DATA
        hasher.Update(*bigData);
#else
        hasher.Update(testString);
        //hasher.Update(longTestString);
#endif
        hashResult = hasher.End();
        
        s.stop_timer();
    }
    
    SHA224_Software_Hash = Utils::HashToHexString(hashResult);
    SHA224_SoftwareAverager.AddDatapoint(static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0));
}
PICOBENCH(SHA224_Software);

// ***************************************************
// SHA224 hardware implementation
std::string SHA224_Hardware_Hash = "";
Averager<double> SHA224_HardwareAverager;
void SHA224_Hardware(picobench::state& s)
{
    std::vector<uint8_t> hashResult;
    Hasher<SHA224, HARDWARE> hasher;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Reset();
        
#ifdef TEST_BIG_DATA
        hasher.Update(*bigData);
#else
        hasher.Update(testString);
        //hasher.Update(longTestString);
#endif
        hashResult = hasher.End();
        
        s.stop_timer();
    }
    
    SHA224_Hardware_Hash = Utils::HashToHexString(hashResult);
    SHA224_HardwareAverager.AddDatapoint(static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0));
}
PICOBENCH(SHA224_Hardware);

// ***************************************************
// MD5 software implementation
std::string MD5_Software_Hash = "";
Averager<double> MD5_SoftwareAverager;
void MD5_Software(picobench::state& s)
{
    std::vector<uint8_t> hashResult;
    Hasher<MD5, SOFTWARE> hasher;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Reset();
        
#ifdef TEST_BIG_DATA
        hasher.Update(*bigData);
#else
        hasher.Update(testString);
        //hasher.Update(longTestString);
#endif
        hashResult = hasher.End();
        
        s.stop_timer();
    }
    
    MD5_Software_Hash = Utils::HashToHexString(hashResult);
    MD5_SoftwareAverager.AddDatapoint(static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0));
}
PICOBENCH(MD5_Software);

// ***************************************************
// CRC16 software implementation
std::string CRC16_Software_Hash = "";
uint32_t CRC16_Software_Hash_Int16 = 0;
Averager<double> CRC16_SoftwareAverager;
void CRC16_Software(picobench::state& s)
{
    uint16_t hashResultInt16;
    std::vector<uint8_t> hashResult;
    
    Hasher<CRC16, SOFTWARE> hasher;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Reset();
        
#ifdef TEST_BIG_DATA
        hasher.Update(*bigData);
#else
        hasher.Update(testString);
        //hasher.Update(longTestString);
#endif
        hashResult = hasher.End();
        hashResultInt16 = hasher.End_GetAsInteger16();
        
        s.stop_timer();
    }
    
    CRC16_Software_Hash_Int16 = hashResultInt16;
    CRC16_Software_Hash = Utils::HashToHexString(hashResult);
    
    CRC16_SoftwareAverager.AddDatapoint(static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0));
}
PICOBENCH(CRC16_Software);

// ***************************************************
// CRC16 hardware implementation
//std::string CRC16_Hardware_Hash = "";
//uint32_t CRC16_Hardware_Hash_Int16 = 0;
//Averager<double> CRC16_HardwareAverager;
//void CRC16_Hardware(picobench::state& s)
//{
//    uint16_t hashResultInt16;
//    std::vector<uint8_t> hashResult;
//    
//    Hasher<CRC32, HARDWARE> hasher;
//    
//    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
//    {
//        s.start_timer();
//        
//        hasher.Reset();
//        
//#ifdef TEST_BIG_DATA
//        hasher.Update(*bigData);
//#else
//        hasher.Update(testString);
//        //hasher.Update(longTestString);
//#endif
//        hashResult = hasher.End();
//        hashResultInt16 = hasher.End_GetAsInteger16();
//        
//        s.stop_timer();
//    }
//    
//    CRC16_Hardware_Hash_Int16 = hashResultInt16;
//    CRC16_Hardware_Hash = Utils::HashToHexString(hashResult);
//    
//    CRC16_HardwareAverager.AddDatapoint(static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0));
//}
//PICOBENCH(CRC16_Hardware);

// ***************************************************
// CRC32 software implementation
std::string CRC32_Software_Hash = "";
uint32_t CRC32_Software_Hash_Int32 = 0;
Averager<double> CRC32_SoftwareAverager;
void CRC32_Software(picobench::state& s)
{
    uint32_t hashResultInt32;
    std::vector<uint8_t> hashResult;
    
    Hasher<CRC32, SOFTWARE> hasher;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Reset();
        
#ifdef TEST_BIG_DATA
        hasher.Update(*bigData);
#else
        hasher.Update(testString);
        //hasher.Update(longTestString);
#endif
        hashResult = hasher.End();
        hashResultInt32 = hasher.End_GetAsInteger32();
        
        s.stop_timer();
    }
    
    CRC32_Software_Hash_Int32 = hashResultInt32;
    CRC32_Software_Hash = Utils::HashToHexString(hashResult);
    
    CRC32_SoftwareAverager.AddDatapoint(static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0));
}
PICOBENCH(CRC32_Software);

// ***************************************************
// CRC32 hardware implementation
std::string CRC32_Hardware_Hash = "";
uint32_t CRC32_Hardware_Hash_Int32 = 0;
Averager<double> CRC32_HardwareAverager;
void CRC32_Hardware(picobench::state& s)
{
    uint32_t hashResultInt32;
    std::vector<uint8_t> hashResult;
    
    Hasher<CRC32, HARDWARE> hasher;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Reset();
        
#ifdef TEST_BIG_DATA
        hasher.Update(*bigData);
#else
        hasher.Update(testString);
        //hasher.Update(longTestString);
#endif
        hashResult = hasher.End();
        hashResultInt32 = hasher.End_GetAsInteger32();
        
        s.stop_timer();
    }
    
    CRC32_Hardware_Hash_Int32 = hashResultInt32;
    CRC32_Hardware_Hash = Utils::HashToHexString(hashResult);
    
    CRC32_HardwareAverager.AddDatapoint(static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0));
}
PICOBENCH(CRC32_Hardware);

// ***************************************************
// CRC64 software implementation
std::string CRC64_Software_Hash = "";
uint64_t CRC64_Software_Hash_Int64 = 0;
Averager<double> CRC64_SoftwareAverager;
void CRC64_Software(picobench::state& s)
{
    uint64_t hashResultInt64;
    std::vector<uint8_t> hashResult;
    
    Hasher<CRC64, SOFTWARE> hasher;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Reset();
        
#ifdef TEST_BIG_DATA
        hasher.Update(*bigData);
#else
        hasher.Update(testString);
        //hasher.Update(longTestString);
#endif
        hashResult = hasher.End();
        hashResultInt64 = hasher.End_GetAsInteger64();
        
        s.stop_timer();
    }
    
    CRC64_Software_Hash_Int64 = hashResultInt64;
    CRC64_Software_Hash = Utils::HashToHexString(hashResult);
    
    CRC64_SoftwareAverager.AddDatapoint(static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0));
}
PICOBENCH(CRC64_Software);

// ***************************************************
// Fill big data with random values
void PrepareBigData()
{
    std::cout << "Generating random big data..." << std::flush;
    
    bigData = new std::vector<uint8_t>(bigDataSize);
    
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<unsigned int> dist(0, 255);
        
    for (uint64_t i = 0; i < bigDataSize; i++)
        bigData->at(i) = dist(mt);
    
    std::cout << "Done! (" << bigDataSize / 1024 / 1024 << " MB)" << std::endl << std::endl;
}

// ***************************************************
// Startup text
void PrintStartupHeader()
{
    std::cout << "*****************************************************" << std::endl;
    std::cout << "* " << HashMe::GetDescription() << " *" << std::endl;
    std::cout << "*                                                   *" << std::endl;
    std::cout << "*                 Simple Example                    *" << std::endl;
    std::cout << "*                                                   *" << std::endl;
    std::cout << "* Lib version: " << HashMe::GetVersionString() << "                               *" << std::endl;
    std::cout << "*****************************************************" << std::endl << std::endl;
}

// ***************************************************
// Print measured speed and computed hashes
void PrintSpeedAndHash()
{
    // SHA224
    std::cout << "***********************************************************" << std::endl;
    std::cout << "SHA224 (Software): " << std::fixed << std::setprecision(2) << SHA224_SoftwareAverager.GetAverage() << " MB/s" << std::endl;
    std::cout << "SHA224 (Software): " << SHA224_Software_Hash << std::endl << std::endl;
    
    std::cout << "***********************************************************" << std::endl;
    std::cout << "SHA224 (Hardware): " << std::fixed << std::setprecision(2) << SHA224_HardwareAverager.GetAverage() << " MB/s" << std::endl;
    std::cout << "SHA224 (Hardware): " << SHA224_Hardware_Hash << std::endl << std::endl;
    
    // SHA256
    std::cout << "***********************************************************" << std::endl;
    std::cout << "SHA256 (Software): " << std::fixed << std::setprecision(2) << SHA256_SoftwareAverager.GetAverage() << " MB/s" << std::endl;
    std::cout << "SHA256 (Software): " << SHA256_Software_Hash << std::endl << std::endl;
    
    std::cout << "***********************************************************" << std::endl;
    std::cout << "SHA256 (Hardware): " << std::fixed << std::setprecision(2) << SHA256_HardwareAverager.GetAverage() << " MB/s" << std::endl;
    std::cout << "SHA256 (Hardware): " << SHA256_Hardware_Hash << std::endl << std::endl;
    
    // MD5
    std::cout << "***********************************************************" << std::endl;
    std::cout << "MD5 (Software): " << std::fixed << std::setprecision(2) << MD5_SoftwareAverager.GetAverage() << " MB/s" << std::endl;
    std::cout << "MD5 (Software): " << MD5_Software_Hash << std::endl << std::endl << std::endl;
    
    // CRC16
    std::cout << "***********************************************************" << std::endl;
    std::cout << "CRC16 (Software): " << std::fixed << std::setprecision(2) << CRC16_SoftwareAverager.GetAverage() << " MB/s" << std::endl;
    std::cout << "CRC16 (Software): " << CRC16_Software_Hash << std::endl << std::endl << std::endl;
    
//    std::cout << "***********************************************************" << std::endl;
//    std::cout << "CRC16 (Hardware): " << std::fixed << std::setprecision(2) << CRC32averager.GetAverage() << " MB/s" << std::endl;
//    std::cout << "CRC16 (Hardware): " << CRC32_Hardware_Hash << std::endl << std::endl << std::endl;
//    std::cout << "CRC16 (Hardware) Int32: " << CRC32_Hardware_Hash_Int32 << std::endl << std::endl << std::endl;
//
    // CRC32
    std::cout << "***********************************************************" << std::endl;
    std::cout << "CRC32 (Software): " << std::fixed << std::setprecision(2) << CRC32_SoftwareAverager.GetAverage() << " MB/s" << std::endl;
    std::cout << "CRC32 (Software): " << CRC32_Software_Hash << std::endl << std::endl << std::endl;
    
    std::cout << "***********************************************************" << std::endl;
    std::cout << "CRC32 (Hardware): " << std::fixed << std::setprecision(2) << CRC32_HardwareAverager.GetAverage() << " MB/s" << std::endl;
    std::cout << "CRC32 (Hardware): " << CRC32_Hardware_Hash << std::endl << std::endl << std::endl;
    
    // CRC64
    std::cout << "***********************************************************" << std::endl;
    std::cout << "CRC64 (Software): " << std::fixed << std::setprecision(2) << CRC64_SoftwareAverager.GetAverage() << " MB/s" << std::endl;
    std::cout << "CRC64 (Software): " << CRC64_Software_Hash << std::endl << std::endl << std::endl;
    
//    std::cout << "***********************************************************" << std::endl;
//    std::cout << "CRC64 (Hardware): " << std::fixed << std::setprecision(2) << CRC64_HardwareAverager.GetAverage() << " MB/s" << std::endl;
//    std::cout << "CRC64 (Hardware): " << CRC64_Hardware_Hash << std::endl << std::endl << std::endl;
    
#ifndef TEST_BIG_DATA
    // Check generated hashes for testString
    assert(testStringHashSHA224_expected == SHA224_Software_Hash);
    assert(testStringHashSHA224_expected == SHA224_Hardware_Hash);
    
    assert(testStringHashSHA256_expected == SHA256_Software_Hash);
    assert(testStringHashSHA256_expected == SHA256_Hardware_Hash);
    
    assert(testStringHashMD5_expected == MD5_Software_Hash);
    
    assert(testStringHashCRC16_expected == CRC16_Software_Hash);
    //assert(testStringHashCRC16_expected == CRC16_Hardware_Hash);
    
    assert(testStringHashCRC32_expected == CRC32_Software_Hash);
    assert(testStringHashCRC32_expected == CRC32_Hardware_Hash);
    
    assert(testStringHashCRC64_expected == CRC64_Software_Hash);
//    assert(testStringHashCRC64_expected == CRC64_Hardware_Hash);
#endif
}

// ***************************************************
// Print measured speed and computed hashes
void PrintPredefInfo()
{
    // ***************************************************
    // Architecture
#ifdef HASH_PREDEF_ARCH_ARM_AVAILABLE
    std::cout << "ARM architecture. Version: " << HASH_PREDEF_VERSION_NUMBER_MAJOR(HASH_PREDEF_ARCH_ARM) << "." << HASH_PREDEF_VERSION_NUMBER_MINOR(HASH_PREDEF_ARCH_ARM) << std::endl;
#elif defined(HASH_PREDEF_ARCH_X86_32_AVAILABLE)
    std::cout << "x86_32 architecture" << std::endl;
#elif defined(HASH_PREDEF_ARCH_X86_64_AVAILABLE)
    std::cout << "x86_64 architecture" << std::endl;
#endif
    
    // ***************************************************
    // Endianness
#ifdef HM_BIG_ENDIAN
    std::cout << "Big endian" << std::endl;
#elif defined(HM_LITTLE_ENDIAN)
    std::cout << "Little endian" << std::endl;
#endif
    
    // ***************************************************
    // Check for SIMD
#ifdef HASH_PREDEF_HW_SIMD_AVAILABLE
    std::cout << "SIMD detected: ";
#endif
    
#if HASH_PREDEF_HW_SIMD_ARM // ARM SIMD
    std::cout << "ARM SIMD. Version: " << HASH_PREDEF_VERSION_NUMBER_MAJOR(HASH_PREDEF_HW_SIMD_ARM) << "." << HASH_PREDEF_VERSION_NUMBER_MINOR(HASH_PREDEF_HW_SIMD_ARM) << std::endl;
#elif HASH_PREDEF_HW_SIMD_X86 || HASH_PREDEF_HW_SIMD_X86_AMD // X86 SIMD (AMD)
    std::cout << "x86 SIMD. Version: " << HASH_PREDEF_VERSION_NUMBER_MAJOR(HASH_PREDEF_HW_SIMD_X86) << "." << HASH_PREDEF_VERSION_NUMBER_MINOR(HASH_PREDEF_HW_SIMD_X86) << std::endl;
    std::cout << "x86 SIMD AMD. Version: " << HASH_PREDEF_VERSION_NUMBER_MAJOR(HASH_PREDEF_HW_SIMD_X86_AMD) << "." << HASH_PREDEF_VERSION_NUMBER_MINOR(HASH_PREDEF_HW_SIMD_X86_AMD) << std::endl;
#endif
    
    std::cout << std::endl;
}

int main()
{
//    Hasher<CRC16, SOFTWARE> crc16_software;
//    crc16_software.Update("123");
//    //crc16_software.Update("456");
//    std::cout << "CRC16 two step (software): " << Utils::HashToHexString(crc16_software.End()) << std::endl;
    
    
//    Hasher<CRC64, SOFTWARE> crc64_software;
//    crc64_software.Update("123");
//    //crc64_software.Update("456");
//    std::cout << "CRC64 two step (software): " << Utils::HashToHexString(crc64_software.End()) << std::endl;
    
    
    
    
    // ***************************************************
    // Print startup header
    PrintStartupHeader();
 
    // Print predef infos
    PrintPredefInfo();
    
    // Fill big data with random values
#ifdef TEST_BIG_DATA
    PrepareBigData();
#endif
    
    // ***************************************************
    // Benchmark
    std::cout << "Running benchmark..." << std::flush;
    
    picobench::runner runner(0); // Use 0 as the RNG seed
    
    runner.set_default_samples(2);
    //runner.set_default_samples(1);
    
    //runner.set_default_state_iterations({1, 2, 8, 32, 64});
    runner.set_default_state_iterations({1, 2, 4, 8});
    //runner.set_default_state_iterations({1, 2});
    
    runner.run_benchmarks();
    
    std::cout << "Done!" << std::endl << std::endl;
    
    // ***************************************************
    // Generate report
    const auto report = runner.generate_report();
    report.to_text(std::cout);
    //report.to_text_concise(std::cout); // No iterations breakdown
    
    delete bigData;
    
    // ***************************************************
    // Print speed and hashes
    PrintSpeedAndHash();
    
    return 0;
}
