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
ADD_LIBRARY(ai_service SHARED ${SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/script/ai2lua.cpp)
#SET_TARGET_PROPERTIES(ai_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(ai_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(ai_service PROPERTIES CXX_VISIBILITY_PRESET hidden)



if(AUTO_GEN_EXPORT_LUA)
add_dependencies(ai_service cxx2lua)
GET_DIRECTORY_PROPERTY(includes INCLUDE_DIRECTORIES)
add_custom_command(TARGET ai_service
PRE_BUILD
COMMAND echo ${SRCS} > /tmp/ai_cpps.txt && echo ${includes} > /tmp/ai_includes.txt 
COMMAND ${BINARIES_CORE}/cxx2lua --cpps=/tmp/ai_cpps.txt --include=/tmp/ai_includes.txt --output=${CMAKE_CURRENT_SOURCE_DIR}/script/ai2lua.cpp --output_functionname=ai2lua --skip_cpp  --format=clang-format-9
COMMENT "gen ai2lua for export_lua"
)

else()
endif()

#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(ai_service basecode network mydb myproto mydbtable service_comm gamemap script)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(ai_service
 libbasecode.so
 libnetwork.so 
 libmydb.so 
 libmyproto.so 
 libmydbtable.so 
 libservice_comm.so 
 libgamemap.so 
 libscript.so
 libfmt.so
 pthread
 rt
 dl
 ${JEMALLOC_LIB_NAME}
)
cotire(ai_service)
