cmake_minimum_required(VERSION 2.8)

project(libbaalue C)

set(SOURCE_FILES
	libbaalue.c
	daemon.c
	config.c
	network.c
) 

set(CMAKE_C_FLAGS "-g -Wall -Wextra -std=c11")

# set version number
set(libbaalue_VERSION_MAJOR 0)
set(libbaalue_VERSION_MINOR 0)
set(libbaalue_VERSION_PATCH 1)
set(libbaalue_VERSION ${libbaalue_VERSION_MAJOR}.${libbaalue_VERSION_MINOR}.${libbaalue_VERSION_PATCH})

# special config handling
configure_file (
"${PROJECT_SOURCE_DIR}/config.h.in"
"${PROJECT_BINARY_DIR}/config.h"
)

# add bin to include path
include_directories(
	"${PROJECT_BINARY_DIR}"
	)

# check needed includes
include(CheckIncludeFile)
check_include_file(confuse.h HAVE_LIBCONFUSE)
check_include_file(bsd/stdlib.h HAVE_LIBBSD)

# create libraries
add_library(baalue_static STATIC ${SOURCE_FILES})
add_library(baalue SHARED ${SOURCE_FILES})

# link all together (libbaalue.so)
target_link_libraries(baalue
	confuse
       	bsd
)
# link all together (libbaalue.a)
target_link_libraries(baalue_static
	confuse
       bsd
)

# target properties
set_target_properties(baalue PROPERTIES VERSION ${libbaalue_VERSION} SOVERSION ${libbaalue_VERSION_MAJOR})

# install all
install(TARGETS baalue_static DESTINATION lib)
install(TARGETS baalue DESTINATION lib)
install(FILES libbaalue.h DESTINATION include)
