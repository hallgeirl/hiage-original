cmake_minimum_required (VERSION 3.8)

project ("hiage-original")

# Include sub-projects.
add_subdirectory ("third_party/src/lua")
add_subdirectory ("third_party/src/luabind")
add_subdirectory ("engine")
add_subdirectory ("mario")
