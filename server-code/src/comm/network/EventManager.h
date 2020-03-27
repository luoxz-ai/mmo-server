#ifndef EventManager_h__
#define EventManager_h__

#include <map>
#include <set>

#include <time.h>

#include "BaseCode.h"
#include "EventEntry.h"

struct event_base;
struct event;

class CEventManager : public Noncopyable<CEventManager>
{
    CEventManager();
    bool Init(event_base* base);
public:
    CreateNewImpl(CEventManager);
    ~CEventManager();

    
    void Destory();

    void   OnTimer();
    size_t GetEventCount();

    void DeleteEntry(CEventEntry*& pEntry);
    bool ScheduleEvent(uint32_t evType, EventCallBackFunc&& cb, time_t tWaitTime, bool bPersist);
    bool ScheduleEvent(uint32_t            evType,
                       EventCallBackFunc&& cb,
                       time_t              tWaitTime,
                       bool                bPersist,
                       CEventEntryPtr&     refEntry);
    bool ScheduleEvent(uint32_t            evType,
                       EventCallBackFunc&& cb,
                       time_t              tWaitTime,
                       bool                bPersist,
                       CEventEntryQueue&   refEntryQueue);
    bool ScheduleEvent(uint32_t            evType,
                       EventCallBackFunc&& cb,
                       time_t              tWaitTime,
                       bool                bPersist,
                       CEventEntryMap&     refEntryMap);
    bool RemoveWait(CEventEntry* pEntry);

protected:
    CEventEntry* ScheduleEvent(uint32_t            evType,
                               EventCallBackFunc&& cb,
                               time_t              tWaitTime,
                               bool                bPersist,
                               CEventEntry*        pEntry);
    CEventEntry* CreateEntry(uint32_t            evType,
                             EventCallBackFunc&& cb        = nullptr,
                             time_t              tWaitTime = 0,
                             bool                bPersist  = false);
    CEventEntry* PushWait(CEventEntry* pEntry);
    void         ScheduleWait();

protected:
    void Delete(CEventEntry* pEntry);

protected:
    event_base*                        m_pBase;
    bool                               m_bHasBaseOwnerShip;
    std::map<uint32_t, struct timeval> m_mapCommonTimeVal;
    std::map<CEventEntry*, bool>       m_mapEntry;
    std::set<CEventEntry*>             m_setWaitEntry;
    std::mutex                         m_mutex;
    struct event*                      m_DefaultEvent = nullptr;
};
#endif // EventManager_h__
