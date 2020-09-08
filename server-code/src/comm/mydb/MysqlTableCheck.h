#ifndef MYSQLTABLECHECK_H
#define MYSQLTABLECHECK_H

#include "MysqlConnection.h"

struct MysqlTableCheck
{
    template<class T>
    static bool CheckTable(CMysqlConnection* pDB)
    {
        __ENTER_FUNCTION
        auto ptr              = pDB->MakeRecord(T::table_name());
        auto pFieldInfo_MYSQL = pDB->QueryFieldInfo(T::table_name());
        if(pFieldInfo_MYSQL == nullptr)
        {
            LOGFATAL("GameDB Check Error, table:{} not find in MYSQL", T::table_name());
            return false;
        }
        auto pFieldInfo_DDL = std::make_unique<CDDLFieldInfoList<T>>();
        if(pFieldInfo_DDL->size_fields == 0)
        {
            LOGFATAL("GameDB Check Error, table:{} not find in ddl", T::table_name());
            return false;
        }
        for(size_t i = 0; i < pFieldInfo_DDL->size_fields; i++)
        {
            const CDBFieldInfo* pInfo_DDL   = pFieldInfo_DDL->get(i);
            const CDBFieldInfo* pInfo_MYSQL = pFieldInfo_MYSQL->get(i);
            if(pInfo_DDL == nullptr || pInfo_MYSQL == nullptr)
            {
                LOGFATAL("GameDB Check Error, table:{}, field:{}", T::table_name(), i);
                return false;
            }
            else if(pInfo_DDL->GetFieldType() != pInfo_MYSQL->GetFieldType())
            {
                LOGFATAL("GameDB Check Error, table:{}, field:{} ddl_field:{}  mysql_field:{} ddl_fieldt:{}  mysql_fieldt:{}",
                         T::table_name(),
                         i,
                         pInfo_DDL->GetFieldName(),
                         pInfo_MYSQL->GetFieldName(),
                         pInfo_DDL->GetFieldType(),
                         pInfo_MYSQL->GetFieldType());
                return false;
            }
        }
        return true;
        __LEAVE_FUNCTION
        return false;
    }





    template<typename TupleType>
    static inline bool CheckAllTableImpl(CMysqlConnection* pDB, std::index_sequence<>)
    {
        return true;
    }
    template<typename TupleType, std::size_t first, std::size_t... is>
    static inline bool CheckAllTableImpl(CMysqlConnection* pDB, std::index_sequence<first, is...>)
    {
        using TableType = typename std::tuple_element<first, TupleType>::type;
        return CheckTable<TableType>(pDB) && CheckAllTableImpl<TupleType>(pDB, std::index_sequence<is...>{});
    }
    
 
    template<class type_list_Table>
    static inline bool CheckAllTable(CMysqlConnection* pDB)
    {
        using TupleType = typename type_list_Table::tuple_type;
        constexpr size_t tuple_size = type_list_Table::size;
        return CheckAllTableImpl<TupleType>(pDB, std::make_index_sequence<tuple_size>{});
    }
};

#endif /* MYSQLTABLECHECK_H */
