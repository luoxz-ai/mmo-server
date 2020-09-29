MESSAGE(STATUS "cmake: sqlurl")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
ELSE()
ENDIF()

# 生成库文件
add_executable(sqlurl ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})
ADD_DEPENDENCIES(sqlurl crypto_comm_static)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(sqlurl
 libfmt.a
 crypto_comm_static
 libcryptopp.a
 dl
 pthread
)


cotire(sqlurl)
