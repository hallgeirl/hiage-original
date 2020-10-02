# CMakeList.txt : CMake project for hiage-original, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

add_library(hiage STATIC
	"hiage/core/audio.cpp"
	"hiage/core/entity.cpp"
	"hiage/core/game.cpp"
	
	"hiage/core/inputmanager.cpp"
	"hiage/core/map.cpp"
	"hiage/core/entitymanager.cpp"
	"hiage/core/resourcemanager.cpp"
	"hiage/core/script_lua.cpp"
	"hiage/gfx/display.cpp"
	"hiage/gfx/fonts.cpp"
	"hiage/gfx/particles.cpp"
	"hiage/gfx/renderer.cpp"
	"hiage/gfx/sprite.cpp"
	"hiage/gfx/texture.cpp"
	"hiage/gfx/tilemap.cpp"
	"hiage/gfx/tileset.cpp"
	
	"hiage/ui/menu.cpp"
	"hiage/ui/windows.cpp"
	"hiage/util/exceptions.cpp"
	"hiage/util/timer.cpp"
	"hiage/core/components.cpp" 
	"hiage/core/systems.cpp" "hiage/core/gamestate.hpp" "hiage/core/events.hpp" "hiage/core/events.cpp" "hiage/core/gamestate.cpp" "hiage/core/collisions.hpp" "hiage/core/collisions.cpp" "hiage/core/boundingpolygon.hpp" "hiage/core/boundingpolygon.cpp")

target_include_directories(hiage PUBLIC ${PROJECT_SOURCE_DIR}/third_party/include ${PROJECT_SOURCE_DIR}/third_party/src/tinyxml)

target_link_directories(hiage PUBLIC ${PROJECT_SOURCE_DIR}/third_party/lib/win-x64/SDL ${PROJECT_SOURCE_DIR}/third_party/lib/win-x64/SDL_Mixer)

target_link_libraries(hiage PUBLIC SDL2.lib OpenGL32.dll SDL2_Mixer.lib SDL2main lua_static luabind tinyxml)
target_compile_features(hiage PUBLIC cxx_std_20)

add_definitions(-DNOMINMAX)

# Collision debugging
# add_definitions(-DDEBUG_COLLISION_OBJECT_POLYGON)
