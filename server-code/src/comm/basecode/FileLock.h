/**
@file	: filelock.h
@author : V5(weiwutan@gmail.com)
created : 2016.07.11
purpose : 文件锁
*/
#ifndef __FILE_LOCK_H__
#define __FILE_LOCK_H__

#include <string>

class file_lock
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
	int32_t		m_fd;
};

#endif // __FILE_LOCK_H__