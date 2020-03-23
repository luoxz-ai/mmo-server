#include "Encryptor.h"

class CEncryptorImpl
{
public:
    CEncryptorImpl() {}
    ~CEncryptorImpl() {}

    void Init(uint32_t key) {}

    void Encryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len) {}

private:
};

CEncryptor::CEncryptor()
{
    m_pImpl = new CEncryptorImpl;
}

CEncryptor::~CEncryptor()
{
    SAFE_DELETE(m_pImpl);
}

void CEncryptor::Init(uint32_t key)
{
    __ENTER_FUNCTION
    m_pImpl->Init(key);
    __LEAVE_FUNCTION
}

void CEncryptor::Encryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len)
{
    __ENTER_FUNCTION
    m_pImpl->Encryptor(in_buffer, in_len, out_buffer, out_len);
    __LEAVE_FUNCTION
}
