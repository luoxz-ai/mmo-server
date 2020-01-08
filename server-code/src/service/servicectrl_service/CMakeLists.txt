MESSAGE(STATUS "cmake: servicectrl_service")

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
ADD_LIBRARY(servicectrl_service SHARED ${SRCS})
#SET_TARGET_PROPERTIES(servicectrl_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(servicectrl_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(servicectrl_service PROPERTIES CXX_VISIBILITY_PRESET hidden)
cotire(servicectrl_service)

#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(servicectrl_service basecode network mydb myproto mydbtable service_comm gamemap)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(servicectrl_service
 basecode 
 network 
 mydb 
 myproto 
 mydbtable 
 service_comm  
 gamemap
 brpc
 fmt
  ${JEMALLOC_LIB_NAME}
)

