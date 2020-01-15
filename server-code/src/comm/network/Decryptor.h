#ifndef DECRYPTOR_H
#define DECRYPTOR_H


#include "BaseCode.h"

class CDecryptorImpl;
class CDecryptor
{
  public:
	CDecryptor();
	~CDecryptor();

	void Init(uint32_t key);
	void Decryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len);

  private:
	CDecryptorImpl* m_pImpl;
};
#endif /* DECRYPTOR_H */
