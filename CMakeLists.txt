cmake_minimum_required(VERSION 2.8)

project(libbaalue C)

set(SOURCE_FILES libbaalue.c) 

set(CMAKE_C_FLAGS "-g -Wall -std=c11")

# set version number
set(libbaalue_VERSION_MAJOR 0)
set(libbaalue_VERSION_MINOR 0)
set(libbaalue_VERSION_BUGFIX 1)

configure_file (
"${PROJECT_SOURCE_DIR}/config.h.in"
"${PROJECT_BINARY_DIR}/config.h"
)

# add binary to include path
include_directories("${PROJECT_BINARY_DIR}")

# check needed includes
include(CheckIncludeFile)
check_include_file(confuse.h HAVE_CONFUSE)

# external libs 
#target_link_libraries(baalue_static confuse)
#target_link_libraries(baalue confuse)

# create librarys
add_library(baalue_static STATIC ${SOURCE_FILES})
add_library(baalue SHARED ${SOURCE_FILES})

# 
#target_link_libraries(baalue_static confuse)
#target_link_libraries(baalue confuse)

# install all
install(TARGETS baalue_static DESTINATION lib)
install(TARGETS baalue DESTINATION lib)
install(FILES libbaalue.h DESTINATION include)
