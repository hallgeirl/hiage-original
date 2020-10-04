cmake_minimum_required (VERSION 3.15)

add_executable (tilemapconverter 
	"main.cpp" 
)

target_include_directories(tilemapconverter PUBLIC ../hiage)

target_link_libraries(tilemapconverter PUBLIC hiage)
target_compile_features(tilemapconverter PUBLIC cxx_std_20)

add_definitions(-DDATAROOT="${PROJECT_SOURCE_DIR}/mario/data")

target_compile_options(tilemapconverter
    PRIVATE "-W4"
)

add_custom_command(TARGET tilemapconverter POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/third_party/lib/win-x64/SDL/SDL2.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:tilemapconverter>)                 # <--this is out-file path

#add_custom_command(TARGET tilemapconverter PRE_LINK        # Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
#        "${PROJECT_SOURCE_DIR}/mario/data"      # <--this is in-file
#        $<TARGET_FILE_DIR:tilemapconverter>/data)                 # <--this is out-file path

add_custom_command(TARGET tilemapconverter POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/third_party/lib/win-x64/SDL_Mixer"      # <--this is in-file
        $<TARGET_FILE_DIR:tilemapconverter>)                 # <--this is out-file path

