/** \file BaseType.h
 *
 *	filename: 	BaseType.h
 *	purpose:	Definition
 *	created:	2012-3-20  20:41
 *	author:		zlong
 *	purpose:
 *
 *	(c)2006 - 2010 All Rights Reserved
 ****************************************************************************/

#ifndef __BASETYPE_H__
#define __BASETYPE_H__

#define NOMINMAX
/////////////////////////////////////////////////////////////////////////////////
//当前包含的系统头文件引用
/////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <exception>
#include <string>

#include <execinfo.h>
#include <pthread.h>
#include <semaphore.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>

using float32_t = float;
using float64_t = double;

using byte = unsigned char;
using BYTE	= uint8_t;

using OBJID = uint64_t;


using HMODULE = void*;
using HANDLE  = void*;

#ifndef SOCKET
using SOCKET = unsigned int;
#endif

///////////////////////////////////////////////////////////////////////
const OBJID ID_NONE = static_cast<OBJID>(0);

///////////////////////////////////////////////////////////////////////
#define TRUE	 1
#define FALSE	 0
#define INFINITE 0xFFFFFFFF // Infinite timeout

#define INVALID_SOCKET static_cast<SOCKET>(-1)
#define SOCKET_ERROR   static_cast<SOCKET>(-1)

#define LINEEND "\n"

#define FAR
#define NEAR
#define STDCALL

#define __max(a, b) (((a) > (b)) ? (a) : (b))
#define __min(a, b) (((a) < (b)) ? (a) : (b))

#endif //__BASETYPE_H__
