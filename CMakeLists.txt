cmake_minimum_required(VERSION 2.8.11)
project(deuceclient)
enable_testing()

include(CTest)
include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY tests)

find_package(httpverbs REQUIRED)
find_package(OpenSSL REQUIRED)

if(BUILD_TESTING)
	find_package(Git REQUIRED)
	find_package(PythonInterp 3.3 REQUIRED)
endif()

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-Wall -std=c++0x")
endif()

include_directories(3rdparty/include)
include_directories(include)

file(GLOB deuceclient_srcs src/*.cc)
file(GLOB tests_srcs tests/*.cc)

set(get_pip_py ${CMAKE_SOURCE_DIR}/files/get-pip.py)

add_library(deuceclient STATIC ${deuceclient_srcs})

target_include_directories(deuceclient PUBLIC ${httpverbs_INCLUDE_DIRS})
target_include_directories(deuceclient PUBLIC ${OPENSSL_INCLUDE_DIR})

target_link_libraries(deuceclient $<TARGET_PROPERTY:httpverbs,LOCATION>
    ${httpverbs_LINK_LIBRARIES})
target_link_libraries(deuceclient ${OPENSSL_CRYPTO_LIBRARIES})

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
	    UPDATE_COMMAND git pull
	    PATCH_COMMAND sh -c
	        "git checkout ."
	    CONFIGURE_COMMAND sh -c
	        "[ -e venv ] || ${PYTHON_EXECUTABLE} -m venv venv"
	    BUILD_COMMAND sh -c
	        ". venv/bin/activate$<SEMICOLON> python ${get_pip_py}"
	    INSTALL_COMMAND sh -c
	        ". venv/bin/activate$<SEMICOLON> pip install -e ../deuce")
endif()

file(GLOB deuceclient_hdrs include/deuceclient/*.h)
get_target_property(_inc_dir httpverbs INTERFACE_INCLUDE_DIRECTORIES)
configure_file(deuceclient-config.cmake.in deuceclient-config.cmake)
export(PACKAGE deuceclient)

install(FILES ${deuceclient_hdrs} DESTINATION include/deuceclient)
install(TARGETS deuceclient EXPORT deuceclient-targets ARCHIVE DESTINATION lib)
install(EXPORT deuceclient-targets DESTINATION lib/cmake/deuceclient)
install(FILES deuceclient-config.cmake DESTINATION lib/cmake/deuceclient)
