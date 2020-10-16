MESSAGE(STATUS "cmake: pb_util")

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
file(GLOB_RECURSE SRC "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

# 生成库文件
ADD_LIBRARY(pb_util STATIC ${SRC})

#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(pb_util
	libfmt.a
	protobuf
	dl
	pthread
) 
 
cotire(pb_util)