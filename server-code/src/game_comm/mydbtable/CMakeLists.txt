MESSAGE(STATUS "cmake: mydb_table")

IF(WIN32)
ELSE()

add_custom_target(mydbtable_protoc-gen-sql ALL
 COMMAND ${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/gen_proto.sh gamedb ${BINARIES} && ${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/gen_proto.sh globaldb ${BINARIES}
 DEPENDS protoc-gen-sql
 COMMENT "gen sql from proto"
) 

add_custom_target(mydbtable_sql2cpp ALL
  COMMAND ${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/gen_sql_def.sh ${BINARIES} gamedb && ${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/gen_sql_def.sh  ${BINARIES} globaldb
  DEPENDS sql2cpp mydbtable_protoc-gen-sql
  COMMENT "gen dbdefine from sql"
 )

ENDIF()

IF(USE_JEMALLOC)
ADD_DEFINITIONS(-DUSE_JEMALLOC)
set(JEMALLOC_LIB_NAME "libjemalloc.so")
ELSE()
ENDIF()


# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
source_group_by_dir(SRCS)
# 生成库文件
ADD_LIBRARY(mydbtable SHARED ${SRCS})
#SET_TARGET_PROPERTIES(world_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(world_service PROPERTIES VERSION 0.1 SOVERSION 1)
cotire(mydbtable)

#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})

IF(WIN32)
ADD_DEPENDENCIES(mydbtable basecode mydb )
ELSE()
ADD_DEPENDENCIES(mydbtable basecode mydb mydbtable_protoc-gen-sql mydbtable_sql2cpp)

ENDIF()

# 指定额外的链接库
TARGET_LINK_LIBRARIES(mydbtable
 libbasecode.so
 libmydb.so
 ${JEMALLOC_LIB_NAME}
)

