cmake_minimum_required(VERSION 2.8)

project(libbaalue C)

set(SOURCE_FILES
	libbaalue.c
	daemon.c
	network.c
	lcd.c
) 

set(CMAKE_C_FLAGS "-Wall -Wextra -std=c11")

# set version number
set(libbaalue_VERSION_MAJOR 0)
set(libbaalue_VERSION_MINOR 0)
set(libbaalue_VERSION_PATCH 2)
set(libbaalue_VERSION ${libbaalue_VERSION_MAJOR}.${libbaalue_VERSION_MINOR}.${libbaalue_VERSION_PATCH})

# set build type 
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
   set(CMAKE_BUILD_TYPE     RelWithDebInfo)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")

# some status infos
message(STATUS "libbaalue version: ${libbaalue_VERSION}")
message(STATUS "CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
message(STATUS "Host system: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Source directory: ${libbaalue_SOURCE_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# special config handling
configure_file (
"${PROJECT_SOURCE_DIR}/config.h.in"
"${PROJECT_BINARY_DIR}/config.h"
)

# add missing cmake configs
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

# check for other libs
# find_package(Confuse REQUIRED)
find_package(LibBSD REQUIRED)

# add bin to include path
include_directories(
	"${PROJECT_BINARY_DIR}"
#	"${CONFUSE_INCLUDE_DIR}"
	)

# check needed includes
include(CheckIncludeFile)
# check_include_file(confuse.h HAVE_LIBCONFUSE)
check_include_file(bsd/stdlib.h HAVE_LIBBSD)

# create shared lib
add_library(baalue SHARED ${SOURCE_FILES})
set_target_properties(baalue PROPERTIES VERSION ${libbaalue_VERSION} SOVERSION ${libbaalue_VERSION_MAJOR})
set_target_properties(baalue PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# link all together (libbaalue.so)
target_link_libraries(baalue
	${CONFUSE_LIBRARY}
       	bsd
)

# install shared lib
install(TARGETS baalue
	LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

# create static lib
add_library(baalue_static STATIC ${SOURCE_FILES})
set_target_properties(baalue_static PROPERTIES OUTPUT_NAME "baalue")
#set_target_properties(baalue-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# link all together (libbaalue.a)
target_link_libraries(baalue_static
	${CONFUSE_LIBRARY}
       	bsd
)

# install static lib
install(TARGETS baalue_static ARCHIVE DESTINATION lib${LIB_SUFFIX})

# install header
install(FILES libbaalue.h
          DESTINATION include
          COMPONENT headers
        )

