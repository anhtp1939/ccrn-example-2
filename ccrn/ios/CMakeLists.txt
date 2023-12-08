cmake_minimum_required(VERSION 3.8)

set(CMAKE_SYSTEM_NAME iOS)
set(APP_NAME "ccrn"  CACHE STRING "Project Name")

project(${APP_NAME} CXX)

set(CC_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(CC_UI_RESOURCES)
set(CC_PROJ_SOURCES)
set(CC_ASSET_FILES)
set(CC_COMMON_SOURCES)
set(CC_ALL_SOURCES)
set(COCOS_SOURCE_LIST)
set(ENGINE_NAME)

list(APPEND COCOS_SOURCE_LIST
    ${CC_PROJECT_DIR}/service/SDKWrapper.m
    ${CC_PROJECT_DIR}/service/SDKWrapper.h
    ${CC_PROJECT_DIR}/src/CCAppDelegate.mm
    ${CC_PROJECT_DIR}/src/CCAppDelegate.h
    ${CC_PROJECT_DIR}/src/ViewController.mm
    ${CC_PROJECT_DIR}/src/ViewController.h
)

include(${CC_PROJECT_DIR}/../common/CMakeLists.txt)

target_sources(${ENGINE_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/SDKWrapper.h
    ${CMAKE_CURRENT_LIST_DIR}/src/SDKWrapper.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/CCAppDelegate.h
    ${CMAKE_CURRENT_LIST_DIR}/src/CCAppDelegate.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/ViewController.h
    ${CMAKE_CURRENT_LIST_DIR}/src/ViewController.mm
    ${CMAKE_CURRENT_LIST_DIR}/../common/Classes/Game.h
    ${CMAKE_CURRENT_LIST_DIR}/../common/Classes/Game.cpp
)

target_include_directories(${ENGINE_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../common/Classes
    ${CMAKE_CURRENT_LIST_DIR}/src
)