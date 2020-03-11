#include "globaldb.h"

#include "DBField.h"

constexpr const char*	 TBLD_DBINFO::table_name;
constexpr const char*	 TBLD_DBINFO::field_name[];
constexpr DB_FIELD_TYPES TBLD_DBINFO::field_type_enum_list[];
constexpr bool			 TBLD_DBINFO::pri_key_idx[];

constexpr const char*	 TBLD_GLOBALVAR::table_name;
constexpr const char*	 TBLD_GLOBALVAR::field_name[];
constexpr DB_FIELD_TYPES TBLD_GLOBALVAR::field_type_enum_list[];
constexpr bool			 TBLD_GLOBALVAR::pri_key_idx[];

constexpr const char*	 TBLD_GMLIST::table_name;
constexpr const char*	 TBLD_GMLIST::field_name[];
constexpr DB_FIELD_TYPES TBLD_GMLIST::field_type_enum_list[];
constexpr bool			 TBLD_GMLIST::pri_key_idx[];

constexpr const char*	 TBLD_SERVERLIST::table_name;
constexpr const char*	 TBLD_SERVERLIST::field_name[];
constexpr DB_FIELD_TYPES TBLD_SERVERLIST::field_type_enum_list[];
constexpr bool			 TBLD_SERVERLIST::pri_key_idx[];

constexpr const char*	 TBLD_SERVICEDETAIL::table_name;
constexpr const char*	 TBLD_SERVICEDETAIL::field_name[];
constexpr DB_FIELD_TYPES TBLD_SERVICEDETAIL::field_type_enum_list[];
constexpr bool			 TBLD_SERVICEDETAIL::pri_key_idx[];
