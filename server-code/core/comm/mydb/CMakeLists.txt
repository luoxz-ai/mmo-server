MESSAGE(STATUS "cmake: mydb")
 
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
ELSE()
ENDIF()
IF(USE_JEMALLOC)
ADD_DEFINITIONS(-DUSE_JEMALLOC)
set(JEMALLOC_LIB_NAME "libjemalloc.so")
ELSE()
ENDIF()


# 生成库文件
ADD_LIBRARY(mydb SHARED ${SRCS})


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})
ADD_DEPENDENCIES(mydb basecode)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(mydb
	libbasecode.so
	libmysqlclient.a
	ssl
	dl
	pthread
	${JEMALLOC_LIB_NAME}
)
cotire(mydb)
