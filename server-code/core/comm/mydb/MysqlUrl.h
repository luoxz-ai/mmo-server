#ifndef MYSQLURL_H
#define MYSQLURL_H

#include <cstdlib>
#include <regex>
#include <string>
struct MysqlUrl
{
    std::string host;
    std::string user;
    std::string password;
    std::string db;
    uint32_t    port = 0;
};

inline MysqlUrl ParseMysqlUrl(const std::string& url)
{
    MysqlUrl    result;
    std::string regextxt = R"(mysql://(.+):(.+)@(.+):(.+)/(.*))";
    std::smatch base_match;
    if(std::regex_search(url, base_match, std::regex(regextxt)))
    {
        result.user     = base_match[1];
        result.password = base_match[2];
        result.host     = base_match[3];
        result.port     = std::atoi(base_match[4].str().c_str());
        result.db       = base_match[5];
    }

    return result;
}

#endif /* MYSQLURL_H */
