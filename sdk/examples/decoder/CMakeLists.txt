cmake_minimum_required(VERSION 3.10)

# Project name and version
project(LumasonicDecoderExample 
        VERSION 1.0.0
        DESCRIPTION "Lumasonic Decoder Example"
        LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add subdirectories
add_subdirectory(../../lib lib)
add_subdirectory(app)

# Set the executable as the start up project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT lsdecoder)