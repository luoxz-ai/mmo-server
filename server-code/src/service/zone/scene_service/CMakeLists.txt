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






GET_DIRECTORY_PROPERTY(includes INCLUDE_DIRECTORIES)


# make scene_service_gen_lua 生成 scene2lua.cpp
add_custom_target(scene_service_gen_lua
COMMAND echo ${SRCS} > ${CMAKE_CURRENT_BINARY_DIR}/scene_cpps.txt && echo ${includes} > ${CMAKE_CURRENT_BINARY_DIR}/scene_includes.txt 
COMMAND cxx2lua --cpps=${CMAKE_CURRENT_BINARY_DIR}/scene_cpps.txt --include=${CMAKE_CURRENT_BINARY_DIR}/scene_includes.txt 
--output=${CMAKE_CURRENT_BINARY_DIR}/scene2lua.cpp --output_functionname=scene2lua --skip_cpp --format=clang-format-9
COMMENT "gen scene2lua for export_lua"
DEPENDS cxx2lua
 )
# 将scene_service_gen_lua 挂接到 make gen_all_lua
ADD_DEPENDENCIES(gen_all_lua scene_service_gen_lua)


# scene2lua.cpp不存在，则自动生成 scene2lua.cpp
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/scene2lua.cpp
COMMAND echo ${SRCS} > ${CMAKE_CURRENT_BINARY_DIR}/scene_cpps.txt && echo ${includes} > ${CMAKE_CURRENT_BINARY_DIR}/scene_includes.txt 
COMMAND cxx2lua --cpps=${CMAKE_CURRENT_BINARY_DIR}/scene_cpps.txt --include=${CMAKE_CURRENT_BINARY_DIR}/scene_includes.txt 
--output=${CMAKE_CURRENT_BINARY_DIR}/scene2lua.cpp --output_functionname=scene2lua --skip_cpp --format=clang-format-9
IMPLICIT_DEPENDS CXX ${CMAKE_CURRENT_BINARY_DIR}/scene2lua.cpp
COMMENT "gen scene2lua for scene2lua.cpp"
  DEPENDS cxx2lua
)
set_property (SOURCE ${CMAKE_CURRENT_BINARY_DIR}/scene2lua.cpp PROPERTY COTIRE_EXCLUDED  "TRUE")

# 生成库文件
ADD_LIBRARY(scene_service SHARED ${SRCS})
#SET_TARGET_PROPERTIES(scene_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(scene_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(scene_service PROPERTIES CXX_VISIBILITY_PRESET hidden)

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
target_sources(scene_service PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/scene2lua.cpp)