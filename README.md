# HashMe - Hardware accelerated hash library for C++ (SIMD intrinsics)

![Version](https://img.shields.io/badge/Version-V0.1-brightgreen?style=flat&&logo=framework) ![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS%20-blue?style=flat&&logo=buffer) ![CPU](https://img.shields.io/badge/CPU-x86%20%7C%20ARMv8-red?style=flat&&logo=qmk) ![CPP](https://img.shields.io/badge/Language-C++20-orange?style=flat&&logo=C%2b%2b)

<h3 align="center">Hardware acceleration for</h3>
<p align="center" width="100%">
<img src="/img/intel_logo.svg" alt="Intel logo"/>
<img src="/img/arm_logo.svg" alt="ARM logo"/>
</p>

## Contents
- [Description](#description)
    - [Supported hash algorithms](#supported-hash-algorithms)
    - [Additional algorithms](#additional-algorithms)
    - [Performance and Benchmarks](#performance-and-benchmarks)
        - [ARMv8 (M1 Pro AppleSilicon)](#armv8-m1-pro-applesilicon)
        - [x86_64 (Intel)](#x86-64-intel)
- [Getting the library](#getting-the-library)
- [Compiling](#compiling)
    - [CMake](#cmake)
    - [Visual Studio](#visual-studio)
    - [Xcode](#xcode)
- [How to use it](#how-to-use-it)
    - [Hardware acceleration](#hardware-acceleration)
- [Included examples](#included-examples)

*See also: [License (zlib)](LICENSE.md)*

## Description
**Library is currently WIP! Will be updated frequently.**

A simple to use, fast and modern C++20 Hash library which supports **hardware accelerated** algorithms (SIMD intrinsics) for **x86 and ARMv8 (like AppleSilicon)**. See [Performance and Benchmarks](#performance-and-benchmarks) for speeds. The static library provides very easy integration in your projects and ultra high speeds for the most common hash algorithms like **MD5, MD6, SHA0, SHA1, SHA2 (SHA224, SHA256, SHA384, SHA512), SHA3 (SHA3-224, SHA3-256, SHA3-384, SHA3-512), CRC16, CRC32, CRC64, CRC128** and others. See [Supported hash algorithms](#supported-hash-algorithms) for details.
CMake build environment is supported, as well as generating projects for VisualStudio or Xcode.

TODO WRITE: STREAMING FOR LARGE FILES

### Supported hash algorithms
#### CRC
Algorithm | Supported | Hardware acceleration (SIMD) |
| -- | --------- | --------- |
| CRC16 | ✅ Yes | ❌ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |
| CRC32 | ✅ Yes | ✅ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |
| CRC64 (ECMA) | ✅ Yes | ❌ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |

#### MD
Algorithm | Supported | Hardware acceleration (SIMD) |
| -- | --------- | --------- |
| MD5 | ✅ Yes | ❌ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |
| MD6 | ❌ WIP | ❌ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |

#### SHA
Algorithm | Supported | Hardware acceleration (SIMD) |
| -- | --------- | --------- |
| SHA1 | ❌ WIP | ❌ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |
| SHA224 | ✅ Yes | ✅ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;⚠️ x86 |
| SHA256 | ✅ Yes | ✅ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;⚠️ x86 |
| SHA384 | ✅ Yes | ❌ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |
| SHA512 | ✅ Yes | ✅ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |
| SHA512/224 | ❌ WIP | ❌ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |
| SHA512/256 | ❌ WIP | ❌ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |
| SHA3-512 | ❌ WIP | ❌ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |

*⚠️ = Work in progress*
*❌ = Not yet supported*

---

### Additional algorithms
Algorithm | Supported | Hardware acceleration |
| -- | --------- | --------- |
| Base64 | ❌ WIP | ❌ ARM&nbsp;&nbsp;\|&nbsp;&nbsp;❌ x86 |

*⚠️ = Work in progress*
*❌ = Not yet supported*

---

### Performance and Benchmarks

#### ARMv8 (M1 Pro AppleSilicon)
*Apple Clang, Release build, -O3 optimization*

#### CRC
Algorithm      |   Speed (MB/s)
---------------|--------:
CRC16 Software | 375.54 MB/s
CRC16 Hardware (SIMD) | xxx MB/s
CRC32 Software | 376.49 MB/s
CRC32 Hardware (SIMD) | 8031.77 MB/s
CRC64 Software | 378.56 MB/s
CRC64 Hardware (SIMD) | xxx MB/s

#### MD
Algorithm      |   Speed (MB/s)
---------------|--------:
MD5 Software | 538.12 MB/s
MD5 Hardware (SIMD) | xxx MB/s

#### SHA
Algorithm      |   Speed (MB/s)
---------------|--------:
SHA224 Software | 218.50 MB/s
SHA224 Hardware (SIMD) | 1944.76 MB/s
SHA256 Software | 217.76 MB/s
SHA256 Hardware (SIMD) | 1944.79 MB/s
SHA384 Software | 317.07 MB/s
SHA384 Hardware (SIMD) | xxx MB/s
SHA512 Software | 317.10 MB/s
SHA512 Hardware (SIMD) | xxx MB/s


#### x86_64 (Intel)
**TODO**

## Getting the library
To download the library simply clone the repository:
~~~
git clone https://github.com/LostInCompilation/HashMe
~~~

## Compiling
Compilation of the library and examples can be done using CMake and make. Additionally projects for VisualStudio and Xcode can be generated. Small Shell scripts for compiling and generating projects are included.

### CMake
TODO

### Visual Studio
TODO

### Xcode
TODO

## How to use it
Include the main header file of the library in your project:
```cpp
#include "HashMe.hpp"
```

LINKING

If you prefer, directly use the namespace for the library:
```cpp
using namespace HashMe;
```
A shorter namespace alias ```HM```is also available:
```cpp
using namespace HM;
```

TODO

### Hardware acceleration
TODO

## Included Examples
TODO

