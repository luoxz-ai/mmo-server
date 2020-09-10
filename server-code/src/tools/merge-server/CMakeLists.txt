MESSAGE(STATUS "cmake: merge-server")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
ELSE()
ENDIF()

# 生成库文件
add_executable(merge-server ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})

# 指定额外的链接库

ADD_DEPENDENCIES(merge-server basecode mydb mydbtable)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(merge-server
 libbasecode.so
 libmydb.so
 libmysqlclient.a
 libmydbtable.so
 libfmt.so
 ssl
 crypto
 z
 ${JEMALLOC_LIB_NAME}
 dl
 pthread)

cotire(merge-server)
