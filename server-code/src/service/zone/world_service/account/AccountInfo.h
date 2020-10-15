#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H

#include "DBRecord.h"
#include "gamedb.h"

struct ST_ROLE_INFO
{
    OBJID              GetID() const { return m_pRecord->Field(TBLD_PLAYER::ID); }
    uint16_t           GetWorldID() const { return m_pRecord->Field(TBLD_PLAYER::WORLDID); }
    uint16_t           GetOriWorldID() const { return m_pRecord->Field(TBLD_PLAYER::WORLDID); }
    const std::string& GetOpenID() const { return m_pRecord->Field(TBLD_PLAYER::OPENID); }
    const std::string& GetName() const { return m_pRecord->Field(TBLD_PLAYER::NAME); }
    uint32_t           GetProf() const { return m_pRecord->Field(TBLD_PLAYER::PROF); }
    uint32_t           GetBaseLook() const { return m_pRecord->Field(TBLD_PLAYER::BASELOOK); }
    uint32_t           GetLev() const { return m_pRecord->Field(TBLD_PLAYER::LEV); }
    uint32_t           GetVipLev() const { return m_pRecord->Field(TBLD_PLAYER::VIPLEV); }
    uint64_t           GetRecordSceneIdx() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_SCENEID); }
    uint64_t           GetHomeSceneIdx() const { return m_pRecord->Field(TBLD_PLAYER::HOME_SCENEID); }
    float              GetRecordPosX() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_X); }
    float              GetRecordPosY() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_Y); }
    float              GetRecordFace() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_FACE); }
    float              GetHomePosX() const { return m_pRecord->Field(TBLD_PLAYER::HOME_X); }
    float              GetHomePosY() const { return m_pRecord->Field(TBLD_PLAYER::HOME_Y); }
    float              GetHomeFace() const { return m_pRecord->Field(TBLD_PLAYER::HOME_FACE); }
    uint64_t           GetTeamID() const { return m_pRecord->Field(TBLD_PLAYER::TEAMID); }
    uint64_t           GetGuildID() const { return m_pRecord->Field(TBLD_PLAYER::GUILDID); }
    uint64_t           GetMateID() const { return m_pRecord->Field(TBLD_PLAYER::MATE_ID); }

    void SetDelTime(uint32_t v)
    {
        __ENTER_FUNCTION
        m_pRecord->Field(TBLD_PLAYER::DEL_TIME) = v;
        m_pRecord->Update();
        __LEAVE_FUNCTION
    }
    void ClearMate()
    {
        __ENTER_FUNCTION
        m_pRecord->Field(TBLD_PLAYER::MATE_ID)   = 0;
        m_pRecord->Field(TBLD_PLAYER::MATE_NAME) = "";
        m_pRecord->Update();
        __LEAVE_FUNCTION
    }

    ST_ROLE_INFO(CDBRecordPtr&& pRecord)
        : m_pRecord(std::move(pRecord))
    {
    }

    CDBRecordPtr m_pRecord;
};
using ST_ROLE_INFO_PTR = std::unique_ptr<ST_ROLE_INFO>;

#endif /* ACCOUNTINFO_H */
