#ifndef __MD5_H__
#define __MD5_H__

#include <stdio.h>

#include "BaseType.h"
#include "export_lua.h"

export_lua void GetMD5Code(BYTE* pBuf, int nSize, char pCode[34]);
export_lua void GetMD5CodeBig(BYTE* pBuf, int nSize, char pCode[34]);
export_lua std::string md5(const std::string& str);

#endif //__MD5_H__
