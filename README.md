# HashMe - Hardware accelerated hash library for C++ (SIMD intrinsics) - WIP

![Version](https://img.shields.io/badge/Version-V0.1-brightgreen?style=flat&&logo=framework) ![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS%20-blue?style=flat&&logo=buffer) ![CPU](https://img.shields.io/badge/CPU-x86%20%7C%20ARMv8-red?style=flat&&logo=qmk) ![CPP](https://img.shields.io/badge/Language-C++20-orange?style=flat&&logo=C%2b%2b)

<h3 align="center">Hardware acceleration for:</h3>
<p align="center" width="100%">
<img src="/logo/intel_logo.svg" alt="Intel logo"/>
![arm_logo](https://github.com/LostInCompilation/HashMe/blob/main/logo/arm_logo.svg)
</p>

## Contents
- [Description](#description)
    - [Supported hash algorithms](#supported-hash-algorithms)
    - [Additional features](#additional-features)
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
Currently WIP!

A simple to use, modern and free C++20 Hash library which supports **hardware accelerated** (SIMD intrinsics) algorithms for **x86 and ARMv8 (like AppleSilicon)**. The static library provides very easy integration in your projects and ultra high speeds for the most common hash algorithms like MD5, SHA1, SHA2, SHA256, SHA512 and many more.
CMake build environment is supported, as well as generating projects for VisualStudio or Xcode.

### Supported hash algorithms
The following hash algorithms are supported:
Algorithm | Supported | x86 Hardware accelerated |
| -- | --------- | --------- |
| MD5 | ✅ Yes | ❌ ⚠️ |

### Additional features
TODO - BASE64?

## Getting the library
To download the library simply clone the repository:
~~~
git clone https://github.com/LostInCompilation/HashMe
~~~

## Compiling
Compilation of the library and examples can be done using CMake and make, or projects for VisualStudio and Xcode can be generated. Small Shell scripts are included for compiling and generating projects.

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

TODO

### Hardware acceleration
TODO

## Included Examples
TODO

