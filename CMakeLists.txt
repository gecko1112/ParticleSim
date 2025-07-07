cmake_minimum_required(VERSION 3.10)
project(ParticleSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add all source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

# executable
add_executable(ParticleSim ${SOURCES})

# SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
target_link_libraries(ParticleSim sfml-graphics sfml-window sfml-system)

# include directories
target_include_directories(ParticleSim PRIVATE src utils)
