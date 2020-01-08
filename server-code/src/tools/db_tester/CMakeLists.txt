
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
IF(USE_JEMALLOC)
ADD_DEFINITIONS(-DUSE_JEMALLOC)
set(JEMALLOC_LIB_NAME "libjemalloc.so")
ELSE()
ENDIF()

# 生成库文件
add_executable(db_tester ${SRCS})

set(CMAKE_CXX_CLANG_TIDY "")

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})
ADD_DEPENDENCIES(db_tester basecode mydb mydbtable)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(db_tester
 libbasecode.so
 libmydb.so
 libmysqlclient.a
 mydbtable
 fmt
 ${JEMALLOC_LIB_NAME}
 dl
 pthread
)

