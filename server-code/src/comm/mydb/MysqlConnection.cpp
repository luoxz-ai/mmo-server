#include "MysqlConnection.h"

#include "mysql/errmsg.h"

constexpr int32_t MAX_PING_TIMES_PER_QUERY = 10;

CMysqlConnection::CMysqlConnection()
    : m_pHandle(nullptr, &mysql_close)
    , m_pAsyncHandle(nullptr, &mysql_close)
{
}

CMysqlConnection::~CMysqlConnection()
{
    if(m_AsyncThread)
    {
        m_AsyncThread->Stop();
        m_AsyncThread->Join();
    }
    std::string* pStr = nullptr;
    while(m_MessageQueue.get(pStr))
    {
        SAFE_DELETE(pStr);
    }
    m_MysqlFieldInfoCache.clear();

    if(m_pAsyncHandle.get())
    {
        auto host_info = mysql_get_host_info(m_pAsyncHandle.get());
        if(host_info)
            LOGDBDEBUG("MysqlClose:{}", host_info);
        m_pAsyncHandle.reset();
    }
    if(m_pHandle.get())
    {
        auto host_info = mysql_get_host_info(m_pHandle.get());
        if(host_info)
            LOGDBDEBUG("MysqlClose:{}", host_info);

        m_pHandle.reset();
    }
}

bool CMysqlConnection::Connect(const std::string& host,
                               const std::string& user,
                               const std::string& password,
                               const std::string& db,
                               uint32_t           port,
                               unsigned long      client_flag,
                               bool               bCreateAsync)
{
    __ENTER_FUNCTION

    m_pHandle.reset(mysql_init(0));
    int32_t nError = 0;
    // mysql_options(m_pHandle.get(), MYSQL_SET_CHARSET_NAME, MYSQL_AUTODETECT_CHARSET_NAME);
    nError                  = mysql_options(m_pHandle.get(), MYSQL_SET_CHARSET_NAME, "utf8");
    nError                  = mysql_set_character_set(m_pHandle.get(), "utf8");
    int32_t nConnectTimeOut = 5;
    nError                  = mysql_options(m_pHandle.get(), MYSQL_OPT_CONNECT_TIMEOUT, &nConnectTimeOut);
    int32_t nWriteTimeOut   = 1;
    nError                  = mysql_options(m_pHandle.get(), MYSQL_OPT_WRITE_TIMEOUT, &nWriteTimeOut);
    int32_t nReadTimeOut    = 1;
    nError                  = mysql_options(m_pHandle.get(), MYSQL_OPT_READ_TIMEOUT, &nReadTimeOut);
    bool bReconnect         = true;
    nError                  = mysql_options(m_pHandle.get(), MYSQL_OPT_RECONNECT, &bReconnect);
    if(nError)
    {
        const char* error_str = mysql_error(m_pHandle.get());
        LOGDBERROR("mysql_error:{} when set_options.", error_str);
    }
    MYSQL* h = mysql_real_connect(m_pHandle.get(),
                                  host.c_str(),
                                  user.c_str(),
                                  password.c_str(),
                                  db.c_str(),
                                  port,
                                  nullptr,
                                  client_flag | CLIENT_REMEMBER_OPTIONS);
    if(h == nullptr || h != m_pHandle.get())
    {
        // log error
        LOGDBFATAL("mysql connect to {}:{} {} fail.", host.c_str(), port, db.c_str());
        return false;
    }

    // std::string query = "show variables like '%char%'";
    // auto result = UnionQuery(query);
    // for (uint32_t i = 0; i < result->get_num_row(); i++)
    //{
    //	auto row = result->fetch_row(false);
    //	std::string name = row->Field(0);
    //	std::string val = row->Field(1);
    //	int32_t k = 0;
    //}
    // result.reset();

    if(bCreateAsync)
    {
        m_AsyncThread = std::make_unique<CWorkerThread>(
            "DB AsyncThread",
            [this, host, user, password, db, port, client_flag]() {
                __ENTER_FUNCTION
                LOGMESSAGE("ThreadID:{}", get_cur_thread_id());

                int32_t nError = 0;
                m_pAsyncHandle.reset(mysql_init(0));
                // mysql_options(m_pAsyncHandle.get(), MYSQL_SET_CHARSET_NAME, MYSQL_AUTODETECT_CHARSET_NAME);
                nError                  = mysql_options(m_pAsyncHandle.get(), MYSQL_SET_CHARSET_NAME, "utf8");
                nError                  = mysql_set_character_set(m_pAsyncHandle.get(), "utf8");
                int32_t nConnectTimeOut = 10;
                nError = mysql_options(m_pAsyncHandle.get(), MYSQL_OPT_CONNECT_TIMEOUT, &nConnectTimeOut);
                int32_t nWriteTimeOut = 5;
                nError                = mysql_options(m_pAsyncHandle.get(), MYSQL_OPT_WRITE_TIMEOUT, &nWriteTimeOut);
                int32_t nReadTimeOut  = 5;
                nError                = mysql_options(m_pAsyncHandle.get(), MYSQL_OPT_READ_TIMEOUT, &nReadTimeOut);
                bool bReconnect       = true;
                nError                = mysql_options(m_pAsyncHandle.get(), MYSQL_OPT_RECONNECT, &bReconnect);
                if(nError)
                {
                    const char* error_str = mysql_error(m_pAsyncHandle.get());
                    LOGDBERROR("mysql_error:{} when set_options.", error_str);
                }

                MYSQL* h = mysql_real_connect(m_pAsyncHandle.get(),
                                              host.c_str(),
                                              user.c_str(),
                                              password.c_str(),
                                              db.c_str(),
                                              port,
                                              nullptr,
                                              client_flag | CLIENT_REMEMBER_OPTIONS);
                if(h == nullptr || h != m_pAsyncHandle.get())
                {
                    LOGDBFATAL("mysql async connect to {}:{} {} fail.", host.c_str(), port, db.c_str());
                    // log error
                }
                __LEAVE_FUNCTION
            },
            []() { BaseCode::ClearNdc(); });

        while(m_AsyncThread->IsReady() == false)
        {
            std::this_thread::yield();
        }
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CMysqlConnection::AsyncExec(const std::string& s)
{
    __ENTER_FUNCTION
    if(m_AsyncThread)
    {
        m_AsyncThread->AddJob([this, s]() {
            for(int32_t i = 0; i < MAX_PING_TIMES_PER_QUERY; i++)
            {
                int32_t nError     = mysql_real_query(m_pAsyncHandle.get(), s.c_str(), s.size());
                if(nError == 0)
                {
                    while(mysql_more_results(m_pAsyncHandle.get()))
                    {
                        mysql_next_result(m_pAsyncHandle.get());
                    }
                    return;
                }
                else
                {
                    int32_t mysql_erro = mysql_errno(m_pHandle.get());
                    if(mysql_erro != CR_SERVER_LOST)
                    {
                        const char* error_str = mysql_error(m_pHandle.get());
                        // log error
                        LOGDBERROR("mysql_error:{} when async_exec {}.", error_str, s.c_str());
                        return;
                    }
                    else
                    {
                        LOGWARNING("{}", "mysql_lost server, start ping");
                        mysql_ping(m_pAsyncHandle.get());
                        continue;
                    }
                }
            }
        });
    }

    __LEAVE_FUNCTION
}

void CMysqlConnection::JoinAsyncThreadFinish()
{
    __ENTER_FUNCTION
    if(m_AsyncThread)
    {
        m_AsyncThread->Stop();
        m_AsyncThread->Join();
    }
    __LEAVE_FUNCTION
}

CMysqlStmtPtr CMysqlConnection::Prepare(const std::string& s)
{
    __ENTER_FUNCTION
    MYSQL_STMT_PTR stmt(mysql_stmt_init(m_pHandle.get()), mysql_stmt_close);
    if(mysql_stmt_prepare(stmt.get(), s.c_str(), s.size()) == 0)
    {
        return std::make_unique<CMysqlStmt>(stmt.release());
    }
    __LEAVE_FUNCTION
    return nullptr;
}

CMysqlResultPtr CMysqlConnection::UnionQuery(const std::string& query)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_Mutex);

    while(MoreResults())
    {
        NextResult();
    }
    LOGDBDEBUG("UnionQuery:{}", query.c_str());

    for(int32_t i = 0; i < MAX_PING_TIMES_PER_QUERY; i++)
    {
        int32_t     nError     = mysql_real_query(m_pHandle.get(), query.c_str(), query.size());
        if(nError == 0)
        {
            return UseResult(query);
        }
        else
        {
            int32_t     mysql_erro = mysql_errno(m_pHandle.get());
            if(mysql_erro != CR_SERVER_LOST)
            {
                const char* error_str  = mysql_error(m_pHandle.get());
                // log error
                LOGDBERROR("mysql_error:{} when query {}.", error_str, query.c_str());
                return nullptr;
            }
            else
            {
                LOGWARNING("{}", "mysql_lost server, start ping");
                mysql_ping(m_pHandle.get());
                continue;
            }
        }
    }
    __LEAVE_FUNCTION
    return nullptr;
}

uint64_t CMysqlConnection::Insert(const std::string& query)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_Mutex);
    while(MoreResults())
    {
        NextResult();
    }
    LOGDBDEBUG("Insert:{}", query.c_str());

    for(int32_t i = 0; i < MAX_PING_TIMES_PER_QUERY; i++)
    {
        int32_t     nError     = mysql_real_query(m_pHandle.get(), query.c_str(), query.size());
        if(nError == 0)
        {
            mysql_free_result(mysql_use_result(m_pHandle.get()));
            return GetInsertID();
        }
        else
        {
            int32_t     mysql_erro = mysql_errno(m_pHandle.get());
            if(mysql_erro != CR_SERVER_LOST)
            {
                // log error
                const char* error_str  = mysql_error(m_pHandle.get());
                LOGDBERROR("mysql_error:{} when query {}.", error_str, query.c_str());
                return (uint64_t)(-1);
            }
            else
            {
                LOGWARNING("{}", "mysql_lost server, start ping");
                mysql_ping(m_pHandle.get());
                continue;
            }
        }
    }
    __LEAVE_FUNCTION
    return (uint64_t)(-1);
}

uint64_t CMysqlConnection::Update(const std::string& query)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_Mutex);

    LOGDBDEBUG("Update:{}", query.c_str());
    while(MoreResults())
    {
        NextResult();
    }
    for(int32_t i = 0; i < MAX_PING_TIMES_PER_QUERY; i++)
    {
        int32_t     nError     = mysql_real_query(m_pHandle.get(), query.c_str(), query.size());      

        if(nError == 0)
        {
            uint32_t nAffectedRows = GetAffectedRows();
            mysql_free_result(mysql_use_result(m_pHandle.get()));
            return nAffectedRows;
        }
        else
        {
            int32_t     mysql_erro = mysql_errno(m_pHandle.get());
            if(mysql_erro != CR_SERVER_LOST)
            {
                const char* error_str  = mysql_error(m_pHandle.get());
                // log error
                LOGDBERROR("mysql_error:{} when query {}.", error_str, query.c_str());
                return 0;
            }
            else
            {
                LOGWARNING("{}", "mysql_lost server, start ping");
                mysql_ping(m_pHandle.get());
                continue;
            }
        }
    }
    __LEAVE_FUNCTION
    return 0;
}

CMysqlResultPtr CMysqlConnection::Query(const std::string& table_name, const std::string& query)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_Mutex);
    LOGDBDEBUG("Table:{} Query:{}", table_name.c_str(), query.c_str());

    while(MoreResults())
    {
        NextResult();
    }
    for(int32_t i = 0; i < MAX_PING_TIMES_PER_QUERY; i++)
    {

        int32_t nError = 0;
        if(query.empty())
        {
            std::string simple_query = "SELECT * FROM " + table_name;

            nError = mysql_real_query(m_pHandle.get(), simple_query.c_str(), simple_query.size());
        }
        else
        {
            nError = mysql_real_query(m_pHandle.get(), query.c_str(), query.size());
        }

        if(nError == 0)
        {
            return UseResult(table_name);
        }
        else
        {
            int32_t     mysql_erro = mysql_errno(m_pHandle.get());
            if(mysql_erro != CR_SERVER_LOST)
            {
                // log error
                const char* error_str  = mysql_error(m_pHandle.get());

                LOGDBERROR("mysql_error:{} when query {}.", error_str, query.c_str());
                return nullptr;
            }
            else
            {
                LOGWARNING("{}", "mysql_lost server, start ping");
                mysql_ping(m_pHandle.get());
                continue;
            }
        }
    }
    __LEAVE_FUNCTION
    return nullptr;
}

bool CMysqlConnection::SyncExec(const std::string& query)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_Mutex);
    LOGDBDEBUG("SyncExec:{}", query.c_str());

    for(int32_t i = 0; i < MAX_PING_TIMES_PER_QUERY; i++)
    {
        int32_t nError = 0;
        nError         = mysql_real_query(m_pHandle.get(), query.c_str(), query.size());
        if(nError == 0)
        {
            return true;
        }
        else
        {
            // log error
            int32_t     mysql_erro = mysql_errno(m_pHandle.get());
            if(mysql_erro != CR_SERVER_LOST)
            {
                const char* error_str  = mysql_error(m_pHandle.get());
                LOGDBERROR("mysql_error:{} when query {}.", error_str, query.c_str());
                return false;
            }
            else
            {
                LOGWARNING("{}", "mysql_lost server, start ping");
                mysql_ping(m_pHandle.get());
                continue;
            }
        }
    }
    __LEAVE_FUNCTION
    return false;
}

uint64_t CMysqlConnection::GetInsertID()
{
    return mysql_insert_id(m_pHandle.get());
}

uint64_t CMysqlConnection::GetAffectedRows()
{
    return mysql_affected_rows(m_pHandle.get());
}

CDBRecordPtr CMysqlConnection::MakeRecord(const std::string& table_name)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_Mutex);

    while(MoreResults())
    {
        NextResult();
    }
    auto ptr = QueryFieldInfo(table_name);
    if(ptr)
    {
        return std::make_unique<CDBRecord>(this, ptr, true, nullptr, nullptr);
    }
    else
    {
        auto mysql_res  = make_mysql_res_ptr(mysql_list_fields(m_pHandle.get(), table_name.c_str(), ""));
        auto pFieldInfo = CreateFieldInfo(table_name, mysql_res);

        if(pFieldInfo)
            return std::make_unique<CDBRecord>(this, pFieldInfo, true, nullptr, nullptr);
        else
            return CDBRecordPtr();
    }
    __LEAVE_FUNCTION
    return CDBRecordPtr();
}

bool CMysqlConnection::MoreResults()
{
    return mysql_more_results(m_pHandle.get());
}

bool CMysqlConnection::NextResult()
{
    return mysql_next_result(m_pHandle.get());
}

CMysqlResultPtr CMysqlConnection::UseResult(const std::string& query)
{
    __ENTER_FUNCTION
    auto result = make_mysql_res_ptr(mysql_store_result(m_pHandle.get()));
    if(result == nullptr)
    {
        return nullptr;
    }
    auto pFieldInfo = QueryFieldInfo(query);
    if(pFieldInfo == nullptr)
        pFieldInfo = CreateFieldInfo(query, result);

    return std::make_unique<CMysqlResult>(this, std::move(result), pFieldInfo);
    __LEAVE_FUNCTION
    return nullptr;
}

CDBFieldInfoListPtr CMysqlConnection::CreateFieldInfo(const std::string& s, const MYSQL_RES_PTR& res)
{
    if(res == nullptr)
        return nullptr;

    m_MysqlFieldInfoCache.emplace(s, std::make_shared<CMysqlFieldInfoList>(res.get()));
    return QueryFieldInfo(s);
}

CDBFieldInfoListPtr CMysqlConnection::QueryFieldInfo(const std::string& s)
{
    __ENTER_FUNCTION
    auto itFind = m_MysqlFieldInfoCache.find(s);
    if(itFind == m_MysqlFieldInfoCache.end())
    {
        return nullptr;
    }
    else
    {
        return itFind->second;
    }
    __LEAVE_FUNCTION
    return nullptr;
}

void CMysqlConnection::_AddFieldInfo(const std::string& s, CDBFieldInfoListPtr ptr)
{
    if(ptr)
    {
        m_MysqlFieldInfoCache[s] = ptr;
    }
}

bool CMysqlConnection::EscapeString(char* pszDst, const char* pszSrc, int32_t nLen)
{
    return mysql_real_escape_string(m_pHandle.get(), pszDst, pszSrc, nLen);
}

//////////////////////////////////////////////////////////////////////////

CMysqlResult::CMysqlResult(CMysqlConnection* pMysqlConnection, MYSQL_RES_PTR&& res, CDBFieldInfoListPtr infolist_ptr)
    : m_pMysqlConnection(pMysqlConnection)
    , m_MySqlResult(std::move(res))
    , m_pFieldInfoList(infolist_ptr)
{
}

CMysqlResult::~CMysqlResult() {}

CDBFieldInfoListPtr CMysqlResult::GetFieldInfo() const
{
    return m_pFieldInfoList;
}

size_t CMysqlResult::get_num_row()
{
    __ENTER_FUNCTION
    if(m_MySqlResult)
        return mysql_num_rows(m_MySqlResult.get());

    __LEAVE_FUNCTION
    return 0;
}

CDBRecordPtr CMysqlResult::fetch_row(bool bModify /*= true*/)
{
    __ENTER_FUNCTION
    if(m_MySqlResult)
    {
        MYSQL_ROW row = mysql_fetch_row(m_MySqlResult.get());
        if(row != nullptr)
        {
            //将row转换为Record
            return std::make_unique<CDBRecord>(m_pMysqlConnection,
                                               m_pFieldInfoList,
                                               bModify,
                                               row,
                                               mysql_fetch_lengths(m_MySqlResult.get()));
        }
    }
    __LEAVE_FUNCTION
    return nullptr;
}
