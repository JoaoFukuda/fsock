cmake_minimum_required(VERSION 3.5)
include(GNUInstallDirs)

project(fsock LANGUAGES CXX)

add_library(fsock STATIC fsock.cpp)

set_target_properties(fsock PROPERTIES PUBLIC_HEADER "fsock.hpp")

install(TARGETS fsock
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

option(EXAMPLES OFF)

if(EXAMPLES)
	add_executable(example_simple examples/example.cpp)
endif()

