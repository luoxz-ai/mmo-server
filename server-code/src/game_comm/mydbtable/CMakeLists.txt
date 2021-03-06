MESSAGE(STATUS "cmake: mydb_table")



# add_custom_target(mydbtable_protoc-gen-sql ALL
#  COMMAND ${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/gen_proto.sh gamedb ${BINARIES_CORE} 
#  COMMAND ${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/gen_proto.sh globaldb ${BINARIES_CORE}
#  DEPENDS protoc-gen-sql
#  COMMENT "gen sql from proto"
# ) 


# add_custom_target(mydbtable_sql2cpp ALL
#   COMMAND ${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/gen_sql_def.sh ${BINARIES_CORE} gamedb && ${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/gen_sql_def.sh  ${BINARIES_CORE} globaldb
#   DEPENDS sql2cpp mydbtable_protoc-gen-sql
#   COMMENT "gen dbdefine from sql"
#  )


add_custom_command(
      OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/db_proto/gamedb.pb.cc"
             "${CMAKE_CURRENT_SOURCE_DIR}/db_proto/gamedb.pb.h"
      COMMAND protobuf::protoc
      ARGS --plugin=${BINARIES_CORE}/protoc-gen-sql --proto_path=${CMAKE_CURRENT_SOURCE_DIR} --sql_out=${CMAKE_CURRENT_SOURCE_DIR}/db/ --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/db_proto/gamedb.proto
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/db_proto/gamedb.proto" protoc-gen-sql
      COMMENT "Running C++ protocol buffer compiler on db_proto/gamedb.proto"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM )

    
add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/db_proto/globaldb.pb.cc"
               "${CMAKE_CURRENT_SOURCE_DIR}/db_proto/globaldb.pb.h"
        COMMAND protobuf::protoc
        ARGS --plugin=${BINARIES_CORE}/protoc-gen-sql --proto_path=${CMAKE_CURRENT_SOURCE_DIR} --sql_out=${CMAKE_CURRENT_SOURCE_DIR}/db/ --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/db_proto/globaldb.proto
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/db_proto/globaldb.proto" protoc-gen-sql
        COMMENT "Running C++ protocol buffer compiler on db_proto/globaldb.proto"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM )

add_custom_command(
          OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/gamedb.h"
          COMMAND ${BINARIES_CORE}/sql2cpp 
          ARGS --input=${CMAKE_CURRENT_SOURCE_DIR}/db/db_proto/gamedb.pb.sql --outdir=${CMAKE_CURRENT_SOURCE_DIR}/ --output=gamedb --format=clang-format-9
          DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/db_proto/gamedb.pb.h sql2cpp 
          COMMENT "gen gamedb.h from sql"
          VERBATIM )

add_custom_command(
            OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/globaldb.h"
            COMMAND ${BINARIES_CORE}/sql2cpp 
            ARGS --input=${CMAKE_CURRENT_SOURCE_DIR}/db/db_proto/globaldb.pb.sql --outdir=${CMAKE_CURRENT_SOURCE_DIR}/ --output=globaldb --format=clang-format-9
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/db_proto/globaldb.pb.h sql2cpp
            COMMENT "gen globaldb.h from sql"
            VERBATIM )





# 搜索当前目录下所有文件
#file(GLOB_RECURSE SRCS FOLLOW_SYMLINKS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
set(SRCS
"${CMAKE_CURRENT_SOURCE_DIR}/db_proto/gamedb.pb.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/db_proto/gamedb.pb.h"
"${CMAKE_CURRENT_SOURCE_DIR}/db_proto/globaldb.pb.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/db_proto/globaldb.pb.h"
"${CMAKE_CURRENT_SOURCE_DIR}/gamedb.h"
"${CMAKE_CURRENT_SOURCE_DIR}/globaldb.h")


source_group_by_dir(SRCS)
# 生成库文件
ADD_LIBRARY(mydbtable SHARED ${SRCS})
#SET_TARGET_PROPERTIES(world_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(world_service PROPERTIES VERSION 0.1 SOVERSION 1)


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(mydbtable sql_options)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(mydbtable
 protobuf
 sql_options
 ${JEMALLOC_LIB_NAME}
)
#cotire(mydbtable)
