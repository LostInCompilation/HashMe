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
// Test data
const uint64_t bigDataSize = 300 * 1024 * 1024; // 300MB
std::vector<uint8_t>* bigData = nullptr; // Allocate on heap to prevent "Compiler out of heap space error" in VisualStudio

const std::string testString = "123";
//const std::string testString = "12345678901234567890123456789012345678901234567890123456789012345"; // 65 chars

// Hash results for checking
const std::string testStringHashSHA256_expected = "a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3";
const std::string testStringHashMD5_expected = "202cb962ac59075b964b07152d234b70";

// Testing mode
#define TEST_BIG_DATA // Use big data for test

// ***************************************************
// SHA256 software implementation
double SHA256_Software_Speed = 0.0;
std::string SHA256_Software_Hash = "";
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
    SHA256_Software_Speed += static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0);
}
PICOBENCH(SHA256_Software);

// ***************************************************
// SHA256 hardware implementation
double SHA256_Hardware_Speed = 0.0;
std::string SHA256_Hardware_Hash = "";
/*void SHA256_Hardware(picobench::state& s)
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
    SHA256_Hardware_Speed += static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0);
}
PICOBENCH(SHA256_Hardware).baseline();*/

// ***************************************************
// MD5 software implementation
double MD5_Software_Speed = 0.0;
std::string MD5_Software_Hash = "";
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
    MD5_Software_Speed += static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0);
}
PICOBENCH(MD5_Software);

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
    std::cout << "SHA256 (Software): " << std::fixed << std::setprecision(2) << SHA256_Software_Speed / 8.0 << " MB/s" << std::endl;
    std::cout << "SHA256 (Software): " << SHA256_Software_Hash << std::endl << std::endl;
    
    std::cout << "SHA256 (Hardware): " << std::fixed << std::setprecision(2) << SHA256_Hardware_Speed / 8.0 << " MB/s" << std::endl;
    std::cout << "SHA256 (Hardware): " << SHA256_Hardware_Hash << std::endl << std::endl;
    
    std::cout << "MD5 (Software): " << std::fixed << std::setprecision(2) << MD5_Software_Speed / 8.0 << " MB/s" << std::endl;
    std::cout << "MD5 (Software): " << MD5_Software_Hash << std::endl << std::endl << std::endl;
    
#ifndef TEST_BIG_DATA
    // Check generated hashes for testString
    assert(testStringHashSHA256_expected == SHA256_Software_Hash);
    assert(testStringHashMD5_expected == MD5_Software_Hash);
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
    //runner.set_default_state_iterations({1, 2, 8, 32, 64});
    runner.set_default_state_iterations({1, 2, 4, 8});
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
