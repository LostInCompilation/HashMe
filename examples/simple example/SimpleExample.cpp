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

// Benchmarking lib
#define PICOBENCH_IMPLEMENT
#include "picobench.h"

// HashMe lib
#include "HashMe.hpp"

using namespace HashMe;

// ***************************************************
// Test data
const uint64_t bigDataSize = 500 * 1024 * 1024; // 500MB
std::vector<uint8_t> bigData(bigDataSize);
const std::string testString = "123";

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
// SHA256 software implementation
void SHA256_Software(picobench::state& s)
{
    Hasher<SHA256, SOFTWARE> hasher;
    std::vector<uint8_t> hashResult;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Initialize();
        
        //hasher.Update(bigData.data(), bigData.size());
        hasher.Update(bigData);
        //hasher.Update(testString);
        
        hashResult = hasher.End();
        
        s.stop_timer();
    }
    
    // Print hash
    std::cout << "SHA256 (Software): " << HashToHexString(hashResult) << std::endl;
    
    // Print speed
    const double speed = static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0);
    std::cout << "Speed (Software): " << std::fixed << speed << " MB/s" << std::endl << std::endl;
}
PICOBENCH(SHA256_Software);

// ***************************************************
// SHA256 hardware implementation
void SHA256_Hardware(picobench::state& s)
{
    Hasher<SHA256, HARDWARE> hasher;
    std::vector<uint8_t> hashResult;
    
    for(int32_t iterations = 0; iterations < s.iterations(); iterations++)
    {
        s.start_timer();
        
        hasher.Initialize();
        
        //hasher.Update(bigData.data(), bigData.size());
        hasher.Update(bigData);
        //hasher.Update(testString);
        
        hashResult = hasher.End();
        
        s.stop_timer();
    }
    
    // Print hash
    std::cout << "SHA256 (Hardware): " << HashToHexString(hashResult) << std::endl;
    
    // Print speed
    const double speed = static_cast<double>(bigDataSize / 1024.0 / 1024.0) / (s.duration_ns() / 1000.0 / 1000.0 / 1000.0);
    std::cout << "Speed (Hardware): " << std::fixed << speed << " MB/s" << std::endl << std::endl;
}
PICOBENCH(SHA256_Hardware);

// ***************************************************
// Fill big data with random values
void PrepareBigData()
{
    std::cout << "Generating random big data...";
    
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    
    for (uint64_t i = 0; i < bigDataSize; i++)
        bigData[i] = dist(mt);
    
    std::cout << "Done!" << std::endl << std::endl;
}

int main()
{
    PrintStartupHeader();
    
    // Fill big data with random values
    PrepareBigData();
    
    // ***************************************************
    // Benchmark SHA256
    picobench::runner runner(0); // Use 0 as seed
    runner.set_default_samples(1);
    runner.set_default_state_iterations({1});
    runner.run_benchmarks();
    
    // ***************************************************
    // Genereate report
    const auto report = runner.generate_report();
    report.to_text(std::cout);
    //report.to_text_concise(std::cout); // No iterations breakdown
    
    // ***************************************************
    // Use software implementation
    //SHA256_Software();
    
    // Use hardware implementation
    //SHA256_Hardware();
    
    return 0;
}
