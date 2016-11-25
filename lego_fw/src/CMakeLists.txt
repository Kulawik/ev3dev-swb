cmake_minimum_required(VERSION 2.8)

set(EV3DEV_PLATFORM "EV3" CACHE STRING "Target ev3dev platform (EV3/RPI)")
set_property(CACHE EV3DEV_PLATFORM PROPERTY STRINGS "EV3" "RPI")

include_directories(${PROJECT_SOURCE_DIR}/src)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_library(ev3dev STATIC ev3dev.cpp)

function(add_ev3_executable target sources)
    add_executable(${target} ${sources})
    target_link_libraries(${target} ev3dev pthread)
endfunction()

add_ev3_executable(LoggerTest Logger_test.cpp)

