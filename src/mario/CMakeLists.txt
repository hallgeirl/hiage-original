cmake_minimum_required (VERSION 3.15)

add_executable (mario 
    "mariocomponents.cpp"
	"main.cpp" 
	"mariogame.cpp"
	"mariostate.cpp"
    "mariosystems.cpp"
 "events.hpp")

target_include_directories(mario PUBLIC ../hiage)

target_link_libraries(mario PUBLIC hiage)
target_compile_features(mario PUBLIC cxx_std_20)

add_definitions(-DDATAROOT="${PROJECT_SOURCE_DIR}/mario/data")

target_compile_options(mario
    PRIVATE "-W4"
)

add_custom_command(TARGET mario POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/third_party/lib/win-x64/SDL/SDL2.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:mario>)                 # <--this is out-file path

#add_custom_command(TARGET mario PRE_LINK        # Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
#        "${PROJECT_SOURCE_DIR}/mario/data"      # <--this is in-file
#        $<TARGET_FILE_DIR:mario>/data)                 # <--this is out-file path

add_custom_command(TARGET mario POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/third_party/lib/win-x64/SDL_Mixer"      # <--this is in-file
        $<TARGET_FILE_DIR:mario>)                 # <--this is out-file path

