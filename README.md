# HashMe - Hardware accelerated hash library for C++ (SIMD intrinsics) - WIP

![Version](https://img.shields.io/badge/Version-V0.1-brightgreen?style=flat&&logo=framework) ![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS%20-blue?style=flat&&logo=Buffer.sh) ![CPU](https://img.shields.io/badge/CPU-x86%20%7C%20ARMv8-red?style=flat&&logo=Platform.sh) ![CPP](https://img.shields.io/badge/Language-C++20-orange?style=flat&&logo=C%2b%2b)

### <center>Hardware acceleration for:</center>
<center>
<svg role="img" viewBox="0 1 24 24" width="120" height="80" fill="white" xmlns="http://www.w3.org/2000/svg"><title>Intel</title><path d="M20.42 7.345v9.18h1.651v-9.18zM0 7.475v1.737h1.737V7.474zm9.78.352v6.053c0 .513.044.945.13 1.292.087.34.235.618.44.828.203.21.475.359.803.451.334.093.754.136 1.255.136h.216v-1.533c-.24 0-.445-.012-.593-.037a.672.672 0 0 1-.39-.173.693.693 0 0 1-.173-.377 4.002 4.002 0 0 1-.037-.606v-2.182h1.193v-1.416h-1.193V7.827zm-3.505 2.312c-.396 0-.76.08-1.082.241-.327.161-.6.384-.822.668l-.087.117v-.902H2.658v6.256h1.639v-3.214c.018-.588.16-1.02.433-1.299.29-.297.642-.445 1.044-.445.476 0 .841.149 1.082.433.235.284.359.686.359 1.2v3.324h1.663V12.97c.006-.89-.229-1.595-.686-2.09-.458-.495-1.1-.742-1.917-.742zm10.065.006a3.252 3.252 0 0 0-2.306.946c-.29.29-.525.637-.692 1.033a3.145 3.145 0 0 0-.254 1.273c0 .452.08.878.241 1.274.161.395.39.742.674 1.032.284.29.637.526 1.045.693.408.173.86.26 1.342.26 1.397 0 2.262-.637 2.782-1.23l-1.187-.904c-.248.297-.841.699-1.583.699-.464 0-.847-.105-1.138-.321a1.588 1.588 0 0 1-.593-.872l-.019-.056h4.915v-.587c0-.451-.08-.872-.235-1.267a3.393 3.393 0 0 0-.661-1.033 3.013 3.013 0 0 0-1.02-.692 3.345 3.345 0 0 0-1.311-.248zm-16.297.118v6.256h1.651v-6.256zm16.278 1.286c1.132 0 1.664.797 1.664 1.255l-3.32.006c0-.458.525-1.255 1.656-1.261zm7.073 3.814a.606.606 0 0 0-.606.606.606.606 0 0 0 .606.606.606.606 0 0 0 .606-.606.606.606 0 0 0-.606-.606zm-.008.105a.5.5 0 0 1 .002 0 .5.5 0 0 1 .5.501.5.5 0 0 1-.5.5.5.5 0 0 1-.5-.5.5.5 0 0 1 .498-.5zm-.233.155v.699h.13v-.285h.093l.173.285h.136l-.18-.297a.191.191 0 0 0 .118-.056c.03-.03.05-.074.05-.136 0-.068-.02-.117-.063-.154-.037-.038-.105-.056-.185-.056zm.13.099h.154c.019 0 .037.006.056.012a.064.064 0 0 1 .037.031c.013.013.012.031.012.056a.124.124 0 0 1-.012.055.164.164 0 0 1-.037.031c-.019.006-.037.013-.056.013h-.154Z"/></svg><svg role="img" viewBox="0 0 24 24" width="120" height="80" fill="white" xmlns="http://www.w3.org/2000/svg"><title>Arm</title><path d="M5.419 8.534h1.614v6.911H5.419v-.72c-.71.822-1.573.933-2.07.933C1.218 15.658 0 13.882 0 11.985c0-2.253 1.542-3.633 3.37-3.633.507 0 1.4.132 2.049.984zm-3.765 3.491c0 1.198.751 2.202 1.918 2.202 1.015 0 1.959-.74 1.959-2.181 0-1.512-.934-2.233-1.959-2.233-1.167-.01-1.918.974-1.918 2.212zm7.297-3.49h1.613v.618a3 3 0 0 1 .67-.578c.314-.183.619-.233.984-.233.396 0 .822.06 1.269.324l-.66 1.462a1.432 1.432 0 0 0-.822-.244c-.345 0-.69.05-1.005.376-.446.477-.446 1.136-.446 1.593v3.582H8.94zm5.56 0h1.614v.639c.538-.66 1.177-.822 1.705-.822.72 0 1.4.345 1.786 1.015.579-.822 1.441-1.015 2.05-1.015.842 0 1.573.396 1.969 1.086.132.233.365.74.365 1.745v4.272h-1.614V11.65c0-.771-.08-1.086-.152-1.228-.101-.264-.345-.609-.923-.609-.396 0-.741.213-.954.508-.284.395-.315.984-.315 1.572v3.562H18.43V11.65c0-.771-.081-1.086-.152-1.228-.102-.264-.345-.609-.924-.609-.396 0-.74.213-.954.508-.284.395-.314.984-.314 1.572v3.562h-1.573z"/></svg>
</center>

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

