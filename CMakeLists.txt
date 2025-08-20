cmake_minimum_required(VERSION 3.16)
project(Sol2QtApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

# Find Lua
find_package(PkgConfig REQUIRED)
find_library(LUA_LIBRARY lua REQUIRED)

qt6_standard_project_setup()

qt6_add_executable(Sol2QtApp
	main.cpp
	sol2qtmainwindow.cpp
	LuaControlledWidget.hpp
	LuaWindowFactory.hpp
        LuaChartWidget.hpp
	GenericDataTableWidget.hpp
	sol2qtmainwindow.hpp
)

# Include Sol2 headers (header-only library)
target_include_directories(Sol2QtApp PRIVATE 
    ${CMAKE_SOURCE_DIR}/sol2/include  # Sol2 headers
    /opt/homebrew/include                      # Lua headers via Homebrew
)

target_compile_options(Sol2QtApp PRIVATE
    -Wno-deprecated-declarations
    -g
    )
    
target_link_libraries(Sol2QtApp PRIVATE
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Charts 
    ${LUA_LIBRARY}
)

if(APPLE)
    find_library(ACCELERATE_FRAMEWORK Accelerate REQUIRED)
    message(STATUS "Found Accelerate Framework: ${ACCELERATE_FRAMEWORK}")
    target_link_libraries(Sol2QtApp PRIVATE ${ACCELERATE_FRAMEWORK})
    target_compile_definitions(Sol2QtApp PRIVATE USE_ACCELERATE_FRAMEWORK=1)
endif()

target_compile_definitions(Sol2QtApp PRIVATE
    SOL_ALL_SAFETIES_ON=1
)
