MESSAGE(STATUS "cmake: protoc-gen-sql")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS FOLLOW_SYMLINKS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
ELSE()
ENDIF()



# 生成库文件
add_executable(protoc-gen-sql ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(protoc-gen-sql
 protoc
 protobuf
 fmt
 dl
 pthread
)
cotire(protoc-gen-sql)


