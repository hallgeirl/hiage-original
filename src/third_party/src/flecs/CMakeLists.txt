# CMakeList.txt : CMake project for hiage-original, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

add_library(flecs STATIC
	flecs.c
)
