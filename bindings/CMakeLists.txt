cmake_minimum_required(VERSION 3.16)
project(lua_binding_generator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find libclang
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Try to find libclang via pkg-config first
pkg_check_modules(LIBCLANG QUIET libclang)

if(LIBCLANG_FOUND)
    message(STATUS "Found libclang via pkg-config")
    set(CLANG_LIBRARIES ${LIBCLANG_LIBRARIES})
    set(CLANG_INCLUDE_DIRS ${LIBCLANG_INCLUDE_DIRS})
else()
    # Fallback: try to find clang manually
    find_path(CLANG_INCLUDE_DIRS
        NAMES clang-c/Index.h
        PATHS
            /usr/include
            /usr/local/include
            /opt/homebrew/include
            /opt/homebrew/opt/llvm/include
            /usr/include/llvm-*
            /usr/include/clang-*
        PATH_SUFFIXES
            clang-c
    )

    find_library(CLANG_LIBRARIES
        NAMES clang libclang
        PATHS
            /usr/lib
            /usr/local/lib
            /opt/homebrew/lib
            /opt/homebrew/opt/llvm/lib
            /usr/lib/llvm-*
            /usr/lib/x86_64-linux-gnu
    )

    if(CLANG_INCLUDE_DIRS AND CLANG_LIBRARIES)
        message(STATUS "Found libclang manually")
        message(STATUS "  Include dirs: ${CLANG_INCLUDE_DIRS}")
        message(STATUS "  Libraries: ${CLANG_LIBRARIES}")
    else()
        message(FATAL_ERROR "Could not find libclang. Please install clang development packages.")
    endif()
endif()

# Create the executable
add_executable(ast_extractor ast_extractor.cpp)

# Set up include directories and libraries
target_include_directories(ast_extractor PRIVATE ${CLANG_INCLUDE_DIRS})
target_link_libraries(ast_extractor ${CLANG_LIBRARIES})

# Platform-specific settings
if(APPLE)
    # macOS might need additional frameworks
    find_library(FOUNDATION_FRAMEWORK Foundation)
    if(FOUNDATION_FRAMEWORK)
        target_link_libraries(ast_extractor ${FOUNDATION_FRAMEWORK})
    endif()
endif()

# Install the executable
install(TARGETS ast_extractor
    RUNTIME DESTINATION bin
)

# Create the executable
add_executable(binding_generator binding_generator.cpp)

# Set up include directories and libraries
target_include_directories(binding_generator PRIVATE ${CLANG_INCLUDE_DIRS})
target_link_libraries(binding_generator ${CLANG_LIBRARIES} nlohmann_json::nlohmann_json)

# Platform-specific settings
if(APPLE)
    # macOS might need additional frameworks
    find_library(FOUNDATION_FRAMEWORK Foundation)
    if(FOUNDATION_FRAMEWORK)
        target_link_libraries(binding_generator ${FOUNDATION_FRAMEWORK})
    endif()
endif()

# Install the executable
install(TARGETS binding_generator
    RUNTIME DESTINATION bin
)

# Print helpful information
message(STATUS "Build configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Clang include dirs: ${CLANG_INCLUDE_DIRS}")
message(STATUS "  Clang libraries: ${CLANG_LIBRARIES}")
