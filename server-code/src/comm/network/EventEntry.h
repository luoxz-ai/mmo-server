#ifndef EventEntry_h__
#define EventEntry_h__

#include <functional>
#include <map>
#include <set>

#include "BaseCode.h"
#include "MemoryHeap.h"

struct event;
struct event_base;
typedef std::function<void()> EventCallBackFunc;

enum EVENT_MANAGER_TYPE
{
    EMT_EVMANAGER,
    EMT_ENTRY_PTR,
    EMT_ENTRY_QUEUE,
    EMT_ENTRY_MAP,
};
class CEventManager;
class CEventEntry
{
public:
    CEventEntry(CEventManager* pManager, uint32_t evType, EventCallBackFunc&& cb, time_t tWaitTime, bool bPersisit);
    ~CEventEntry();

public:
    void Destory();
    void Cancel();
    void Clear();
    void Set(uint32_t evType, EventCallBackFunc cb, time_t tWaitTime, bool bPersisit);
    void ReleaseFromManager();
    bool CreateEvTimer(event_base* base);
    void Trigger();

    uint32_t GetManagerType() const;
    void     SetManagerType(uint32_t val);
    uint32_t GetEventType() const;
    void     SetEventType(uint32_t val);
    bool     IsCanceled() const;
    bool     IsRunning() const;
    bool     IsVaild() const;

public:
    MEMORYHEAP_DECLARATION(s_heap);

private:
    CEventManager*    m_pManager;
    EventCallBackFunc m_pCallBack;
    time_t            m_tWaitTime;
    bool              m_bPersist;
    event*            m_pevTimer = nullptr;
    std::atomic<bool> m_bRunning = true;
    uint32_t          m_evType;
    uint32_t          m_evManagerType;
    friend class CEventManager;
};

class CEventEntryMap
{
public:
    CEventEntryMap();
    ~CEventEntryMap();

public:
    void               Clear();
    bool               Cancel(uint32_t evType);
    void               ClearByType(uint32_t evType);
    const CEventEntry* Query(uint32_t evType) const;

protected:
    CEventEntry*& GetRef(uint32_t evType);
    CEventEntry*& operator[](uint32_t evType);
    bool          Set(CEventEntry* pEntry);

protected:
    std::map<uint32_t, CEventEntry*> m_setEntry;
    friend class CEventManager;
};

class CEventEntryQueue
{
public:
    CEventEntryQueue();
    ~CEventEntryQueue();

public:
    void Clear();
    void ClearByType(uint32_t evType);
    void Clear_IF(std::function<bool(CEventEntry*)> func);

protected:
    bool Add(CEventEntry* pEntry);

protected:
    std::set<CEventEntry*> m_setEntry;
    friend class CEventManager;
};

class CEventEntryPtr
{
public:
    CEventEntryPtr();
    ~CEventEntryPtr();

public:
    bool Cancel();
    void Clear();
    bool IsRunning();

protected:
    CEventEntry*  Query() const;
    CEventEntry*& GetRef();
    bool          Set(CEventEntry* pEntry);

protected:
    CEventEntry* m_pEntry;
    friend class CEventManager;
};
#endif // EventEntry_h__
