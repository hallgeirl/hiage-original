# CMakeList.txt : CMake project for hiage-original, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

#project(hiage VERSION 1.0.0 DESCRIPTION "Hiage game engine")

add_library(hiage STATIC
	com_exceptions.cpp
	com_game.cpp
	com_gamestate.cpp
	com_objectfactory.cpp
	com_resmanager.cpp
	game_entity.cpp
	game_map.cpp
	gfx_display.cpp
	gfx_particles.cpp
	gfx_renderer.cpp
	gfx_sprite.cpp
	gfx_texture.cpp
	gfx_tilemap.cpp
	gfx_tileset.cpp
	in_inputmanager.cpp
	script_lua.cpp
	snd_audio.cpp
	ui_menu.cpp
	ui_windows.cpp
	util_fonts.cpp
	util_timer.cpp
	tinyxml/tinystr.cpp
	tinyxml/tinyxml.cpp
	tinyxml/tinyxmlerror.cpp
	tinyxml/tinyxmlparser.cpp
)

target_include_directories(hiage PUBLIC ../external-libraries/include C:/local/boost_1_74_0)

target_link_directories(hiage PUBLIC ../external-libraries/win-x64/SDL ../external-libraries/win-x64/SDL_Mixer)

target_link_libraries(hiage PUBLIC SDL2.lib OpenGL32.dll SDL2_Mixer.lib lua_static luabind)
target_compile_features(hiage PUBLIC cxx_std_20)

# Add source to this project's executable.


# add_executable (hiage-original "hiage-original.cpp" "hiage-original.h")

# TODO: Add tests and install targets if needed.
