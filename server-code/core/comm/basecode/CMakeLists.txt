MESSAGE(STATUS "cmake: basecode")


	
# 搜索当前目录下所有文件
file(GLOB_RECURSE LIBBASECODE_SRC "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")


GET_DIRECTORY_PROPERTY(LIBBASECODE_INCLUDES INCLUDE_DIRECTORIES)
SET_PROPERTY(GLOBAL PROPERTY LIBBASECODE_INCLUDES ${LIBBASECODE_INCLUDES})
SET_PROPERTY(GLOBAL PROPERTY LIBBASECODE_SRC ${LIBBASECODE_SRC})


# 生成库文件
ADD_LIBRARY(basecode SHARED ${LIBBASECODE_SRC})

#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(basecode
	PUBLIC libfmt.a
	PRIVATE protobuf
	PRIVATE ${JEMALLOC_LIB_NAME}
	PUBLIC ${LIB_FS}
	PRIVATE bfd
	PUBLIC dl
	PUBLIC pthread
) 
 
cotire(basecode)