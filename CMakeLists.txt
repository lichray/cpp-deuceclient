cmake_minimum_required(VERSION 2.8)
project(deuceclient)
enable_testing()

set(CMAKE_MODULE_PATH cmake)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY tests)

find_package(CURL REQUIRED)

include_directories(${CURL_INCLUDE_DIR})
include_directories(3rdparty/include)

file(GLOB restclient_srcs 3rdparty/src/restclient/*.cpp)
file(GLOB tests_srcs tests/*.cc)

add_library(restclient STATIC ${restclient_srcs})
target_link_libraries(restclient ${CURL_LIBRARY})

foreach(test_src ${tests_srcs})
	get_filename_component(test_suite ${test_src} NAME_WE)
	add_executable(${test_suite} ${test_src})
	target_link_libraries(${test_suite} restclient)
	add_test(${test_suite} tests/${test_suite})
endforeach()
