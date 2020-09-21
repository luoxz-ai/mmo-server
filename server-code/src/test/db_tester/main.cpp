#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>

#include "MysqlConnection.h"
#include "MysqlTableCheck.h"
#include "UIDFactory.h"
#include "gamedb.h"
int main()
{

    __ENTER_FUNCTION
    BaseCode::InitLog("/data/log/db_tester");
    BaseCode::SetNdc("test_ndc");
    CMysqlConnection mysql_conn;
    mysql_conn.Connect("172.28.2.2", "root", "test12345", "zone_1", 3306, 0, true);
    mysql_conn._AddFieldInfo(TBLD_PLAYER::table_name(), std::make_shared<CDDLFieldInfoList<TBLD_PLAYER>>());
    mysql_conn._AddFieldInfo(TBLD_ITEM::table_name(), std::make_shared<CDDLFieldInfoList<TBLD_ITEM>>());
    CHECKF(MysqlTableCheck::CheckAllTableAndFix<GAMEDB_TABLE_LIST>(&mysql_conn));
    {
        auto result_ptr = mysql_conn.UnionQuery("SELECT * FROM tbld_player");
        if(result_ptr)
        {
            for(size_t i = 0; i < result_ptr->get_num_row(); i++)
            {
                auto        db_record_ptr = result_ptr->fetch_row(true);
                uint64_t    id            = db_record_ptr->Field("id");
                std::string name          = db_record_ptr->Field("name");
                uint32_t    dwProf        = db_record_ptr->Field(TBLD_PLAYER::PROF);

                db_record_ptr->Field(TBLD_PLAYER::PROF) = (uint32_t)(dwProf + 1);
                db_record_ptr->Update(false);
            }
        }
    }
    {
        auto result_ptr = mysql_conn.Query("tbld_player", "SELECT * FROM tbld_player");
        if(result_ptr)
        {
            for(size_t i = 0; i < result_ptr->get_num_row(); i++)
            {
                auto        db_record_ptr = result_ptr->fetch_row(true);
                uint64_t    id            = db_record_ptr->Field("id");
                std::string name          = db_record_ptr->Field("name");
                uint32_t    dwProf        = db_record_ptr->Field(TBLD_PLAYER::PROF);

                db_record_ptr->Field(TBLD_PLAYER::PROF) = (uint32_t)(dwProf + 1);
                db_record_ptr->Update(false);
            }
        }
    }

    {
        CUIDFactory uid_factory;
        uid_factory.Init(1000, 11);

        OBJID idPlayer      = uid_factory.CreateID();
        auto  db_record_ptr = mysql_conn.MakeRecord(TBLD_PLAYER::table_name());

        db_record_ptr->Field(TBLD_PLAYER::ID) = idPlayer;
        db_record_ptr->Field("name")          = "test2";

        char szBuf[1024] = {"test3"};

        db_record_ptr->Field(DBFieldHelp<TBLD_PLAYER, TBLD_PLAYER::NAME>::GetFieldName()) = szBuf;

        fmt::format_to_n(szBuf, 1024, "test{}", idPlayer);
        db_record_ptr->Field(DBFieldHelp<TBLD_PLAYER, TBLD_PLAYER::NAME>::GetFieldName()) = szBuf;

        db_record_ptr->Field("prof")            = (int8_t)1; // type check error, not exec
        db_record_ptr->Field(TBLD_PLAYER::PROF) = (uint32_t)2;

        db_record_ptr->Field("lev")            = (int16_t)1; // type check error, not exec
        db_record_ptr->Field(TBLD_PLAYER::LEV) = (uint32_t)2;

        db_record_ptr->Field("exp")            = (uint32_t)1; // type check error, not exec
        db_record_ptr->Field(TBLD_PLAYER::EXP) = (uint32_t)2; // type check error, not exec
        db_record_ptr->Field(TBLD_PLAYER::EXP) = (uint64_t)0xF0000FFF;

        db_record_ptr->Update(true);
        uint64_t id = db_record_ptr->Field(TBLD_PLAYER::ID);
        assert(id != 0);
    }

    mysql_conn.JoinAsyncThreadFinish();

    __LEAVE_FUNCTION
}