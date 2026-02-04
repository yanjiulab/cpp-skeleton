# TODO

1. The src directory contains all source code. src/cpp11 is the collection of source code modules for C++11, and the same applies to cpp14, cpp17 and cpp20. Each subdirectory under a source code module collection is a dedicated module source directory (e.g., src/cpp14/demo), with the same rule for all other collections. All modules are automatically collected in src/CMakeLists.txt.
2. Each module compiles to an executable or library: a module with main.cpp is an executable module, while one without is a library module.
3. Assuming the built modules are located in the build directory, the compiled output of the src/cpp14/demo module will be placed in build/cpp14/demo. Two build modes are supported:
    - Standard Mode: Executables (for executable modules) are placed in build/cpp14/demo/bin; configuration files (json/toml/yml/ini) in build/cpp14/demo/etc; documentation files (md) in build/cpp14/demo/doc; library files (for library modules) in build/cpp14/demo/lib; header files (for library modules) in build/cpp14/demo/include.
    - Flat Mode: All output files are placed directly in build/cpp14/demo.
4. All configuration files and documentation files are collected from the current module directory based on their file suffixes.
5. Please update all CMakeLists.txt files in accordance with the above specifications.

##

create modules based on src/cpp11/app11


- cpp11
  - app11: a demo using asio 11
  - demo11: a demo
  - spdlog_demo
  - CLI11_demo
  - json_demo
  - toml11_demo
  - tabulate_demo
  - threadpool_demo
  - queue_demo
- 14
  - app14
  - demo14
  - cli_demo
  - config_demo
  - sigslot_demo
- 17
  - app17
  - demo17
- 20
  - app20
  - demo20
  - lynx