cmake_minimum_required(VERSION 2.8)
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to RelWithDebInfo")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type")
endif()

project(lego-framework)

set(EV3DEV_PLATFORM "EV3" CACHE STRING "Target ev3dev platform (EV3/RPI)")
set_property(CACHE EV3DEV_PLATFORM PROPERTY STRINGS "EV3" "RPI")

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
