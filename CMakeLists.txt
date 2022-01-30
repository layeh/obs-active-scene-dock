cmake_minimum_required(VERSION 3.18)

project(active-scene-dock VERSION 0.1.1)

if(BUILD_OUT_OF_TREE)
    find_package(LibObs REQUIRED)
    include_directories(../obs-studio/UI/obs-frontend-api)
    link_directories(../obs-studio/build/UI/obs-frontend-api/Release)
else()
    include_directories(../../UI/obs-frontend-api)
endif()

find_package(Qt5Widgets REQUIRED)

set(active-scene-dock_SOURCES
    active-scene-dock.cpp
    ActiveSceneDock.hpp ActiveSceneDock.cpp)

add_library(active-scene-dock MODULE
    ${active-scene-dock_SOURCES})

set_target_properties(active-scene-dock PROPERTIES PREFIX "")
target_link_libraries(active-scene-dock
    Qt5::Widgets
    libobs
    obs-frontend-api)

IF(BUILD_OUT_OF_TREE)
    install(TARGETS active-scene-dock
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/obs-plugins)
    install(DIRECTORY data/locale
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/obs/obs-plugins/${CMAKE_PROJECT_NAME})
else()
    install_obs_plugin_with_data(active-scene-dock data)
endif()
