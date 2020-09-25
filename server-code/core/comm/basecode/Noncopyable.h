#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

#include "CheckUtil.h"

#define CreateNewImpl(Type)                                     \
    template<typename... Args>                                  \
    static inline Type* CreateNew(Args&&... args)               \
    {                                                           \
        Type* newT = nullptr;                                   \
        __ENTER_FUNCTION                                        \
        {                                                       \
            newT = new Type();                                  \
            if(newT && newT->Init(std::forward<Args>(args)...)) \
            {                                                   \
                return newT;                                    \
            }                                                   \
        }                                                       \
        __LEAVE_FUNCTION                                        \
        SAFE_DELETE(newT);                                      \
        return nullptr;                                         \
    }

#define CreateNewRealeaseImpl(Type)                             \
    template<typename... Args>                                  \
    static inline Type* CreateNew(Args&&... args)               \
    {                                                           \
        Type* newT = nullptr;                                   \
        __ENTER_FUNCTION                                        \
        {                                                       \
            newT = new Type();                                  \
            if(newT && newT->Init(std::forward<Args>(args)...)) \
            {                                                   \
                return newT;                                    \
            }                                                   \
        }                                                       \
        __LEAVE_FUNCTION                                        \
        SAFE_RELEASE(newT);                                     \
        return nullptr;                                         \
    }

template<class Type>
class NoncopyableT
{
protected:
    NoncopyableT() {}
    virtual ~NoncopyableT() {}

public:
    NoncopyableT(const NoncopyableT&) = delete;
    const NoncopyableT& operator=(const NoncopyableT&) = delete;
};

template<class Type>
class CreateNewT
{
public:
    CreateNewT(){}
    virtual ~CreateNewT(){}

    template<typename... Args>                                  
    static inline Type* CreateNew(Args&&... args)               
    {                                                           
        Type* newT = nullptr;                                   
        __ENTER_FUNCTION                                        
        {                                                       
            newT = new Type();                                  
            if(newT && newT->Init(std::forward<Args>(args)...)) 
            {                                                   
                return newT;                                    
            }                                                   
        }                                                       
        __LEAVE_FUNCTION                                        
        SAFE_DELETE(newT);                                      
        return nullptr;                                         
    }
private:
};

class Noncopyable
{
protected:
    Noncopyable() {}
    virtual ~Noncopyable() {}

public:
    Noncopyable(const Noncopyable&) = delete;
    const Noncopyable& operator=(const Noncopyable&) = delete;
};

#endif /* NONCOPYABLE_H */
