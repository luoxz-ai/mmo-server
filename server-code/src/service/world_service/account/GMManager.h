#ifndef GMMANAGER_H
#define GMMANAGER_H
#include "BaseCode.h"

class CGMManager: public Noncopyable<CGMManager>
{
    CGMManager();
    bool Init();
public:
    CreateNewImpl(CGMManager);
public:
    ~CGMManager();
    
    
    void Destory();
    void Reload();

    uint32_t GetGMLevel(const std::string& openid) const;

private:
    std::map<std::string, uint32_t> m_GMList;
};
#endif /* GMMANAGER_H */
