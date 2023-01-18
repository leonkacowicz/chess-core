# What is this?
This package is a one of a series packages intended to 
support the development of general purpose chess engines and tools.

This package implements the foundational code that implements the basic chess
abstractions. It also implements a fast move generation function using techniques
like magic-bitboards.

It also implements zobrist hashing and fen encoding/decoding.  

It contains a comprehensive test suite in the `test/` directory which can be used
to infer how the classes/functions should be used.

Most of the techniques implemented in this package are explained in more detail in https://www.chessprogramming.org

# How do I build it?

This package uses CMake as a build-tool. In theory, it requires CMake 3.5, 
but I haven't tested it with previous versions of CMake. If you manage to
build it with an older version of CMake, let me know or send a pull request
decreasing the required version on line 1 of `/CMakeLists.txt`

To build it:
```sh
mkdir build
cd build
cmake ..
make
```

# Testing

After building, an executable at `build/test/chess/chess_test` should be generated.
All tests specified in `/tests/` should be invoked by this executable.

# How do I import it in my CMake project?

You can add a file called `chess-core.cmake` to your project with the following contents:

```cmake
include(ExternalProject)
ExternalProject_Add(
        chess-core-external
        URL https://github.com/leonkacowicz/chess-core/archive/refs/heads/main.zip
        PREFIX ${CMAKE_BINARY_DIR}/chess-core-external
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON)

add_library(libchess-core IMPORTED STATIC GLOBAL)
add_dependencies(libchess-core chess-core-external)
include_directories("${CMAKE_BINARY_DIR}/chess-core-external/src/chess-core-external/src/chess/include")
set_target_properties(libchess-core PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/chess-core-external/src/chess-core-external/"
        IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/chess-core-external/src/chess-core-external-build/src/chess/libchess.a"
        )
```

Then add the following to your CMakeLists.txt:
```cmake
include(chess-core.cmake)
target_link_libraries(YOUR_TARGET libchess-core)
```