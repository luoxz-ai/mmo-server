MESSAGE(STATUS "cmake: ai_service")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
file(GLOB_RECURSE SHARES "../server_share/*.h" "../server_share/*.hpp")

list(APPEND SRCS ${SHARES})
source_group_by_dir(SRCS)

IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
ELSE()
ENDIF()

IF(USE_JEMALLOC)
ADD_DEFINITIONS(-DUSE_JEMALLOC)
set(JEMALLOC_LIB_NAME "libjemalloc.so")
ELSE()
ENDIF()

# 生成库文件
ADD_LIBRARY(ai_service SHARED ${SRCS})
#SET_TARGET_PROPERTIES(ai_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(ai_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(ai_service PROPERTIES CXX_VISIBILITY_PRESET hidden)
cotire(ai_service)

if(AUTO_GEN_EXPORT_LUA)
GET_DIRECTORY_PROPERTY(includes INCLUDE_DIRECTORIES)
add_custom_target(ai2lua ALL
  COMMAND echo ${SRCS} > /tmp/ai_cpps.txt && echo ${includes} > /tmp/ai_includes.txt && ${BINARIES}/cxx2lua --cpps=/tmp/ai_cpps.txt --include=/tmp/ai_includes.txt --output=${PROJECT_SOURCE_DIR}/src/service/ai_service/script/ai2lua.cpp --output_functionname=ai2lua --skip_cpp  --format=clang-format-9
  DEPENDS cxx2lua
  COMMENT "gen ai2lua for export_lua"
 )
ADD_DEPENDENCIES(ai_service ai2lua)
else()
endif()

#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(ai_service basecode network mydb myproto mydbtable service_comm gamemap script)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(ai_service
 basecode 
 network 
 mydb 
 myproto 
 mydbtable 
 service_comm 
 gamemap 
 script
 fmt
 pthread
 rt
 dl
 ${JEMALLOC_LIB_NAME}
)

