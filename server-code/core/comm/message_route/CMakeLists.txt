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
# msg_internal.proto
PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/msg_internal.proto)


add_custom_command(
      OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/db_proto/serverinfodb.pb.cc"
             "${CMAKE_CURRENT_SOURCE_DIR}/db_proto/serverinfodb.pb.h"
      COMMAND protobuf::protoc
      ARGS --plugin=${BINARIES_CORE}/protoc-gen-sql --proto_path=${CMAKE_CURRENT_SOURCE_DIR} --sql_out=${CMAKE_CURRENT_SOURCE_DIR}/db/ --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/db_proto/serverinfodb.proto
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/db_proto/serverinfodb.proto" protoc-gen-sql
      COMMENT "Running C++ protocol buffer compiler on db_proto/serverinfodb.proto"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM )

add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/serverinfodb.h"
        COMMAND ${BINARIES_CORE}/sql2cpp 
        ARGS --input=${CMAKE_CURRENT_SOURCE_DIR}/db/db_proto/serverinfodb.pb.sql --outdir=${CMAKE_CURRENT_SOURCE_DIR}/ --output=serverinfodb --format=clang-format-9
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/db_proto/serverinfodb.pb.h sql2cpp 
        COMMENT "gen serverinfodb.h from sql"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM )


# 生成库文件
ADD_LIBRARY(message_route SHARED ${SRCS} ${PROTO_SRC} ${PROTO_HEADER}
"${CMAKE_CURRENT_SOURCE_DIR}/db_proto/serverinfodb.pb.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/db_proto/serverinfodb.pb.h"
"${CMAKE_CURRENT_SOURCE_DIR}/serverinfodb.h")
#SET_TARGET_PROPERTIES(network PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(network PROPERTIES VERSION 0.1 SOVERSION 1)


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})


ADD_DEPENDENCIES(message_route network basecode mydb sql_options)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(message_route
 libnetwork.so
 libbasecode.so
 libmydb.so
 sql_options
 cpp_redis
 tinyxml2
 tacopie
 libfmt.so
 ${JEMALLOC_LIB_NAME}
)


cotire(message_route)