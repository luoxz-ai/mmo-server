#ifndef EventManager_h__
#define EventManager_h__

#include <map>
#include <set>

#include <time.h>

#include "BaseCode.h"
#include "EventEntry.h"

struct event_base;
struct event;

class CEventManager : public NoncopyableT<CEventManager>
{
    CEventManager();
    bool Init(event_base* base);

public:
    CreateNewImpl(CEventManager);
    ~CEventManager();

    void Destory();

    void   OnTimer();
    size_t GetEventCount();

    bool RemoveWait(CEventEntry* pEntry);

    bool ScheduleEvent(const CEventEntryCreateParam& param);
    bool ScheduleEvent(const CEventEntryCreateParam& param,
                       CEventEntryPtr&     refEntry);
    bool ScheduleEvent(const CEventEntryCreateParam& param,
                       CEventEntryQueue&   refEntryQueue);
    bool ScheduleEvent(const CEventEntryCreateParam& param,
                       CEventEntryMap&     refEntryMap);

protected:
    CEventEntry* _ScheduleEvent(const CEventEntryCreateParam& param,
                               CEventEntry*        pEntry,
                               uint32_t nManagerType);
    CEventEntry* CreateEntry(const CEventEntryCreateParam& param,
                             uint32_t nManagerType = 0);
    void         PushWait(CEventEntry* pEntry);
    void         ScheduleWait();

protected:
    void Delete(CEventEntry* pEntry);

protected:
    event_base*                        m_pBase;
    bool                               m_bOwnBase;
    std::map<uint32_t, struct timeval> m_mapCommonTimeVal;
    std::map<CEventEntry*, bool>       m_mapEntry;
    std::set<CEventEntry*>             m_setWaitEntry;
    std::mutex                         m_mutex;
    struct event*                      m_pScheduleWaitEvent = nullptr;
};
#endif // EventManager_h__
