#include "EventManager.h"

#include "event2/event.h"

CEventManager::CEventManager()
    : m_pBase(nullptr)
    , m_bHasBaseOwnerShip(false)
{
}

CEventManager::~CEventManager()
{
    Destory();
}

bool CEventManager::Init(event_base* base)
{
    if(base == nullptr)
    {
        m_pBase             = event_base_new();
        m_bHasBaseOwnerShip = true;
    }
    else
    {
        m_pBase                    = base;
        const uint32_t time_out_ms = 200;
        m_DefaultEvent             = event_new(
            m_pBase,
            -1,
            EV_PERSIST,
            [](int32_t, short int32_t, void* ctx) {
                CEventManager* pThis = (CEventManager*)ctx;
                pThis->ScheduleWait();
            },
            (void*)this);
        struct timeval tv
        {
            time_out_ms / 1000, time_out_ms % 1000
        };
        event_add(m_DefaultEvent, &tv);
        //每200ms自动调用一次ScheduleWait
    }

    timeval set_tv_opt[] = {
        {0, 100 * 1000},   // 100ms
        {0, 200 * 1000},   // 200ms
        {0, 400 * 1000},   // 400ms
        {0, 600 * 1000},   // 600ms
        {0, 320 * 1000},   // 320ms
        {0, 680 * 1000},   // 680ms
        {1, 0},            // 1s
        {2, 0},            // 2s
        {3, 0},            // 3s
        {10, 0},           // 10s
        {30, 0},           // 30s
        {60, 0},           // 60s
        {3 * 60, 0},       // 3m
        {60 * 60, 0},      // 1h
        {24 * 60 * 60, 0}, // 1d
    };
    for(size_t i = 0; i < sizeOfArray(set_tv_opt); i++)
    {
        const timeval* tv_out = event_base_init_common_timeout(m_pBase, &set_tv_opt[i]);
        if(tv_out)
        {
            uint32_t uKey            = set_tv_opt[i].tv_sec * 1000 + set_tv_opt[i].tv_usec / 1000;
            m_mapCommonTimeVal[uKey] = *tv_out;
        }
    }

    return true;
}

void CEventManager::Destory()
{
    if(m_bHasBaseOwnerShip)
    {
        event_base_free(m_pBase);
        m_pBase = nullptr;
    }
    else if(m_DefaultEvent)
    {
        event_del(m_DefaultEvent);
        event_free(m_DefaultEvent);
        m_DefaultEvent = nullptr;
        m_pBase        = nullptr;
    }

    for(auto it = m_mapEntry.begin(); it != m_mapEntry.end(); it++)
    {
        if(it->second == true)
        {
            CEventEntry* pEntry = it->first;
            SAFE_DELETE(pEntry);
        }
    }
    m_mapEntry.clear();
    for(auto it = m_setWaitEntry.begin(); it != m_setWaitEntry.end(); it++)
    {
        CEventEntry* pEntry = *it;
        SAFE_DELETE(pEntry);
    }
    m_setWaitEntry.clear();
}

void CEventManager::OnTimer()
{
    if(m_bHasBaseOwnerShip)
    {
        event_base_loop(m_pBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
        ScheduleWait();
    }
}

void CEventManager::DeleteEntry(CEventEntry*& pEntry)
{
    if(pEntry == nullptr)
        return;

    RemoveWait(pEntry);
    SAFE_DELETE(pEntry);
}

bool CEventManager::ScheduleEvent(uint32_t            evType,
                                  EventCallBackFunc&& cb,
                                  time_t              tWaitTime,
                                  bool                bPersist,
                                  CEventEntryPtr&     refEntry)
{
    if(cb == nullptr)
        return false;
    CEventEntry* pEntry = ScheduleEvent(evType, std::move(cb), tWaitTime, bPersist, refEntry.GetRef());
    if(pEntry)
    {
        pEntry->SetManagerType(EMT_ENTRY_PTR);
        return refEntry.Set(pEntry);
    }
    else
        return false;
}

bool CEventManager::ScheduleEvent(uint32_t            evType,
                                  EventCallBackFunc&& cb,
                                  time_t              tWaitTime,
                                  bool                bPersist,
                                  CEventEntryQueue&   refEntryQueue)
{
    if(cb == nullptr)
        return false;
    CEventEntry* pEntry = ScheduleEvent(evType, std::move(cb), tWaitTime, bPersist, nullptr);
    if(pEntry)
    {
        pEntry->SetManagerType(EMT_ENTRY_QUEUE);
        return refEntryQueue.Add(pEntry);
    }
    else
        return false;
}

bool CEventManager::ScheduleEvent(uint32_t            evType,
                                  EventCallBackFunc&& cb,
                                  time_t              tWaitTime,
                                  bool                bPersist,
                                  CEventEntryMap&     refEntryMap)
{
    if(cb == nullptr)
        return false;
    CEventEntry* pEntry = ScheduleEvent(evType, std::move(cb), tWaitTime, bPersist, refEntryMap.GetRef(evType));
    if(pEntry)
    {
        pEntry->SetManagerType(EMT_ENTRY_MAP);
        return refEntryMap.Set(pEntry);
    }
    else
        return false;
}

bool CEventManager::ScheduleEvent(uint32_t evType, EventCallBackFunc&& cb, time_t tWaitTime, bool bPersist)
{
    if(tWaitTime < 0)
    {
        return false;
    }

    CEventEntry* pEntry = CreateEntry(evType, std::move(cb), tWaitTime, bPersist);
    if(PushWait(pEntry) == nullptr)
    {
        SAFE_DELETE(pEntry);
        return false;
    }
    m_mapEntry[pEntry] = false;
    return true;
}

CEventEntry* CEventManager::ScheduleEvent(uint32_t            evType,
                                          EventCallBackFunc&& cb,
                                          time_t              tWaitTime,
                                          bool                bPersist,
                                          CEventEntry*        pEntry)
{
    if(tWaitTime < 0)
    {
        return nullptr;
    }

    if(pEntry)
    {
        RemoveWait(pEntry);
        pEntry->Clear();
        pEntry->Set(evType, std::move(cb), tWaitTime, bPersist);
    }
    else
    {
        pEntry = CreateEntry(evType, std::move(cb), tWaitTime, bPersist);
    }

    if(PushWait(pEntry) == nullptr)
    {
        SAFE_DELETE(pEntry);
        return nullptr;
    }
    return pEntry;
}

CEventEntry* CEventManager::CreateEntry(uint32_t            evType,
                                        EventCallBackFunc&& cb /*= nullptr*/,
                                        time_t              tWaitTime /*= 0*/,
                                        bool                bPersist)
{
    CEventEntry* pEntry = new CEventEntry(this, evType, std::move(cb), tWaitTime, bPersist);
    if(pEntry)
    {
        if(pEntry->CreateEvTimer(m_pBase))
            return pEntry;

        SAFE_DELETE(pEntry);
    }
    return nullptr;
}

CEventEntry* CEventManager::PushWait(CEventEntry* pEntry)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_setWaitEntry.insert(pEntry);
    return pEntry;
}

void CEventManager::ScheduleWait()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_setWaitEntry.empty() == false)
    {
        CEventEntry* pEntry = *m_setWaitEntry.begin();
        m_setWaitEntry.erase(m_setWaitEntry.begin());
        if(pEntry == nullptr)
            continue;
        ;
        if(pEntry->IsVaild() == false)
        {
            continue;
        }

        if(pEntry->CreateEvTimer(m_pBase))
        {
            int32_t nRet = -1;
            auto    it   = m_mapCommonTimeVal.find((uint32_t)pEntry->m_tWaitTime);
            if(it != m_mapCommonTimeVal.end())
            {
                nRet = evtimer_add(pEntry->m_pevTimer, &it->second);
            }
            else
            {
                struct timeval tv = {(long)pEntry->m_tWaitTime / 1000, (long)pEntry->m_tWaitTime % 1000 * 1000};
                nRet              = evtimer_add(pEntry->m_pevTimer, &tv);
            }

            if(nRet == 0)
            {
                auto it = m_mapEntry.find(pEntry);
                if(it != m_mapEntry.end())
                    it->second = true;
                continue;
            }
        }
        // if evtimer_add fail,delete it
        Delete(pEntry);
    }
}

void CEventManager::Delete(CEventEntry* pEntry)
{
    if(pEntry == nullptr)
        return;

    auto it = m_mapEntry.find(pEntry);
    if(it != m_mapEntry.end())
    {
        m_mapEntry.erase(it);
        SAFE_DELETE(pEntry);
    }
}

size_t CEventManager::GetEventCount()
{
    return m_mapEntry.size();
}

bool CEventManager::RemoveWait(CEventEntry* pEntry)
{
    if(pEntry == nullptr)
        return false;
    std::unique_lock<std::mutex> lock(m_mutex);
    m_setWaitEntry.erase(pEntry);
    return true;
}
