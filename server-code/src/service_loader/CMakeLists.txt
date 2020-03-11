MESSAGE(STATUS "cmake: service_loader")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
ELSE()
ENDIF()
IF(USE_JEMALLOC)
ADD_DEFINITIONS(-DUSE_JEMALLOC)
set(JEMALLOC_LIB_NAME "libjemalloc.so")
ELSE()
ENDIF()


# 生成库文件
add_executable(service_loader ${SRCS})
cotire(service_loader)


#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})
ADD_DEPENDENCIES(service_loader basecode network message_route)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(service_loader
 libevent.so
 libevent_core.so
 libevent_pthreads.so
 libevent_extra.so
 basecode
 network
 message_route
 mydb
 myproto
 websockets
 fmt
 tinyxml2
 pthread
 ssl
 libcrypto.so
 rt
 dl
 z
 ${JEMALLOC_LIB_NAME}

)

