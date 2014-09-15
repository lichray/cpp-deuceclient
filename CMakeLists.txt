cmake_minimum_required(VERSION 2.8)
project(deuceclient)
enable_testing()

include(CTest)
include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY tests)

find_package(httpverbs REQUIRED)

if(BUILD_TESTING)
	find_package(Git REQUIRED)
	find_package(PythonInterp 3.3 REQUIRED)
endif()

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-Wall -std=c++0x")
endif()

include_directories(${httpverbs_INCLUDE_DIRS})
include_directories(3rdparty/include)
include_directories(include)

file(GLOB restclient_srcs 3rdparty/src/restclient/*.cpp)
file(GLOB deuceclient_srcs src/*.cc)
file(GLOB tests_srcs tests/*.cc)

set(get_pip_py ${CMAKE_SOURCE_DIR}/files/get-pip.py)
set(patch_config_py ${CMAKE_SOURCE_DIR}/files/patch-config.py)

add_library(deuceclient STATIC ${deuceclient_srcs})
target_link_libraries(deuceclient httpverbs)

if(BUILD_TESTING)
	foreach(test_src ${tests_srcs})
		get_filename_component(test_suite ${test_src} NAME_WE)
		add_executable(${test_suite} ${test_src})
		target_link_libraries(${test_suite} deuceclient)
		add_test(${test_suite} tests/${test_suite})
	endforeach()

	ExternalProject_Add(
	    deuce
	    GIT_REPOSITORY https://github.com/rackerlabs/deuce.git
	    TIMEOUT 5
	    PATCH_COMMAND sh -c
	        "git checkout . && patch -p0 < ${patch_config_py}"
	    CONFIGURE_COMMAND sh -c
	        "[ -e venv ] || ${PYTHON_EXECUTABLE} -m venv venv"
	    BUILD_COMMAND sh -c
	        ". venv/bin/activate$<SEMICOLON> python ${get_pip_py}"
	    INSTALL_COMMAND sh -c
	        ". venv/bin/activate$<SEMICOLON> pip install -e ../deuce")
endif()
