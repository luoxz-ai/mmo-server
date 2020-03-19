cmake_minimum_required(VERSION 3.0)



IF(GLOBALDB)
configure_file (
  "${PROJECT_SOURCE_DIR}/init_globalservice.sql.template"
  "${PROJECT_BINARY_DIR}/init_globalservice.sql"
  )
ENDIF()


IF(ZONEDB)
configure_file (
  "${PROJECT_SOURCE_DIR}/add_new_server.sql.template"
  "${PROJECT_BINARY_DIR}/zone_init_${ZONEDB}.sql"
  )
ENDIF()