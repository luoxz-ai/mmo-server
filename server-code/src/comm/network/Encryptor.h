#pragma once

#include "BaseCode.h"

class CEncryptorImpl;
class CEncryptor
{
public:
	CEncryptor();
	~CEncryptor();

	void Init(uint32_t key);
	void Encryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len);


private:
	CEncryptorImpl* m_pImpl;
};