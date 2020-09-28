MESSAGE(STATUS "cmake: proto")

IF(USE_JEMALLOC)
ADD_DEFINITIONS(-DUSE_JEMALLOC)
set(JEMALLOC_LIB_NAME "libjemalloc.so")
ELSE()
ENDIF()

## 搜索当前目录下所有proto
file(GLOB PROTOS FOLLOW_SYMLINKS "*/*.proto")

#message(STATUS "PROTO:" ${PROTOS} )
# include PROTO_HEADER
set(Protobuf_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
PROTOBUF_GENERATE_CPP_NEW(PROTO_SRC PROTO_HEADER ${CMAKE_CURRENT_SOURCE_DIR} ${PROTOS})
#message(STATUS "PROTO_SRC:" ${PROTO_SRC} )
#message(STATUS "PROTO_HEADER:" ${PROTO_HEADER} )
# 生成库文件
file(GLOB SRCS FOLLOW_SYMLINKS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
ADD_LIBRARY(myproto SHARED ${SRCS} ${PROTO_SRC} ${PROTO_HEADER})


set(CMAKE_CXX_CLANG_TIDY "")
 
#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(myproto
 protobuf
 ${JEMALLOC_LIB_NAME}
)


#cotire(myproto)