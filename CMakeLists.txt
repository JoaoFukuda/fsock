cmake_minimum_required(VERSION 3.10)

project(fsock)

add_library(fsock fsock.cpp)

set_target_properties(fsock PROPERTIES PUBLIC_HEADER "fsock.hpp")

install(TARGET
	LIBRARY DESTINATION /usr/local/lib
	PUBLIC_HEADER DESTINATION /usr/local/include
)

option(EXAMPLES OFF)

if(EXAMPLES)
	add_executable(example_simple examples/example.cpp)
endif()

