#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>

#include "export_lua.h"

inline FILE* fopen_s(FILE** fp, const char* path, const char* mode)
{
    if(fp == nullptr)
        return nullptr;
    *fp = std::fopen(path, mode);
    return *fp;
}

constexpr unsigned char CODE_UTF_LEAD_0 = 0xefU;
constexpr unsigned char CODE_UTF_LEAD_1 = 0xbbU;
constexpr unsigned char CODE_UTF_LEAD_2 = 0xbfU;

inline void skip_utf8_bom(FILE* fp)
{
    if(fp == NULL)
        return;
    unsigned char ch1 = fgetc(fp);
    unsigned char ch2 = fgetc(fp);
    unsigned char ch3 = fgetc(fp);
    if(!(ch1 == CODE_UTF_LEAD_0 && ch2 == CODE_UTF_LEAD_1 && ch3 == CODE_UTF_LEAD_2)) //不等于BOM头时，重新回退到文件头
        fseek(fp, 0, SEEK_SET);
}

export_lua inline bool scan_dir(const std::string&                                          parent_path,
                                const std::string&                                          path,
                                bool                                                        bRecursive,
                                std::function<void(const std::string&, const std::string&)> func)
{
    std::string cur_dir = parent_path + "/" + path;
    DIR*        dp      = opendir(cur_dir.c_str());
    if(dp == nullptr)
    {
        return false;
    }

    struct dirent* entry = readdir(dp);
    while(entry != nullptr)
    {
        struct stat statbuf;
        memset(&statbuf, 0, sizeof(statbuf));
        lstat(entry->d_name, &statbuf); // 获取下一级成员属性
        if(S_IFDIR & statbuf.st_mode)   // 判断下一级成员是否是目录
        {
            if(std::string(".") == entry->d_name || std::string("..") == entry->d_name)
            {
                entry = readdir(dp);
                continue;
            }

            if(bRecursive)
            {
                scan_dir(cur_dir, entry->d_name, bRecursive, func); // 递归调用自身，扫描下一级目录的内容
            }
        }
        else
        {
            func(cur_dir, entry->d_name);
        }

        entry = readdir(dp);
    }
    closedir(dp);
    return true;
}

#endif /* FILEUTIL_H */
