MESSAGE(STATUS "cmake: basecode")

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


GET_DIRECTORY_PROPERTY(LIBBASECODE_INCLUDES INCLUDE_DIRECTORIES)
SET_PROPERTY(GLOBAL PROPERTY LIBBASECODE_INCLUDES ${LIBBASECODE_INCLUDES})
SET_PROPERTY(GLOBAL PROPERTY LIBBASECODE_SRC ${LIBBASECODE_SRC})



# 生成库文件
ADD_LIBRARY(basecode SHARED ${LIBBASECODE_SRC})

add_dependencies(basecode cxx2lua)
add_custom_command(TARGET basecode
PRE_LINK
COMMAND echo ${LIBBASECODE_SRC} > /tmp/basecode_cpps.txt && echo ${LIBBASECODE_INCLUDES} > /tmp/basecode_includes.txt 
COMMAND ${BINARIES_CORE}/cxx2lua --cpps=/tmp/basecode_cpps.txt --include=/tmp/basecode_includes.txt  --output=${PROJECT_SOURCE_DIR}/core/comm/script/basecode2lua.cpp --output_functionname=basecode2lua --skip_cpp --format=clang-format-9
COMMENT "gen basecode for export_lua"
)

#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(basecode
	libfmt.a
	protobuf
	${JEMALLOC_LIB_NAME}
	bfd
	dl
	pthread
) 
 
cotire(basecode)