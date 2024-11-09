cmake_minimum_required(VERSION 3.24)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
project(4msVCVModules VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PLUGIN_NAME 4msCompany)
set(ADDITIONAL_PLUGIN_DISTRIBUTABLES res LICENSE.md LICENSE-GPL-v3.txt README.md)

include(RackSDK.cmake)

message(STATUS "Installing into '${CMAKE_INSTALL_PREFIX}'")

target_compile_options(${RACK_PLUGIN_LIB} PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)

add_compile_definitions(VCVRACK)

if (("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin") AND ("${CMAKE_OSX_ARCHITECTURES}" MATCHES "x86_64"))
  message("Disabling stack check for macOS x86")
  add_compile_options(-fno-stack-check)
endif()

target_include_directories(${RACK_PLUGIN_LIB} PRIVATE 
	src
	lib
)

file(GLOB SOURCES 
	src/models/*.cc
)

target_sources(${RACK_PLUGIN_LIB} PRIVATE
	src/plugin.cc
	src/hub/hub_map_button.cc
	src/hub/hub_medium.cc
	src/hub/mm_blendish.cc
	src/comm/comm_module.cc
	src/mapping/midi_modules.cc
	src/mapping/patch_writer.cc
    src/network/network.cpp
	${SOURCES}
)

set(VCVRACK TRUE)

add_subdirectory(lib/CoreModules)
add_subdirectory(lib/CoreModules/4ms)
add_subdirectory(lib/metamodule-core-interface)
add_subdirectory(lib/patch-serial)
add_subdirectory(lib/cpputil)

target_link_libraries(${RACK_PLUGIN_LIB} PRIVATE 
	CoreModules
	metamodule::core-interface
	metamodule::patch-serial
	cpputil
)

target_link_libraries(${RACK_PLUGIN_LIB} PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,CoreModules-4ms>")

