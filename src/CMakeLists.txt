# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("hiage-original")

# Include sub-projects.
add_subdirectory ("lua")
add_subdirectory ("luabind")
add_subdirectory ("engine")
add_subdirectory ("mario")
