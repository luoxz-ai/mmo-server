#include "gamedb.h"

#include "DBField.h"

constexpr const char*    TBLD_COMMONDATA::table_name;
constexpr const char*    TBLD_COMMONDATA::field_name[];
constexpr DB_FIELD_TYPES TBLD_COMMONDATA::field_type_enum_list[];
constexpr bool           TBLD_COMMONDATA::pri_key_idx[];

constexpr const char*    TBLD_COOLDOWN::table_name;
constexpr const char*    TBLD_COOLDOWN::field_name[];
constexpr DB_FIELD_TYPES TBLD_COOLDOWN::field_type_enum_list[];
constexpr bool           TBLD_COOLDOWN::pri_key_idx[];

constexpr const char*    TBLD_DATACOUNT::table_name;
constexpr const char*    TBLD_DATACOUNT::field_name[];
constexpr DB_FIELD_TYPES TBLD_DATACOUNT::field_type_enum_list[];
constexpr bool           TBLD_DATACOUNT::pri_key_idx[];

constexpr const char*    TBLD_GLOBAL_MAIL::table_name;
constexpr const char*    TBLD_GLOBAL_MAIL::field_name[];
constexpr DB_FIELD_TYPES TBLD_GLOBAL_MAIL::field_type_enum_list[];
constexpr bool           TBLD_GLOBAL_MAIL::pri_key_idx[];

constexpr const char*    TBLD_GUILD::table_name;
constexpr const char*    TBLD_GUILD::field_name[];
constexpr DB_FIELD_TYPES TBLD_GUILD::field_type_enum_list[];
constexpr bool           TBLD_GUILD::pri_key_idx[];

constexpr const char*    TBLD_ITEM::table_name;
constexpr const char*    TBLD_ITEM::field_name[];
constexpr DB_FIELD_TYPES TBLD_ITEM::field_type_enum_list[];
constexpr bool           TBLD_ITEM::pri_key_idx[];

constexpr const char*    TBLD_MAIL::table_name;
constexpr const char*    TBLD_MAIL::field_name[];
constexpr DB_FIELD_TYPES TBLD_MAIL::field_type_enum_list[];
constexpr bool           TBLD_MAIL::pri_key_idx[];

constexpr const char*    TBLD_MAIL_ATTACHMENT::table_name;
constexpr const char*    TBLD_MAIL_ATTACHMENT::field_name[];
constexpr DB_FIELD_TYPES TBLD_MAIL_ATTACHMENT::field_type_enum_list[];
constexpr bool           TBLD_MAIL_ATTACHMENT::pri_key_idx[];

constexpr const char*    TBLD_PET::table_name;
constexpr const char*    TBLD_PET::field_name[];
constexpr DB_FIELD_TYPES TBLD_PET::field_type_enum_list[];
constexpr bool           TBLD_PET::pri_key_idx[];

constexpr const char*    TBLD_PLAYER::table_name;
constexpr const char*    TBLD_PLAYER::field_name[];
constexpr DB_FIELD_TYPES TBLD_PLAYER::field_type_enum_list[];
constexpr bool           TBLD_PLAYER::pri_key_idx[];

constexpr const char*    TBLD_PLAYER_GUILDINFO::table_name;
constexpr const char*    TBLD_PLAYER_GUILDINFO::field_name[];
constexpr DB_FIELD_TYPES TBLD_PLAYER_GUILDINFO::field_type_enum_list[];
constexpr bool           TBLD_PLAYER_GUILDINFO::pri_key_idx[];

constexpr const char*    TBLD_SKILL::table_name;
constexpr const char*    TBLD_SKILL::field_name[];
constexpr DB_FIELD_TYPES TBLD_SKILL::field_type_enum_list[];
constexpr bool           TBLD_SKILL::pri_key_idx[];

constexpr const char*    TBLD_TASK::table_name;
constexpr const char*    TBLD_TASK::field_name[];
constexpr DB_FIELD_TYPES TBLD_TASK::field_type_enum_list[];
constexpr bool           TBLD_TASK::pri_key_idx[];

constexpr const char*    TBLD_ACHIEVEMENT::table_name;
constexpr const char*    TBLD_ACHIEVEMENT::field_name[];
constexpr DB_FIELD_TYPES TBLD_ACHIEVEMENT::field_type_enum_list[];
constexpr bool           TBLD_ACHIEVEMENT::pri_key_idx[];

constexpr const char*    TBLD_STATUS::table_name;
constexpr const char*    TBLD_STATUS::field_name[];
constexpr DB_FIELD_TYPES TBLD_STATUS::field_type_enum_list[];
constexpr bool           TBLD_STATUS::pri_key_idx[];

constexpr const char*    TBLD_SYSTEMVAR::table_name;
constexpr const char*    TBLD_SYSTEMVAR::field_name[];
constexpr DB_FIELD_TYPES TBLD_SYSTEMVAR::field_type_enum_list[];
constexpr bool           TBLD_SYSTEMVAR::pri_key_idx[];
