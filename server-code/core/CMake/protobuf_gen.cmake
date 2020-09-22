# EN: Largely copied from
# https://raw.githubusercontent.com/IvanSafonov/grpc-cmake-example/master/cmake/FindProtobuf.cmake
# but with some modifications to overcome more .proto layout issues.
#
# Locate and configure the Google Protocol Buffers library
#
# Adds the following targets:
#
#  protobuf::libprotobuf - Protobuf library
#  protobuf::libprotobuf-lite - Protobuf lite library
#  protobuf::libprotoc - Protobuf Protoc Library
#  protobuf::protoc - protoc executable
#

#
# Generates C++ sources from the .proto files
#
# protobuf_generate_cpp (<SRCS> <HDRS> <DEST> [<ARGN>...])
#
#  SRCS - variable to define with autogenerated source files
#  HDRS - variable to define with autogenerated header files
#  DEST - directory where the source files will be created
#  ARGN - .proto files
#
function(PROTOBUF_GENERATE_CPP_NEW SRCS HDRS DEST)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_CPP_NEW() called without any proto files")
    return()
  endif()

  # EN: This is customized to allow generating stubs for nested protos
  #     from a specified base path since by default this only works properly
  #     when run from within the same directory.
  #     https://stackoverflow.com/questions/19343018/cmake-and-findprotobuf
  if (DEFINED BASE_PROTO_SOURCE_DIR_OVERRIDE)
    set(BASE_PROTO_SOURCE_DIR ${BASE_PROTO_SOURCE_DIR_OVERRIDE})
  else()
    set(BASE_PROTO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if(PROTOBUF_GENERATE_CPP_NEW_APPEND_PATH)
    # Create an include path for each file specified
    foreach(FIL ${ARGN})
      get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
      get_filename_component(ABS_PATH ${ABS_FIL} PATH)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  else()
    set(_protobuf_include_path -I ${BASE_PROTO_SOURCE_DIR})
  endif()

  if(DEFINED PROTOBUF_IMPORT_DIRS)
    foreach(DIR ${PROTOBUF_IMPORT_DIRS})
      get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  endif()

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)

    #message("ABS_FIL" ${BAS_FIL})
    #message("FIL_WE" ${FIL_WE})
    if (PROTOBUF_GENERATE_CPP_NEW_APPEND_PATH)
      set(RESOLVED_DEST "${DEST}")
    else()
      # EN: This is customized to allow generating stubs for nested protos
      #     from the main directory since by default this only works properly
      #     when run from within the same directory.
      #     https://stackoverflow.com/questions/19343018/cmake-and-findprotobuf
      file(RELATIVE_PATH
        ITEM_PATH_REL
        ${BASE_PROTO_SOURCE_DIR}
        ${ABS_FIL}
      )
      #message("RELATIVE_PATH" ${RELATIVE_PATH})
      #message("ITEM_PATH_REL" ${ITEM_PATH_REL})
      get_filename_component(RELATIVE_DIR ${ITEM_PATH_REL} DIRECTORY)
      set(RESOLVED_DEST "${DEST}/${RELATIVE_DIR}")
    endif()
    #message("RESOLVED_DEST" ${RESOLVED_DEST})
    list(APPEND ${SRCS} "${RESOLVED_DEST}/${FIL_WE}.pb.cc")
    list(APPEND ${HDRS} "${RESOLVED_DEST}/${FIL_WE}.pb.h")

    add_custom_command(
      OUTPUT "${RESOLVED_DEST}/${FIL_WE}.pb.cc"
             "${RESOLVED_DEST}/${FIL_WE}.pb.h"
      COMMAND protobuf::protoc
      ARGS --cpp_out ${DEST} ${_protobuf_include_path} ${ABS_FIL}
      DEPENDS ${ABS_FIL} protobuf::protoc
      COMMENT "Running C++ protocol buffer compiler on ${FIL}"
      VERBATIM )
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()
