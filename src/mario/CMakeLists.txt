# CMakeList.txt : CMake project for hiage-original, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.

project(mario VERSION 1.0.0 DESCRIPTION "Mario")

add_executable (mario "main.cpp" "mariogame.cpp")

target_include_directories(mario PUBLIC ../ ../external-libraries/include)

target_link_directories(mario PUBLIC ../engine/-libraries/win-x64/SDL ../external-libraries/win-x64/lua ../external-libraries/win-x64/SDL_Mixer)

target_link_libraries(mario PUBLIC ../out/build/x64-Debug/engine/hiage)
target_compile_features(mario PUBLIC cxx_std_20)



# TODO: Add tests and install targets if needed.
