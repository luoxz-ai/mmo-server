MESSAGE(STATUS "cmake: guild_service")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
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
ADD_LIBRARY(guild_service SHARED ${SRCS})
#SET_TARGET_PROPERTIES(guild_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(guild_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(guild_service PROPERTIES CXX_VISIBILITY_PRESET hidden)


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(guild_service basecode network mydb myproto mydbtable service_comm)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(guild_service
 libbasecode.so 
 libnetwork.so 
 libmydb.so 
 libmyproto.so 
 libmydbtable.so 
 libservice_comm.so  
 libfmt.so
 pthread
 rt
 dl
  ${JEMALLOC_LIB_NAME}
)
cotire(guild_service)
