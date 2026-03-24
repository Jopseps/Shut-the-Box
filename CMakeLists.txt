cmake_minimum_required(VERSION 3.10)
project(ShutTheBox VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create the executable named 'shuttb'
add_executable(shuttb main.cpp)

# Install location (standard bin directory)
install(TARGETS shuttb DESTINATION bin)
