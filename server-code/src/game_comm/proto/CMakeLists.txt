MESSAGE(STATUS "cmake: proto")

IF(WIN32)
ELSE()
execute_process(
	COMMAND ${PROJECT_SOURCE_DIR}/src/game_comm/proto/gen_all_proto.sh
	COMMENT "gen proto file"
 )
add_custom_target(myproto_autogen ALL
  COMMAND ${PROJECT_SOURCE_DIR}/src/game_comm/proto/gen_all_proto.sh
  COMMENT "gen proto file"
 )
ENDIF()

IF(USE_JEMALLOC)
ADD_DEFINITIONS(-DUSE_JEMALLOC)
set(JEMALLOC_LIB_NAME "libjemalloc.so")
ELSE()
ENDIF()

## 搜索当前目录下所有文件
#file(GLOB PROTOS "*/*.proto")

## include PROTO_HEADER
#set(Protobuf_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
#set(PROTOBUF_GENERATE_CPP_APPEND_PATH 1)
#include_directories(${CMAKE_CURRENT_BINARY_DIR})
#protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${PROTOS})
## 生成库文件
#ADD_LIBRARY(myproto SHARED ${PROTO_SRC} ${PROTO_HEADER})


# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS FOLLOW_SYMLINKS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
source_group_by_dir(SRCS)
# 生成库文件
ADD_LIBRARY(myproto SHARED ${SRCS})


IF(WIN32)
ELSE()
ADD_DEPENDENCIES(myproto myproto_autogen)
ENDIF()

set(CMAKE_CXX_CLANG_TIDY "")
 
#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(myproto
 protobuf
 ${JEMALLOC_LIB_NAME}
)

cotire(myproto)