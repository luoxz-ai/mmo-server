# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
ELSE()
ENDIF()

# 生成库文件
add_executable(cxx2lua ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(cxx2lua
 libclang.so
 dl
 pthread
)



