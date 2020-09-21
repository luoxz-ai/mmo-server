#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>

#include "BaseCode.h"
#include "CheckUtil.h"
#include "MysqlConnection.h"
#include "MysqlTableCheck.h"
#include "UIDFactory.h"
#include "gamedb.h"
#include "get_opt.h"
#include "globaldb.h"

CMysqlConnectionPtr ConnectGameDB(CMysqlConnection* pGlobalDB, uint32_t worldid)
{
    auto result = pGlobalDB->QueryKeyLimit<TBLD_DBINFO, TBLD_DBINFO::WORLDID>(worldid, 1);
    if(result == nullptr || result->get_num_row() == 0)
    {
        LOGFATAL("CWorldService::Create fail:gamedb info error");
        return nullptr;
    }

    auto row = result->fetch_row(false);
    if(row)
    {
        std::string host   = row->Field(TBLD_DBINFO::DB_IP).get<std::string>();
        uint16_t    port   = row->Field(TBLD_DBINFO::DB_PORT);
        std::string user   = row->Field(TBLD_DBINFO::DB_USER).get<std::string>();
        std::string passwd = row->Field(TBLD_DBINFO::DB_PASSWD).get<std::string>();
        std::string dbname = row->Field(TBLD_DBINFO::DB_NAME).get<std::string>();

        auto pDB = std::make_unique<CMysqlConnection>();
        if(pDB->Connect(host, user, passwd, dbname, port) == false)
        {
            return nullptr;
        }
        return pDB;
    }

    return nullptr;
}

void normal_insert(std::string& merge_sql, CMysqlConnection* pGameDBSrc, const std::string& table_name)
{
    auto result = pGameDBSrc->QueryAll(table_name);
    for(int i = 0; i < result->get_num_row(); i++)
    {
        auto record     = result->fetch_row(false);
        auto insert_sql = record->DumpInsertSQL();
        if(insert_sql.empty() == true)
        {
            LOGERROR("table:{} dump:{} error.", table_name, i);
            ::abort();
        }
        merge_sql += fmt::format("{};\n", insert_sql);
    }
}

bool merge_server(std::string& merge_sql, CMysqlConnection* pGameDBSrc, CMysqlConnection* pGameDBTarget)
{
    // delete no need

    // select than insert
    normal_insert(merge_sql, pGameDBSrc, TBLD_COMMONDATA::table_name());
    // more and more

    return true;
}

bool modify_db_route(std::string& merge_sql, CMysqlConnection* pGlobalDB, uint32_t worldid_src, uint32_t worldid_target)
{
    return true;
}

int main(int argc, char** argv)
{

    __ENTER_FUNCTION
    get_opt opt(argc, (const char**)argv);

    std::string globaldb_host   = opt["--ghost"];
    std::string globaldb_port   = opt["--gport"];
    std::string globaldb_user   = opt["--guser"];
    std::string globaldb_passwd = opt["--gpasswd"];
    std::string globaldb_dbname = opt["--gdbname"];

    std::string zone_src       = opt["--zone_src"];
    uint32_t    zone_src_id    = atoi(zone_src.c_str());
    std::string zone_target    = opt["--zone_target"];
    uint32_t    zone_target_id = atoi(zone_target.c_str());

    BaseCode::InitLog("/data/log/merge-server");
    BaseCode::SetNdc("merge-server");
    CMysqlConnection globaldb;
    CHECKF(globaldb.Connect(globaldb_host, globaldb_user, globaldb_passwd, globaldb_dbname, atoi(globaldb_port.c_str())));

    auto zonedb_src = ConnectGameDB(&globaldb, zone_src_id);
    CHECKF(zonedb_src);
    auto zonedb_target = ConnectGameDB(&globaldb, zone_target_id);
    CHECKF(zonedb_target);

    std::string merge_sql_zone;
    std::string merge_sql_global;
    CHECKF(merge_server(merge_sql_zone, zonedb_src.get(), zonedb_target.get()));
    CHECKF(modify_db_route(merge_sql_global, &globaldb, zone_src_id, zone_target_id));

    std::string out_dir       = opt["--outdir"];
    std::string out_file_name = opt["--output"];
    {
        std::string   output_file_name = out_dir + out_file_name + ".zone.sql";
        std::ofstream output_file(output_file_name);
        output_file << merge_sql_zone;
        output_file.close();
    }
    {
        std::string   output_file_name = out_dir + out_file_name + ".global.sql";
        std::ofstream output_file(output_file_name);
        output_file << merge_sql_global;
        output_file.close();
    }

    return 0;
    __LEAVE_FUNCTION
    return -1;
}
