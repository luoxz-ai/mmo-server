IF(DEBUG)
MESSAGE(STATUS "cmake: basecode (debug)")
ELSE()
MESSAGE(STATUS "cmake: basecode (release)")
ENDIF()

IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
ELSE()
ENDIF()

IF(USE_JEMALLOC)
ADD_DEFINITIONS(-DUSE_JEMALLOC)
set(JEMALLOC_LIB_NAME "libjemalloc.so")
ELSE()
ENDIF()

# 搜索当前目录下所有文件
file(GLOB_RECURSE LIBBASECODE_SRC "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

if(AUTO_GEN_EXPORT_LUA)
GET_DIRECTORY_PROPERTY(includes INCLUDE_DIRECTORIES)
add_custom_target(basecode2lua ALL
  COMMAND echo ${LIBBASECODE_SRC} > /tmp/basecode_cpps.txt && echo ${includes} > /tmp/basecode_includes.txt && ${BINARIES}/cxx2lua --cpps=/tmp/basecode_cpps.txt --include=/tmp/basecode_includes.txt  --output=${PROJECT_SOURCE_DIR}/src/game_comm/script/basecode2lua.cpp --output_functionname=basecode2lua --skip_cpp
  DEPENDS cxx2lua
  COMMENT "gen code for export_lua"
 )
else()
endif()

# 生成库文件
ADD_LIBRARY(basecode SHARED ${LIBBASECODE_SRC})

#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})

cotire(basecode)


# 指定额外的链接库
TARGET_LINK_LIBRARIES(basecode
	libfmt.a
	protobuf
	${JEMALLOC_LIB_NAME}
	bfd
	dl
	pthread
) 
 
