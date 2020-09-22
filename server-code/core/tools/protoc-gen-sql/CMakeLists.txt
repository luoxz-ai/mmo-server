MESSAGE(STATUS "cmake: protoc-gen-sql")


## 搜索当前目录下所有proto
file(GLOB PROTOS FOLLOW_SYMLINKS "sql_options/*.proto")

#message(STATUS "PROTO:" ${PROTOS} )
# include PROTO_HEADER
set(Protobuf_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
PROTOBUF_GENERATE_CPP_NEW(PROTO_SRC PROTO_HEADER ${CMAKE_CURRENT_SOURCE_DIR} ${PROTOS})

# 搜索当前目录下所有文件
file(GLOB SRCS FOLLOW_SYMLINKS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
ELSE()
ENDIF()


# 生成库文件
add_executable(protoc-gen-sql ${SRCS} ${PROTO_SRC} ${PROTO_HEADER})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(protoc-gen-sql
 protoc
 protobuf
 fmt
 dl
 pthread
)
#cotire(protoc-gen-sql)


