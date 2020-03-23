#include "EventEntry.h"

#include "EventManager.h"
#include "event2/event.h"

MEMORYHEAP_IMPLEMENTATION(CEventEntry, s_heap);

CEventEntry::CEventEntry(CEventManager*      pManager,
                         uint32_t            evType,
                         EventCallBackFunc&& cb,
                         time_t              tWaitTime,
                         bool                bPersisit)
    : m_pManager(pManager)
    , m_bRunning(true)
    , m_tWaitTime(tWaitTime)
    , m_evType(evType)
    , m_evManagerType(EMT_EVMANAGER)
    , m_pevTimer(nullptr)
    , m_pCallBack(cb)
    , m_bPersist(bPersisit)
{
}

CEventEntry::~CEventEntry()
{
    Destory();
}

void CEventEntry::Destory()
{
    __ENTER_FUNCTION
    if(m_pevTimer)
        event_free(m_pevTimer);
    __LEAVE_FUNCTION
}

uint32_t CEventEntry::GetManagerType() const
{
    return m_evManagerType;
}

void CEventEntry::SetManagerType(uint32_t val)
{
    m_evManagerType = val;
}

uint32_t CEventEntry::GetEventType() const
{
    return m_evType;
}

void CEventEntry::SetEventType(uint32_t val)
{
    m_evType = val;
}

void CEventEntry::Cancel()
{
    __ENTER_FUNCTION
    if(m_bRunning == false)
        return;
    if(m_pCallBack)
    {
        m_pCallBack = nullptr;
        if(m_pevTimer)
            event_del(m_pevTimer);
        m_bRunning = false;
    }
    __LEAVE_FUNCTION
}

bool CEventEntry::IsCanceled() const
{
    return m_bRunning == false;
}

bool CEventEntry::IsRunning() const
{
    return m_bRunning;
}

bool CEventEntry::IsVaild() const
{
    return m_bRunning == true && m_pCallBack != nullptr;
}

void CEventEntry::Clear()
{
    __ENTER_FUNCTION
    m_pCallBack = nullptr;
    if(m_pevTimer && m_bRunning)
        event_del(m_pevTimer);
    m_bRunning = false;
    __LEAVE_FUNCTION
}

void CEventEntry::Set(uint32_t evType, EventCallBackFunc cb, time_t tWaitTime, bool bPersist)
{
    m_evType    = evType;
    m_pCallBack = cb;
    m_tWaitTime = tWaitTime;
    m_bPersist  = bPersist;
    m_bRunning  = true;
}

void CEventEntry::ReleaseFromManager()
{
    __ENTER_FUNCTION
    if(m_pManager)
        m_pManager->RemoveWait(this);
    delete this;
    __LEAVE_FUNCTION
}

bool CEventEntry::CreateEvTimer(event_base* base)
{
    __ENTER_FUNCTION
    if(m_pevTimer == nullptr)
    {
        m_pevTimer = event_new(
            base,
            -1,
            (m_bPersist) ? EV_PERSIST : 0,
            [](int32_t, short, void* ctx) {
                CEventEntry* pEntry = (CEventEntry*)ctx;
                if(pEntry == nullptr || pEntry->m_pCallBack == nullptr)
                    return;
                int32_t nEventType   = pEntry->GetEventType();
                int32_t nManagerType = pEntry->GetManagerType();
                pEntry->Trigger();
                if(pEntry->m_bPersist == false && nManagerType == EMT_EVMANAGER)
                    pEntry->ReleaseFromManager();
            },
            this);
    }

    return m_pevTimer != nullptr;
    __LEAVE_FUNCTION
    return false;
}

void CEventEntry::Trigger()
{
    __ENTER_FUNCTION
    if(m_pCallBack)
    {
        m_pCallBack();
    }
    if(m_bPersist == false)
        m_bRunning = false;
    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////////
CEventEntryMap::CEventEntryMap() {}

CEventEntryMap::~CEventEntryMap()
{
    Clear();
}

void CEventEntryMap::Clear()
{
    __ENTER_FUNCTION
    for(auto it = m_setEntry.begin(); it != m_setEntry.end();)
    {
        CEventEntry* pEntry = it->second;
        if(pEntry)
        {
            pEntry->ReleaseFromManager();
        }
        it = m_setEntry.erase(it);
    }
    m_setEntry.clear();
    __LEAVE_FUNCTION
}

bool CEventEntryMap::Cancel(uint32_t evType)
{
    __ENTER_FUNCTION
    auto it = m_setEntry.find(evType);
    if(it != m_setEntry.end())
    {
        CEventEntry* pEnery = it->second;
        pEnery->Cancel();
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}

void CEventEntryMap::ClearByType(uint32_t evType)
{
    __ENTER_FUNCTION
    auto it = m_setEntry.find(evType);
    if(it != m_setEntry.end())
    {
        CEventEntry* pEnery = it->second;
        pEnery->ReleaseFromManager();
        m_setEntry.erase(it);
    }
    __LEAVE_FUNCTION
}

const CEventEntry* CEventEntryMap::Query(uint32_t evType) const
{
    __ENTER_FUNCTION
    auto it = m_setEntry.find(evType);
    if(it != m_setEntry.end())
    {
        return it->second;
    }
    __LEAVE_FUNCTION
    return nullptr;
}

CEventEntry*& CEventEntryMap::GetRef(uint32_t evType)
{
    return m_setEntry[evType];
}

CEventEntry*& CEventEntryMap::operator[](uint32_t evType)
{
    return m_setEntry[evType];
}

bool CEventEntryMap::Set(CEventEntry* pEntry)
{
    __ENTER_FUNCTION
    m_setEntry[pEntry->GetEventType()] = pEntry;
    return true;
    __LEAVE_FUNCTION
    return false;
}

//////////////////////////////////////////////////////////////////////////
CEventEntryQueue::CEventEntryQueue() {}

CEventEntryQueue::~CEventEntryQueue()
{
    Clear();
}

void CEventEntryQueue::Clear()
{
    __ENTER_FUNCTION
    for(auto it = m_setEntry.begin(); it != m_setEntry.end();)
    {
        CEventEntry* pEntry = *it;
        if(pEntry)
        {
            pEntry->ReleaseFromManager();
        }
        it = m_setEntry.erase(it);
    }
    m_setEntry.clear();
    __LEAVE_FUNCTION
}

void CEventEntryQueue::ClearByType(uint32_t evType)
{
    __ENTER_FUNCTION
    for(auto it = m_setEntry.begin(); it != m_setEntry.end(); it++)
    {
        CEventEntry* pEntry = *it;
        if(pEntry && pEntry->GetEventType() == evType)
        {
            pEntry->ReleaseFromManager();
            m_setEntry.erase(it);
            return;
        }
    }
    __LEAVE_FUNCTION
}

void CEventEntryQueue::Clear_IF(std::function<bool(CEventEntry*)> func)
{
    __ENTER_FUNCTION
    for(auto it = m_setEntry.begin(); it != m_setEntry.end(); it++)
    {
        CEventEntry* pEntry = *it;
        if(pEntry && func(pEntry) == true)
        {
            pEntry->ReleaseFromManager();
            m_setEntry.erase(it);
            return;
        }
    }
    __LEAVE_FUNCTION
}

bool CEventEntryQueue::Add(CEventEntry* pEntry)
{
    __ENTER_FUNCTION
    if(pEntry)
    {
        m_setEntry.insert(pEntry);
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}

//////////////////////////////////////////////////////////////////////////
CEventEntryPtr::CEventEntryPtr()
    : m_pEntry(nullptr)
{
}

CEventEntryPtr::~CEventEntryPtr()
{
    Clear();
}

bool CEventEntryPtr::Cancel()
{
    __ENTER_FUNCTION
    if(m_pEntry)
    {
        m_pEntry->Cancel();
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}

void CEventEntryPtr::Clear()
{
    __ENTER_FUNCTION
    if(m_pEntry)
    {
        m_pEntry->ReleaseFromManager();
        m_pEntry = nullptr;
    }
    __LEAVE_FUNCTION
}

bool CEventEntryPtr::IsRunning()
{
    __ENTER_FUNCTION
    if(m_pEntry)
    {
        return m_pEntry->IsRunning();
    }
    __LEAVE_FUNCTION
    return false;
}

CEventEntry* CEventEntryPtr::Query() const
{
    return m_pEntry;
}

CEventEntry*& CEventEntryPtr::GetRef()
{
    return m_pEntry;
}

bool CEventEntryPtr::Set(CEventEntry* pEntry)
{
    m_pEntry = pEntry;
    return m_pEntry != nullptr;
}
