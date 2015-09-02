cmake_minimum_required(VERSION 2.8)

project(libbaalue)

add_library(baalue_static STATIC libbaalue.c)
add_library(baalue SHARED libbaalue.c)

install(TARGETS baalue_static DESTINATION lib)
install(TARGETS baalue DESTINATION lib)
install(FILES libbaalue.h DESTINATION include)
