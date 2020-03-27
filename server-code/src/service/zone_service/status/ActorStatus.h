#ifndef ACTORSTATUS_H
#define ACTORSTATUS_H

#include "BaseCode.h"
#include "IStatus.h"
#include "msg/zone_service.pb.h"

class CActor;
export_lua class CActorStatus : public Noncopyable<CActorStatus>
{
    CActorStatus();
    bool Init(CActor* pActor);
public:
    CreateNewImpl(CActorStatus);
public:
    ~CActorStatus();
    
    

    export_lua CStatus* QueryStatus(uint16_t idStatusType) const;
    export_lua bool     AttachStatus(uint16_t idStatusType,
                                     uint8_t  ucLev,
                                     OBJID    idCaster,
                                     uint32_t nPower,
                                     uint32_t nSecs,
                                     uint32_t nTimes);
    export_lua bool     DetachStatus(uint16_t idStatusType);
    export_lua bool     DetachStatusByType(uint32_t nStatusType);
    export_lua bool     DetachStatusByFlag(uint32_t nStatusFlag, bool bHave = true);
    export_lua bool     TestStatusByType(uint32_t nStatusType) const;
    export_lua bool     TestStatusByFlag(uint32_t nFlag) const;
    template<typename Func>
    void for_each(Func func) const
    {
        for(const auto& [k, v]: m_setStatus)
        {
            func(v);
        }
    }

    export_lua void   SyncTo(CActor* pActor);
    export_lua void   FillStatusMsg(SC_STATUS_LIST& status_msg);
    void              SaveInfo();
    void              Stop();
    export_lua size_t size() const { return m_setStatus.size(); }

public:
    void OnMove();
    void OnSkill(uint32_t idSkill);
    void OnAttack(CActor* pTarget, uint32_t idSkill, int32_t nDamage);
    void OnDead(CActor* pKiller);
    void OnBeAttack(CActor* pAttacker, int32_t nDamage);
    void OnLeaveMap();
    void OnLogin();
    void OnLogout();

    template<class Func, class... Args>
    void OnEventDetach(Func func, int32_t flag, Args&&... args)
    {
        __ENTER_FUNCTION
        for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
        {
            CStatus* pStatus = it->second;
            if(HasFlag(pStatus->GetFlag(), flag) == true || std::invoke(func, it->second, std::forward<Args>(args)...))
            {
                pStatus->OnDeatch();
                SAFE_DELETE(pStatus);
                it = m_setStatus.erase(it);
            }
            else
            {
                it++;
            }
        }
        __LEAVE_FUNCTION
    }

    template<class Func, class... Args>
    void OnEventUndeatch(Func func, int32_t flag, Args&&... args)
    {
        __ENTER_FUNCTION
        for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
        {
            CStatus* pStatus = it->second;
            if(HasFlag(pStatus->GetFlag(), flag) == false || std::invoke(func, it->second, std::forward<Args>(args)...))
            {
                pStatus->OnDeatch();
                SAFE_DELETE(pStatus);
                it = m_setStatus.erase(it);
            }
            else
            {
                it++;
            }
        }
        __LEAVE_FUNCTION
    }

private:
    void _AddStatus(CStatus* pStatus);

    void _RemoveStatus(CStatus* pStatus);

private:
    CActor*                                m_pOwner;
    std::unordered_map<uint16_t, CStatus*> m_setStatus;
};
#endif /* ACTORSTATUS_H */
