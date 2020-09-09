MESSAGE(STATUS "cmake: robotclient")
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
add_executable(robotclient ${SRCS})



#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})
ADD_DEPENDENCIES(robotclient basecode network script myproto)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(robotclient
 libbasecode.so
 libevent.so
 libevent_core.so
 libevent_pthreads.so
 libevent_extra.so
 libnetwork.so
 libscript.so
 libmyproto.so
 protobuf
 ${JEMALLOC_LIB_NAME}
 dl
 pthread
)
cotire(robotclient)
