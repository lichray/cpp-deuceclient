cmake_minimum_required(VERSION 2.8)
project(deuceclient)
enable_testing()

include(CTest)
include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY tests)

find_package(CURL REQUIRED)

if(BUILD_TESTING)
	find_package(Git REQUIRED)
	find_package(PythonInterp REQUIRED)
endif()

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

	ExternalProject_Add(
	    deuce
	    GIT_REPOSITORY https://github.com/rackerlabs/deuce.git
	    TIMEOUT 5
	    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
	    CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} -m virtualenv venv
	    BUILD_COMMAND ""
	    INSTALL_COMMAND sh -c
	        ". venv/bin/activate$<SEMICOLON> pip install -e ../deuce")
endif()
