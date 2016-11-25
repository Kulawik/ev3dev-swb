find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)

include_directories (${lego-framework_SOURCE_DIR}/src
                     ${Boost_INCLUDE_DIRS}
                     )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
add_definitions (-DBOOST_TEST_DYN_LINK)


add_library(test_links INTERFACE)


add_executable (TestState test_state.cpp)

target_link_libraries(test_links
    INTERFACE
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
target_link_libraries (TestState test_links)

add_test (NAME TestState COMMAND TestState)

