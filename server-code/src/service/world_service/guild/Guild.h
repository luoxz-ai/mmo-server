#ifndef GUILD_H
#define GUILD_H


#include "BaseCode.h"
#include "game_common_def.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"
#include "DBRecord.h"
#include "gamedb.h"

class CGuildMemberInfo
{
    CGuildMemberInfo();
    bool Init(CDBRecordPtr&& pDBRecord);
public:
    CreateNewImpl(CGuildMemberInfo);
public:
    ~CGuildMemberInfo();

    GuildMemberInfo to_pb() const;
    void to_pb(GuildMemberInfo& info) const;
    
    uint64_t get_member_id() const { return m_pDBRecord->Field(TBLD_PLAYER_GUILDINFO::ID);}
    uint32_t get_member_rank() const { return m_pDBRecord->Field(TBLD_PLAYER_GUILDINFO::RANK);}
	uint32_t get_member_score() const { return m_pDBRecord->Field(TBLD_PLAYER_GUILDINFO::SCORE);}
    uint32_t get_member_total_score() const { return m_pDBRecord->Field(TBLD_PLAYER_GUILDINFO::TOTAL_SCORE);}
    uint32_t get_member_jointime() const { return m_pDBRecord->Field(TBLD_PLAYER_GUILDINFO::JOIN_TIME);}

    void set_member_id(uint64_t v) {m_pDBRecord->Field(TBLD_PLAYER_GUILDINFO::ID) = v;}
    void set_member_rank(uint32_t v){m_pDBRecord->Field(TBLD_PLAYER_GUILDINFO::RANK) = v;}
    void set_member_score(uint32_t v){m_pDBRecord->Field(TBLD_PLAYER_GUILDINFO::SCORE) = v;}
    void set_member_total_score(uint32_t v){m_pDBRecord->Field(TBLD_PLAYER_GUILDINFO::TOTAL_SCORE) = v;}


    
	uint32_t get_member_lev() const { return m_lev;}
	bool   get_member_online() const { return m_bOnline;}
	const std::string& get_member_name() const { return m_name;}
	uint32_t get_member_last_logout() const { return m_lastlogouttime;}
        
    void set_member_lev(uint32_t v){m_lev = v;}
    void set_member_online(bool v){m_bOnline = v;}
    void set_member_name(const std::string& v){m_name = v;}
    void set_member_last_logout(uint32_t v){m_lastlogouttime = v;}

    void Save()
    {
        m_pDBRecord->Update();
    }
    void ClearDirty()
    {
        m_pDBRecord->ClearDirty();
    }

private:
    CDBRecordPtr m_pDBRecord;
    bool m_bOnline = false;
    uint32_t m_lev = 0;
    std::string m_name;
    uint32_t m_lastlogouttime = 0;
public: 
    OBJECTHEAP_DECLARATION(s_heap);
};


class CUser;
class CGuild : public NoncopyableT<CGuild>
{
    CGuild();
    bool Init(CDBRecordPtr&& pDBRecord, CUser* pLeader);
    bool Init(CDBRecordPtr&& pDBRecord);
public:
    CreateNewImpl(CGuild);

public:
    ~CGuild();

    void SendGuildAction(uint32_t nAction, OBJID idOperator, OBJID idMember) const;
    void SendGuildMemberInfoToAll(CGuildMemberInfo* pMemberInfo) const;
    void SendAllGuildMemberInfoTo(CUser* pUser) const;

public:
    void SetLeader(OBJID idOperator, OBJID idLeader, const std::string& strLeaderName);
    void AddMember(OBJID idMember, uint32_t nRank);
    bool AddMemberOffline(OBJID idMember, uint32_t nRank);
    bool KickMember(OBJID idOperator, OBJID idMember);
    void QuitGuild(OBJID idOperator);
    void InviteMember(OBJID idInviter, OBJID idInvitee);
    void AcceptInvite(OBJID idInviter, OBJID idInvitee, bool bResult);
    void ApplyMember(OBJID idApplicant);
    bool AcceptApply(OBJID idApplicant, OBJID idRespondent, bool bResult);
    bool SetMemberRank(OBJID idOperator, OBJID idMember, uint32_t nRank);
    void SetMemberScore(OBJID idMember, uint32_t nScore);
    void Dismiss(OBJID idOperator);

public:
    bool IsLeader(OBJID idActor)const;
    bool IsMember(OBJID idActor)const;
    bool IsFull() const;
    const CGuildMemberInfo* QueryMember(OBJID idMember)const;
    CGuildMemberInfo* QueryMember(OBJID idMember);



    void OnUserOnline(OBJID idActor, bool bOnline);
    void OnUserLevChg(OBJID idActor, uint32_t nLev);
    void OnUserNameChg(OBJID idActor, const std::string& name);
    
    
    bool CheckMemberRank(uint32_t nRank, uint32_t nAction);
public:

    uint64_t GetGuildID()const {return m_pDBRecord->Field(TBLD_GUILD::ID);}
    const std::string& GetGuildName()const {return m_pDBRecord->Field(TBLD_GUILD::NAME);}
    uint32_t GetGuildLev()const {return m_pDBRecord->Field(TBLD_GUILD::LEV);}
    uint64_t GetLeaderID()const {return m_pDBRecord->Field(TBLD_GUILD::LEADER_ID);}
    const std::string& GetLeaderName()const {return m_pDBRecord->Field(TBLD_GUILD::LEADER_NAME);}
    uint32_t GetGuildCreateTime()const {return m_pDBRecord->Field(TBLD_GUILD::CREATE_TIME);}

    size_t GetRankCount(uint32_t nRank);
    size_t GetRankLimit(uint32_t nRank);
private:
    CGuildMemberInfo* CreateMemberInfo(OBJID idMember, uint32_t nRank);
private:
    std::unordered_map<uint64_t, std::unique_ptr<CGuildMemberInfo> >         m_setMemberInfo;
    
    std::unordered_set<uint64_t>    m_setInvite;
    std::unordered_set<uint64_t>    m_setApply;

    CDBRecordPtr m_pDBRecord;
public: 
    OBJECTHEAP_DECLARATION(s_heap);
};

#endif /* GUILD_H */
