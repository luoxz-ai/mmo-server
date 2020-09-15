#ifndef ZONEGUILD_H
#define ZONEGUILD_H


#include "BaseCode.h"
#include <map>
#include "share-code/proto/common/Common.pb.h"
#include "gamedb.h"
#include "DBRecord.h"

class CZoneGuild : public NoncopyableT<CZoneGuild>
{
    CZoneGuild();
    bool Init(CDBRecordPtr&& pDBRecord);
public:
    CreateNewImpl(CZoneGuild);

public:
    ~CZoneGuild();


public:
    bool IsLeader(OBJID idActor)const;
    bool IsMember(OBJID idActor)const;
    bool IsFull() const;
    

public:
    uint64_t GetGuildID()const {return m_pDBRecord->Field(TBLD_GUILD::ID);}
    const std::string& GetGuildName()const {return m_pDBRecord->Field(TBLD_GUILD::NAME);}
    uint32_t GetGuildLev()const {return m_pDBRecord->Field(TBLD_GUILD::LEV);}
    uint64_t GetLeaderID()const {return m_pDBRecord->Field(TBLD_GUILD::LEADER_ID);}
    const std::string& GetLeaderName()const {return m_pDBRecord->Field(TBLD_GUILD::LEADER_NAME);}
    uint32_t GetGuildCreateTime()const {return m_pDBRecord->Field(TBLD_GUILD::CREATE_TIME);}


private:
    std::unordered_set<uint64_t>         m_setMember;



    CDBRecordPtr m_pDBRecord;
public: 
    OBJECTHEAP_DECLARATION(s_heap);
};


#endif /* ZONEGUILD_H */
