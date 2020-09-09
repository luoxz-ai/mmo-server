MESSAGE(STATUS "cmake: pbbin2txt")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

# 生成库文件
add_executable(pbbin2txt ${SRCS})
set(CMAKE_CXX_CLANG_TIDY "")
#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(pbbin2txt
 basecode
 protobuf
 dl
 pthread
)



cotire(pbbin2txt)