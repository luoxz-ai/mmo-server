MESSAGE(STATUS "cmake: protoc-gen-sql")


# 搜索当前目录下所有文件
file(GLOB SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
ELSE()
ENDIF()

set(Protobuf_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# 生成sql_options.proto
PROTOBUF_GENERATE_CPP_NEW(SQL_OPTIONS_SRC SQL_OPTIONS_HEADER ${CMAKE_CURRENT_SOURCE_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}/sql_options/sql_options.proto)


# 生成库文件
add_executable(protoc-gen-sql ${SRCS} ${SQL_OPTIONS_SRC} ${SQL_OPTIONS_HEADER} )

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


