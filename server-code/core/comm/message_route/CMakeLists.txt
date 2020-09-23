MESSAGE(STATUS "cmake: message_route")

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



#message(STATUS "PROTO:" ${PROTOS} )
# include PROTO_HEADER
set(Protobuf_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/msg_internal.proto)


# 生成库文件
ADD_LIBRARY(message_route SHARED ${SRCS} ${PROTO_SRC} ${PROTO_HEADER})
#SET_TARGET_PROPERTIES(network PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(network PROPERTIES VERSION 0.1 SOVERSION 1)


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})


ADD_DEPENDENCIES(message_route network basecode myproto mydb)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(message_route
 libnetwork.so
 libbasecode.so
 libmyproto.so
 libmydb.so
 cpp_redis
 tinyxml2
 tacopie
 libfmt.so
 ${JEMALLOC_LIB_NAME}
)

set_directory_properties(PROPERTIES
    COTIRE_PREFIX_HEADER_INCLUDE_PATH ${COTIRE_PREFIX_HEADER_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/src/comm")
cotire(message_route)