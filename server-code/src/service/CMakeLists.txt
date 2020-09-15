set(PCH_INCLUDE_PATH "${COTIRE_PREFIX_HEADER_INCLUDE_PATH}" "${PROJECT_SOURCE_DIR}/src/comm" "${PROJECT_SOURCE_DIR}/src/game_comm")
set_directory_properties(PROPERTIES COTIRE_PREFIX_HEADER_INCLUDE_PATH "${PCH_INCLUDE_PATH}")



ADD_SUBDIRECTORY(service_comm)
ADD_SUBDIRECTORY(rpcservice_comm)

ADD_SUBDIRECTORY(zone/socket_service)
ADD_SUBDIRECTORY(zone/world_service)
ADD_SUBDIRECTORY(zone/scene_service)
ADD_SUBDIRECTORY(zone/ai_service)
ADD_SUBDIRECTORY(zone/market_service)
ADD_SUBDIRECTORY(zone/gm_service)
ADD_SUBDIRECTORY(zone/auth_service)

ADD_SUBDIRECTORY(global/gmproxy_service)
ADD_SUBDIRECTORY(global/route_service)




