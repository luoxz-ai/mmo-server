#ifndef TEAM_H
#define TEAM_H

#include "BaseCode.h"
#include "game_common_def.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

class CUser;
class CTeam: public NoncopyableT<CTeam>
{
    CTeam();
    bool Init(uint64_t idTeam, OBJID idLeader);
public:
    CreateNewImpl(CTeam);
public:
    ~CTeam();
    
    
    void SendTeamAction(uint32_t nAction, OBJID idOperator, OBJID idMember);
    void SendTeamMemberInfo(const SC_TEAMMEMBER_INFO::MemberInfo& info);
    void SendAllTeamMemberInfo(CUser* pUser);

public:
    void SetLeader(OBJID idOperator, OBJID idLeader);
    void AddMember(OBJID idMember);
    void _AddMember(CUser* pUser);
    void KickMember(OBJID idOperator, OBJID idMember);
    void QuitTeam(OBJID idOperator);
    void InviteMember(OBJID idInviter, OBJID idInvitee);
    void AcceptInvite(OBJID idInviter, OBJID idInvitee, bool bResult);
    void ApplyMember(OBJID idApplicant);
    void AcceptApply(OBJID idApplicant, OBJID idRespondent, bool bResult);

public:
    bool IsLeader(OBJID idActor);
    bool IsMember(OBJID idActor);
    void OnUserOnline(OBJID idActor, bool bOnline);
    void OnUserLevChg(OBJID idActor, uint32_t nLev);
    void OnUserNameChg(OBJID idActor, const std::string& name);
    bool IsFull() const { return m_setMember.size() > _MAX_TEAM_MEMBER; }

private:
    uint64_t                                    m_idTeam;
    uint64_t                                    m_idLeader;
    std::vector<SC_TEAMMEMBER_INFO::MemberInfo> m_setMember;
    std::unordered_set<uint64_t>                m_setInvite;
    std::unordered_set<uint64_t>                m_setApply;

public:
    OBJECTHEAP_DECLARATION(s_heap);
};

#endif /* TEAM_H */
