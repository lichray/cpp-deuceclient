cmake_minimum_required(VERSION 2.8)
project(deuceclient)
enable_testing()

include(CTest)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY tests)

find_package(CURL REQUIRED)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-Wall -std=c++0x")
endif()

include_directories(${CURL_INCLUDE_DIRS})
include_directories(3rdparty/include)
include_directories(include)

file(GLOB restclient_srcs 3rdparty/src/restclient/*.cpp)
file(GLOB deuceclient_srcs src/*.cc)
file(GLOB tests_srcs tests/*.cc)

add_library(restclient STATIC ${restclient_srcs})
target_link_libraries(restclient ${CURL_LIBRARIES})

add_library(deuceclient STATIC ${deuceclient_srcs})

if(BUILD_TESTING)
	foreach(test_src ${tests_srcs})
		get_filename_component(test_suite ${test_src} NAME_WE)
		add_executable(${test_suite} ${test_src})
		target_link_libraries(${test_suite} restclient deuceclient)
		add_test(${test_suite} tests/${test_suite})
	endforeach()
endif()
