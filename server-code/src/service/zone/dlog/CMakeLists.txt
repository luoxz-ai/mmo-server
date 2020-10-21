MESSAGE(STATUS "cmake: dlog")
 
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

# 生成库文件
ADD_LIBRARY(dlog SHARED ${SRCS})


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(dlog
)

