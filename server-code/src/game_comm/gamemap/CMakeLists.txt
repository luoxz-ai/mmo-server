MESSAGE(STATUS "cmake: mapdata")
 
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")


# 生成库文件
ADD_LIBRARY(gamemap SHARED ${SRCS})


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(gamemap basecode myproto data_pack)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(gamemap
pb_util
libdata_pack.so
libmyproto.so
libbasecode.so
${JEMALLOC_LIB_NAME}
)

cotire(gamemap)