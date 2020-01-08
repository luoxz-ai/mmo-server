#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include "export_lua.h"
//跳过utf8_bom头
export_lua void skip_utf8_bom(FILE* fp);
//获取utf8文字长度
export_lua size_t utf8_length(const char* pUTF8, size_t nLen = 0);
//编码转换
export_lua char * ConvertEnc(const char* encFrom, const char* encTo, char* pszBuffIn, size_t lenin, char* pszBuffout, size_t lenout);
//检查控制字符
export_lua bool IsUTF8_NoneControl(const char* pszString, long nSize);

export_lua std::string& ltrim(std::string& ss);
export_lua std::string& rtrim(std::string& ss);
export_lua std::string& trim(std::string& st);

export_lua inline std::vector<std::string> split_string(const std::string & str, const std::string & delimiters)
{
	std::vector<std::string> v;
	std::string::size_type start = 0;
	auto pos = str.find_first_of(delimiters, start);
	while (pos != std::string::npos)
	{
		if (pos != start) // ignore empty tokens
			v.emplace_back(str, start, pos - start);
		start = pos + 1;
		pos = str.find_first_of(delimiters, start);
	}
	if (start < str.length()) // ignore trailing delimiter
		v.emplace_back(str, start, str.length() - start); // add what's left of the string
	return v;
}


export_lua std::string ReplaceStr(std::string& strSource, const std::string& strRepl, const std::string& strNew);
export_lua std::string URLDecode(const char* pszStr);
export_lua std::string URLEncode(const char* pszStr);


export_lua inline std::string GetFileNameFromFullPath(const std::string& szFullPath)
{
	std::string::size_type pos = szFullPath.find_last_of("\\/");
	std::string szName;
	if(pos != std::string::npos){
		szName = szFullPath.substr(pos + 1);
	}else
		szName = szFullPath;
	return szName;
}

export_lua inline std::string GetFileNameWithoutExt(const std::string& szFile)
{
	std::string::size_type pos1 = szFile.find_last_of("/\\");
	std::string::size_type pos2 = szFile.rfind('.');
	pos1 = pos1 == std::string::npos ? 0 : pos1 + 1;
	if(pos2 == std::string::npos)
		pos2 = szFile.size();
	if(pos2 < pos1)
		pos2 = pos1;
	return szFile.substr(pos1, pos2 - pos1);
}

export_lua inline std::string GetFileExt(const std::string& szFile)
{
	std::string szName = GetFileNameFromFullPath(szFile);
	std::string::size_type nPos = szName.rfind('.');
	if(nPos != std::string::npos)
		++nPos;
	else
		nPos = szName.size();

	return szName.substr(nPos);
}

export_lua inline std::string GetBasePath(const std::string& szFullPath)
{
	std::string::size_type pos = szFullPath.find_last_of("/\\");
	std::string szName;
	if(pos != std::string::npos){
		szName = szFullPath.substr(0, pos);
	}else
		szName = szFullPath;
	return szName;	
}

template < typename OI, typename II >
OI remove_sub_sequence(OI first1, OI last1, II first2, II last2)
{
	OI r1 = std::search(first1, last1, first2, last2);
	OI w = r1, r2 = r1;
	const int nDiff = std::distance(first2, last2);
	while(r2 != last1)
	{
		std::advance(r1, nDiff);						// ignore [first2, last2)
		r2 = std::search(r1, last1, first2, last2);		// find next
		while(r1 != r2){								// write element
			*w = *r1;
			++w, ++r1;
		}
	}
	return w;
}


export_lua inline void TrimPath(std::string& szPath)
{
	char c1 = '\\', c2 = '/';
	std::replace(szPath.begin(), szPath.end(), c1, c2);
	std::string szDogDog = std::string("..") + c2;
	std::string::size_type pos = szPath.find(szDogDog);
	for(; pos != std::string::npos && pos > 2; pos = szPath.find(szDogDog)){
		std::string::size_type nLast = szPath.rfind(c2, pos - 2);
		szPath.erase(nLast, pos + szDogDog.size() - nLast - 1);
	}
	std::string szDS = std::string(".") + c2;

	szPath.erase(remove_sub_sequence(szPath.begin(), szPath.end(), szDS.begin(), szDS.end()), szPath.end());
}

export_lua std::string GetFullPath(const std::string& szPath);

export_lua bool ReplaceIllegaWords(std::wstring& wstr);
export_lua bool ReplaceIllegaWords(std::string& utf8);
export_lua bool FindNameError(const std::wstring& wstr);
export_lua bool FindNameError(const std::string& utf8);

export_lua bool RegexStrCheck(const std::string& str);
export_lua bool RegexStrReload();