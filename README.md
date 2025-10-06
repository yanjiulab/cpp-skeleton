# Cpp Skeleton

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Version](https://img.shields.io/badge/version-0.0.1-green.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B-purple.svg)
![Header Only](https://img.shields.io/badge/style-header--only-ff69b4.svg)
![OS](https://img.shields.io/badge/OS-Linux-blueviolet.svg)

[中文版本 (Chinese Version)](README_zh.md)

> Give you a starting point for your c++ project.

cpp-skeleton is a project skeleton for general applications and libraries.

For this project, we will **avoid reinventing the wheel as much as possible**. Instead, we will select open-source libraries that align with the author’s preferences, combine them into a framework, and enable rapid setup during project development. The following are the characteristics of this project:

- **Prohibition of mixing C/C++ features**: This project template can be used for C/C++ development. Although C++ is compatible with C, mixing C and C++ is not recommended. When using C++, pure C++ features should be adopted, unless there is a C library that must be used.
- **Built with `cmake`**: It offers good cross-platform support and popularity.
- **Use of `header-only` dependency libraries**: All of these libraries are placed in the `3rd` directory. This approach eliminates the need to configure library environments such as linking and dependencies, which is particularly convenient in scenarios where:
  - The code needs to be moved frequently;
  - The runtime environment has no internet access or insufficient permissions to install libraries;
  - Cross-compilation and deployment on different platforms are required.
- **Adoption of widely used libraries**: Some libraries have essentially become de facto "standard libraries". Learning to use these libraries provides strong universality.
- **Use of C++ 20**: Development will primarily rely on the `coroutine` model.

## 3rd lib

lib|standard|desc
:---:|:---:|---
[asio (non-Boost)](https://think-async.com/Asio/asio-1.30.2/doc/)|11/14/17/20|Asio is a cross-platform C++ library for network and low-level I/O programming that provides developers with a consistent asynchronous model using a modern C++ approach.
[CLI11](https://github.com/CLIUtils/CLI11)|11|CLI11 is a command line parser for C++11 and beyond that provides a rich feature set with a simple and intuitive interface.
[json](https://github.com/nlohmann/json)|11|JSON for Modern C++
[toml11](https://github.com/ToruNiina/toml11)|11/14/17/20|TOML for Modern C++
[cli](https://github.com/daniele77/cli)|14|A library for interactive command line interfaces in modern C++
[spdlog](https://github.com/gabime/spdlog)|11|Fast C++ logging library. A forked version of structlog is actually used, which supports JSON-structured logging. If you don’t need this feature, you can use the original version directly.
[cinatra](https://github.com/qicosmos/cinatra)|20|modern c++(c++20), cross-platform, header-only, easy to use http framework
[backward-cpp](https://github.com/bombela/backward-cpp)|11|A beautiful stack trace pretty printer for C++
[tabulate](https://github.com/p-ranav/tabulate)|11|Table Maker for Modern C++
[sigslot](https://github.com/palacaze/sigslot)|14|A simple C++14 signal-slots implementation
[sqlite_orm](https://github.com/fnc12/sqlite_orm)|14/17/20|【TODO】SQLite ORM light header only library for modern C++
[cereal](https://github.com/USCiLab/cereal)|11|【TODO】
[frozen](https://github.com/serge-sans-paille/frozen)|14|【TODO】a header-only, constexpr alternative to gperf for C++14 users
catch2|【TODO】|【TODO】
redis|【TODO】|【TODO】
flatbuffers|【TODO】|【TODO】

**If you have recommendations for useful libraries, feel free to open an issue!**

## Project structure

First, we should choose an appropriate way to organize the project code.

```bash
.
├── 3rd                 #
│   ├── include         # Header Files (each library has an independent directory)
│   │   ├── ...         # Dependency A
│   │   └── ...         # Dependency B
│   └── lib             # Library Files
├── build               # Build Directory / Distribution Directory
├── cmake               # CMake Tools
│   └── toolchain       # 
├── CMakeLists.txt      # 
├── config.h            
├── config.h.in         
├── doc                 
│   ├── CHANGELOG.md    
│   └── USAGE.md        
├── etc                 
├── include             # This project generates header files for the external API of the lib. It will be empty if it is a pure EXE project.
├── lib                 # This project generates lib files. It will be empty if it is a pure EXE project.
├── examples            # For 3rd lib
├── README.md           
├── scripts             
├── src                 
│   ├── app             
│   ├── cppdemo         
├── test                
├── tmp                 
└── tools               
```

Key points:

- Header files are placed in the `3rd/include` directory and organize the header files as much as possible in accordance with the original header file structure of the library.
- All CMake build directories shall start with `build`.
- If the program needs to define values that change with project releases (such as version numbers), use the `config.h.in` file template, and then include `config.h` in the code.
- The project documentation must include at least `doc/USAGE.md` and `doc/CHANGELOG.md`, as these are extremely useful.
- Configuration files such as `xx.json` or `xx.toml` are placed in the `etc` directory.
- If the project is a library project, the exported header files need to be placed in the include directory, and the library files in the lib directory. If it is an executable project, these two directories should be left empty.
- In the `src` directory, each subdirectory functions as a "module", and a module can generate either an executable file or a library file.

## Build

```shell
mkdir build; cd build
cmake ..
cmake --build . # or make <target>
```

### Cross Compilation

During cross-compilation, the project has built-in cross-compilation toolchain configurations for ARM and AArch64. If you use a different cross-compilation toolchain, simply modify the TOOLCHAIN_PATH (variable) and it will work.

```shell
mkdir build-arm; cd build-arm
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain/arm-linux-gnueabihf.cmake ..
# cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain/aarch64-linux-gnu.cmake ..
# ...
cmake --build .
```

## Release

```bash
build
├── bin
│   ├── cppdemo       # execute file
│   └── etc           # copy from ./etc
├── doc
│   ├── CHANGELOG.md  # copy from ./doc/CHANGELOG.md
│   └── USAGE.md      # copy from ./doc/USAGE.md
├── include
├── lib
└── etc               # copy from ./etc, same as bin/etc
```

## C++ standard

TODO

## Examples

### lynx

[lynx](doc/lynx/lynx.md) is a server daemon. It includes the following parts:

- app configuration
- daemonization
- log utils
- repl server
- timers
- signals
- udp/tcp server
- http server/client
- rest api server
- database
- rpc server/client

TODO
