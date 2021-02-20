cmake_minimum_required (VERSION 3.15)

project ("hiage-original")

# Include sub-projects.
add_subdirectory ("third_party/src/tinyxml")
add_subdirectory ("third_party/src/lua")
add_subdirectory ("hiage")
add_subdirectory ("mario")
add_subdirectory ("tilemapconverter")
