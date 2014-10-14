cmake_minimum_required(VERSION 2.8.11)
project(deuceclient-demo)

find_package(deuceclient REQUIRED)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-Wall -std=c++0x")
endif()

include_directories(${deuceclient_INCLUDE_DIRS})

file(GLOB demo_srcs *.cc)

foreach(demo_src ${demo_srcs})
	get_filename_component(demo ${demo_src} NAME_WE)
	add_executable(${demo} ${demo_src})
	target_link_libraries(${demo} deuceclient)
endforeach()
