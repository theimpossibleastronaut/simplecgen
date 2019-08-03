cmake_minimum_required(VERSION 3.2)

set (PROJECT "simplecgen")

add_definitions("-DPACKAGE_BUGREPORT=\"andy400-dev@yahoo.com\"")

add_definitions("-DVERSION=\"0.1.02-dev\"")

set(CMAKE_C_FLAGS "-Wall")

set (SIMPLECTEMPLATE "simplectemplate/src")
include_directories(${SIMPLECTEMPLATE})

add_executable(${PROJECT} simplecgen.c utils.c simplectemplate/src/template_functions.c)
