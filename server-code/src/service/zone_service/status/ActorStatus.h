#ifndef ACTORSTATUS_H
#define ACTORSTATUS_H

#include "BaseCode.h"
#include "IStatus.h"
#include "msg/zone_service.pb.h"

class CActor;
export_lua class CActorStatus
{
	CActorStatus();

public:
	~CActorStatus();
	CREATE_NEW_IMPL(CActorStatus);
	bool Init(CActor* pActor);
	void ProcessAttrib(CActorAttrib& attrib);

	export_lua CStatus* QueryStatus(uint16_t idStatusType) const;
	export_lua bool		AttachStatus(uint16_t idStatusType, UCHAR ucLev, OBJID idCaster, uint32_t nPower, uint32_t nSecs, uint32_t nTimes);
	export_lua bool		DetachStatus(uint16_t idStatusType);
	export_lua bool		DetachStatusByType(uint32_t nStatusType);
	export_lua bool		DetachStatusByFlag(uint32_t nStatusFlag, bool bHave = true);
	export_lua bool		TestStatusByType(uint32_t nStatusType) const;
	export_lua bool		TestStatusByFlag(uint32_t nFlag) const;
	template<typename Func>
	void for_each(Func func) const
	{
		for(const auto& [k, v]: m_setStatus)
		{
			func(v);
		}
	}

	export_lua void	  SyncTo(CActor* pActor);
	export_lua void	  FillStatusMsg(SC_STATUS_LIST& status_msg);
	void			  SaveInfo();
	void			  Stop();
	export_lua size_t size() const { return m_setStatus.size(); }

public:
	export_lua void OnMove();
	export_lua void OnSkill(uint32_t idSkill);
	export_lua void OnAttack(CActor* pTarget, uint32_t idSkill, int32_t nDamage);
	export_lua void OnDead(CActor* pKiller);
	export_lua void OnBeAttack(CActor* pAttacker, int32_t nDamage);
	export_lua void OnLeaveMap();
	export_lua void OnLogin();
	export_lua void OnLogout();

private:
	void _AddStatus(CStatus* pStatus);

	void _RemoveStatus(CStatus* pStatus);

private:
	CActor*								   m_pOwner;
	std::unordered_map<uint16_t, CStatus*> m_setStatus;
};
#endif /* ACTORSTATUS_H */
