MESSAGE(STATUS "cmake: robotclient")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")



# 生成库文件
add_executable(robotclient ${SRCS})



#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})
ADD_DEPENDENCIES(robotclient basecode network script myproto)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(robotclient
 libbasecode.so
 libnetwork.so
 libscript.so
 libmyproto.so
 protobuf
 ${JEMALLOC_LIB_NAME}
 dl
 pthread
)
cotire(robotclient)
