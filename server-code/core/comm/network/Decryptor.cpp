#include "Decryptor.h"

class CDecryptorImpl
{
public:
    CDecryptorImpl() {}
    ~CDecryptorImpl() {}

    void Init(uint32_t key) {}

    void Decryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len) {}

private:
};

CDecryptor::CDecryptor()
    : m_pImpl{std::make_unique<CDecryptorImpl>()}
{
}

CDecryptor::~CDecryptor() {}

void CDecryptor::Init(uint32_t key)
{
    __ENTER_FUNCTION
    m_pImpl->Init(key);
    __LEAVE_FUNCTION
}

void CDecryptor::Decryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len)
{
    __ENTER_FUNCTION
    m_pImpl->Decryptor(in_buffer, in_len, out_buffer, out_len);
    __LEAVE_FUNCTION
}
