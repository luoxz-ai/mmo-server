MESSAGE(STATUS "cmake: service_loader")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")




# 生成库文件
add_executable(service_loader ${SRCS})



#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})
ADD_DEPENDENCIES(service_loader basecode network message_route)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(service_loader
 libevent.so
 libevent_core.so
 libevent_pthreads.so
 libevent_extra.so
 libbasecode.so
 libnetwork.so
 libmessage_route.so
 libmydb.so
 libmyproto.so
 libfmt.so
 pthread
 ssl
 crypto
 rt
 dl
 z
 ${JEMALLOC_LIB_NAME}

)


cotire(service_loader)