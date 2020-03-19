#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>

#include "MysqlConnection.h"
#include "UIDFactory.h"
#include "gamedb.h"
int main()
{
	__ENTER_FUNCTION
	BaseCode::SetNdc("test_ndc");
	CMysqlConnection mysql_conn;
	mysql_conn.Connect("127.0.0.1", "root", "123456", "zone_1001", 3306, 0, true);
	mysql_conn._AddFieldInfo(TBLD_PLAYER::table_name, std::make_shared<CDDLFieldInfoList<TBLD_PLAYER>>());
	mysql_conn._AddFieldInfo(TBLD_ITEM::table_name, std::make_shared<CDDLFieldInfoList<TBLD_ITEM>>());

	{
		auto result_ptr = mysql_conn.UnionQuery("SELECT * FROM tbld_player");
		if(result_ptr)
		{
			for(size_t i = 0; i < result_ptr->get_num_row(); i++)
			{
				auto		db_record_ptr = result_ptr->fetch_row(true);
				uint64_t	id			  = db_record_ptr->Field("id");
				std::string name		  = db_record_ptr->Field("name");
				int8_t		dwProf		  = db_record_ptr->Field(TBLD_PLAYER::PROF);

				db_record_ptr->Field(TBLD_PLAYER::PROF) = (int8_t)(dwProf + 1);
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
				auto		db_record_ptr = result_ptr->fetch_row(true);
				uint64_t	id			  = db_record_ptr->Field("id");
				std::string name		  = db_record_ptr->Field("name");
				int8_t		dwProf		  = db_record_ptr->Field(TBLD_PLAYER::PROF);

				db_record_ptr->Field(TBLD_PLAYER::PROF) = (int8_t)(dwProf + 1);
				db_record_ptr->Update(false);
			}
		}
	}

	{
		CUIDFactory uid_factory;
		uid_factory.Init(1000, 11);

		OBJID idPlayer													 = uid_factory.CreateID();
		auto  db_record_ptr												 = mysql_conn.MakeRecord(TBLD_PLAYER::table_name);
		db_record_ptr->Field(TBLD_PLAYER::ID)							 = idPlayer;
		db_record_ptr->Field("name")									 = "test2";
		char szBuf[1024]												 = {"test3"};
		db_record_ptr->Field(TBLD_PLAYER::field_name[TBLD_PLAYER::NAME]) = szBuf;
		fmt::format_to_n(szBuf, 1024, "test{}", idPlayer);
		db_record_ptr->Field(TBLD_PLAYER::field_name[TBLD_PLAYER::NAME]) = szBuf;

		try
		{
			db_record_ptr->Field("prof") = 1; // type check error, not exec
		}
		catch(...)
		{
		}
		db_record_ptr->Field(TBLD_PLAYER::PROF) = (int8_t)2;

		try
		{
			db_record_ptr->Field("lev") = 1; // type check error, not exec
		}
		catch(...)
		{
		}
		db_record_ptr->Field(TBLD_PLAYER::LEV) = (int16_t)2;

		try
		{
			db_record_ptr->Field("exp") = 1; // type check error, not exec
		}
		catch(...)
		{
		}
		db_record_ptr->Field(TBLD_PLAYER::EXP) = (uint32_t)2;
		db_record_ptr->Field(TBLD_PLAYER::EXP) = 0xF0000FFF;

		db_record_ptr->Update(true);
		uint64_t id = db_record_ptr->Field(TBLD_PLAYER::ID);
		assert(id != 0);
	}

	mysql_conn.JoinAsyncThreadFinish();

	__LEAVE_FUNCTION
}