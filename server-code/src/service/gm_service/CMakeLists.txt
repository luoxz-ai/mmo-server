MESSAGE(STATUS "cmake: gm_service")

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


# include PROTO_HEADER
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER gm_service.proto)

# 生成库文件
ADD_LIBRARY(gm_service SHARED ${SRCS} ${PROTO_SRC} ${PROTO_HEADER})
#SET_TARGET_PROPERTIES(gm_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(gm_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(gm_service PROPERTIES CXX_VISIBILITY_PRESET hidden)
cotire(gm_service)
#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(gm_service basecode network myproto mydb mydbtable service_comm)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(gm_service
  basecode 
  network 
  myproto 
  mydb 
  mydbtable 
  service_comm
  libbrpc.so
  fmt
  pthread
  rt
  dl
 ${JEMALLOC_LIB_NAME}
)

