cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS "cmake: project root")

option(DEBUG "ON for debug or OFF for release" OFF)
option(TIDY "ON for use clang-tidy(cmake version 3.6+) or OFF for not use" OFF)
option(USE_LUADEBUGER "ON for use lua_debuger or OFF for lua" OFF)
option(USE_JEMALLOC "ON for use jemalloc or OFF" ON)
option(LIBCLANG "ON for FIND LIBCLANG or OFF" ON)
option(AUTO_GEN_EXPORT_LUA "ON for AUTO_GEN_EXPORT_LUA or OFF" ON)

set_directory_properties(PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)

#-Woverloaded-virtual 


PROJECT(Framework C CXX)

SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -g -DDEBUG ")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -g -O3 -DNDEBUG ")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++17 -g -DDEBUG -Wall -Wno-reorder -Wno-unused-variable -Wno-unused-function -fexceptions -Wno-unused-result")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++17 -g -O3 -DNDEBUG -Wall -Wno-reorder -Wno-unused-variable -Wno-unused-function -fexceptions -Wno-unused-result")

IF(NOT CMAKE_CXX_COMPILER MATCHES "clang")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -rdynamic")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -rdynamic")
ELSE()
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--export-dynamic")
ENDIF()


set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/src/CMake")
include(cotire)


if(LIBCLANG)
include(FindLibClang)
else()
SET(AUTO_GEN_EXPORT_LUA OFF)
endif()

#-----------------------------------------------------------------------------
# Enable ccache if not already enabled by symlink masquerading and if no other
# CMake compiler launchers are already defined
#-----------------------------------------------------------------------------
find_program(CCACHE_EXECUTABLE ccache)
mark_as_advanced(CCACHE_EXECUTABLE)
if(CCACHE_EXECUTABLE)
  foreach(LANG C CXX)
    if(NOT DEFINED CMAKE_${LANG}_COMPILER_LAUNCHER AND NOT CMAKE_${LANG}_COMPILER MATCHES ".*/ccache$")
      message(STATUS "Enabling ccache for ${LANG}")
      set(CMAKE_${LANG}_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE} CACHE STRING "")
    endif()
  endforeach()
endif()

include(FindThreads)

set(PROTOBUF_USE_STATIC_LIBS ON)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${PROJECT_SOURCE_DIR}/depends/src/protobuf/src)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${PROJECT_SOURCE_DIR}/depends/lib)
SET(PROTOBUF_PROTOC_EXECUTABLE ${PROJECT_SOURCE_DIR}/depends/bin/protoc) 
include(FindProtobuf)

set (MY_CLANG_TIDY_STR "clang-tidy-4.0;-style=file;-checks=-*,clang-analyzer-core*,clang-analyzer-cplusplus*,clang-analyzer-nullability*")



SET(CMAKE_BUILD_RPATH ".:./libs")
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_SKIP_INSTALL_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
SET(CMAKE_INSTALL_RPATH ".:./libs")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE) 

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/jemalloc/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/cpp_redis/includes)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/cpp_redis/tacopie/includes)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/libevent/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/protobuf/src)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/lua/src)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/curlpp/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/rapidjson/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/xlnt/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/fmt/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/brpc/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/gflags/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/leveldb/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/snappy)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/libwebsockets/build/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/cpp-taskflow)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/)
if(LIBCLANG)
include_directories(SYSTEM ${LIBCLANG_INCLUDE_DIRS})
else()
endif()
include_directories(${PROJECT_SOURCE_DIR}/src/share/)
include_directories(${PROJECT_SOURCE_DIR}/src/comm/basecode/)
include_directories(${PROJECT_SOURCE_DIR}/src/comm/network/)
include_directories(${PROJECT_SOURCE_DIR}/src/comm/message_route/)
include_directories(${PROJECT_SOURCE_DIR}/src/comm/mydb/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/proto/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/proto/config/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/proto/common/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/script/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/gamemap/)
include_directories(${PROJECT_SOURCE_DIR}/src/service/service_comm/)
include_directories(${PROJECT_SOURCE_DIR}/src/)

macro(source_group_by_dir source_files)
    #if(MSVC)
        set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        foreach(sgbd_file ${${source_files}})
			string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
			string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
			string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
			string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)	
            source_group(${sgbd_group_name} FILES ${sgbd_file})
			list(FIND sgbd_dirs "${sgbd_cur_dir}/${sgbd_group_name}" RET)
			if (${RET} EQUAL -1)
				list(APPEND sgbd_dirs "${sgbd_cur_dir}/${sgbd_group_name}")
			endif()
        endforeach(sgbd_file)
		foreach(sgbd_dir ${sgbd_dirs})
			include_directories("${sgbd_dir}")
		endforeach()	
    #endif(MSVC)
endmacro(source_group_by_dir)

macro(source_group_by_files sgbd_group_name source_files)
    #if(MSVC)
		set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        foreach(sgbd_file ${${source_files}})	
			message("group:${sgbd_group_name} file:${sgbd_file}")	
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    #endif(MSVC)
endmacro(source_group_by_files)


IF(DEBUG)
MESSAGE(STATUS "Build Debug...")
SET(CMAKE_BUILD_TYPE Debug)
SET(LIBRARIES ${PROJECT_BINARY_DIR}/lib/debug)
SET(BINARIES ${PROJECT_BINARY_DIR}/bin/debug)
ELSE()
MESSAGE(STATUS "Build Release...")
SET(CMAKE_BUILD_TYPE Release)
SET(LIBRARIES ${PROJECT_BINARY_DIR}/lib/release)
SET(BINARIES ${PROJECT_BINARY_DIR}/bin/release)
ENDIF()


LINK_DIRECTORIES(
 ${PROJECT_SOURCE_DIR}/depends/lib/
 ${LIBRARIES}
)

if(LIBCLANG)
LINK_DIRECTORIES(${LIBCLANG_LIBRARY_DIR})
else()
endif()

IF(DEBUG)
ADD_SUBDIRECTORY(src obj/debug)
ELSE()
ADD_SUBDIRECTORY(src obj/release)
ENDIF()



MESSAGE(STATUS ${CMAKE_BUILD_TYPE})
