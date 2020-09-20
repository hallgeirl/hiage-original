cmake_minimum_required (VERSION 3.8)

add_library(luabind STATIC
	class.cpp
	class_info.cpp
	class_registry.cpp
	class_rep.cpp
	CMakeLists.txt
	create_class.cpp
	error.cpp
	find_best_match.cpp
	function.cpp
	implicit_cast.cpp
	link_compatibility.cpp
	object_rep.cpp
	open.cpp
	overload_rep.cpp
	pcall.cpp
	ref.cpp
	scope.cpp
	stack_content_by_name.cpp
	weak_ref.cpp
	wrapper_base.cpp
)

target_include_directories(luabind PUBLIC ../external-libraries/include ../lua/include C:/local/boost_1_74_0)

target_link_libraries(luabind PUBLIC lua_static)
target_compile_features(luabind PUBLIC cxx_std_20)
add_definitions(-DLUA_COMPAT_5_3)
