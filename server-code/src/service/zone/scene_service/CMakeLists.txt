MESSAGE(STATUS "cmake: scene_service")

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

link_libraries("-Wl,--no-allow-shlib-undefined")

# 生成库文件
ADD_LIBRARY(scene_service SHARED ${SRCS})
#SET_TARGET_PROPERTIES(scene_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(scene_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(scene_service PROPERTIES CXX_VISIBILITY_PRESET hidden)


if(AUTO_GEN_EXPORT_LUA)
GET_DIRECTORY_PROPERTY(includes INCLUDE_DIRECTORIES)
add_custom_target(scene2lua ALL
  COMMAND echo ${SRCS} > /tmp/scene_cpps.txt && echo ${includes} > /tmp/scene_includes.txt && ${BINARIES_CORE}/cxx2lua --cpps=/tmp/scene_cpps.txt --include=/tmp/scene_includes.txt --output=${PROJECT_SOURCE_DIR}/src/service/zone/scene_service/script/scene2lua.cpp --output_functionname=scene2lua --skip_cpp --format=clang-format-9
  DEPENDS cxx2lua
  COMMENT "gen scene2lua for export_lua"
 )
ADD_DEPENDENCIES(scene_service scene2lua)
else()
endif()

 #指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(scene_service basecode network mydb myproto mydbtable service_comm gamemap script)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(scene_service
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

cotire(scene_service)