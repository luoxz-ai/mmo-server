MESSAGE(STATUS "cmake: zone_service")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
file(GLOB_RECURSE SHARES "../share/*.h" "../share/*.hpp")

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
ADD_LIBRARY(zone_service SHARED ${SRCS})
#SET_TARGET_PROPERTIES(zone_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(zone_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(zone_service PROPERTIES CXX_VISIBILITY_PRESET hidden)
cotire(zone_service)

if(AUTO_GEN_EXPORT_LUA)
GET_DIRECTORY_PROPERTY(includes INCLUDE_DIRECTORIES)
add_custom_target(zone2lua ALL
  COMMAND echo ${SRCS} > /tmp/zone_cpps.txt && echo ${includes} > /tmp/zone_includes.txt && ${BINARIES}/cxx2lua --cpps=/tmp/zone_cpps.txt --include=/tmp/zone_includes.txt --output=${PROJECT_SOURCE_DIR}/src/service/zone_service/script/zone2lua.cpp --output_functionname=zone2lua --skip_cpp
  DEPENDS cxx2lua
  COMMENT "gen zone2lua for export_lua"
 )
ADD_DEPENDENCIES(zone_service zone2lua)
else()
endif()

 #指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(zone_service basecode network mydb myproto mydbtable service_comm gamemap script)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(zone_service
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

