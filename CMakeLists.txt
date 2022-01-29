cmake_minimum_required(VERSION 3.18)

project(obs-active-scene-dock)

find_package(LibObs REQUIRED)
find_package(Qt5Widgets REQUIRED)

if(WIN32)
	include_directories(../obs-studio/UI/obs-frontend-api)
	link_directories(../obs-studio/build/UI/obs-frontend-api/Release)
endif()

set(obs-active-scene-dock_SOURCES
    obs-active-scene-dock.cpp
    ActiveSceneDock.hpp ActiveSceneDock.cpp)

add_library(obs-active-scene-dock MODULE
    ${obs-active-scene-dock_SOURCES})

set_target_properties(obs-active-scene-dock PROPERTIES PREFIX "")
target_link_libraries(obs-active-scene-dock
    Qt5::Widgets
    libobs
    obs-frontend-api)
