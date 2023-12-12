cmake_minimum_required(VERSION 3.15)

project(SpectroSpatialSynth)

######################### Compiler Settings ########################
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

########################## Basic Settings ##########################
## First, we'll add the CMake folder, incase we'll need to find_package later:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")


######################### Compile Commands #########################
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)


######################## Platform settings #########################
## Minimum MacOS target, set globally

if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0 CACHE STRING "Minimum OS X deployment version" FORCE)

    #code signing to run on an iOS device:
#    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer" CACHE STRING "" FORCE)
#    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "XXXXXXXXXX" CACHE STRING "" FORCE)
else()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version" FORCE)
endif()

option(UniversalBinary "Build universal binary for mac" OFF)

if (UniversalBinary)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
endif()

## Static linking in Windows
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


##########################      JUCE      ##########################
## Three different ways of adding JUCE to the project. 
## 1.) Easy, but needs space 
## 2.) Installation results in fast builds
## 3.) Use the package manager, if you are not interested in downloading or installing 
##     JUCE manually 

## 1.) Adding JUCE locally by copying the whole JUCE Folder into the project folder
#add_subdirectory(JUCE)  

## 2.) Using your installation of JUCE. 
## Make sure the JUCE install path is in the shell PATH variable 
## or add it manually in this file: set(CMAKE_PREFIX_PATH "path/to/JUCE/install")
#find_package(JUCE CONFIG REQUIRED)

## 3.) Using the CPM package manager, see CMake/Findjuce.cmake
find_package(juce REQUIRED)


########################## Custom Modules ##########################
add_subdirectory(Modules)

########################### KFR library ############################
include_directories(kfr/include/kfr)

########################## Project Folder ##########################
add_subdirectory(Plugin)

add_subdirectory(kfr)

## Uncomment for benchmarks
#add_subdirectory(SNR)
