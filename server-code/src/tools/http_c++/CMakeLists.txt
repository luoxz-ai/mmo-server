
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER http.proto)
# include PROTO_HEADER
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_DEFINITIONS(-DGFLAGS_NS=google)

SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})

add_executable(http_client http_client.cpp)
add_executable(http_server http_server.cpp ${PROTO_SRC} ${PROTO_HEADER})



target_link_libraries(http_server 
 basecode
 pthread
 gflags
 leveldb
 brpc
 rt
 dl)
 
target_link_libraries(http_client pthread
 gflags
 leveldb
 brpc
 rt
 dl
 )
    

