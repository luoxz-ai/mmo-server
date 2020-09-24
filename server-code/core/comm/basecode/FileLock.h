#ifndef __FILE_LOCK_H__
#define __FILE_LOCK_H__

#include <string>

#include "BaseCode.h"
#include "export_lua.h"

export_lua class file_lock
{
public:
    file_lock(const std::string& programname);
    ~file_lock();

    bool kill(int32_t sig);
    bool test();
    bool lock();

    void unlock();

private:
    std::string m_pid_file;
    int32_t     m_fd;
};

#endif // __FILE_LOCK_H__