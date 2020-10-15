#include "CoolDown.h"

#include "DBRecord.h"
#include "MysqlConnection.h"
#include "Player.h"
#include "SceneService.h"
#include "gamedb.h"
#include "msg/zone_service.pb.h"
OBJECTHEAP_IMPLEMENTATION(ICoolDown, s_heap);
OBJECTHEAP_IMPLEMENTATION(CCoolDownSet, s_heap);

CPlayerCoolDown::CPlayerCoolDown(CPlayer* pPlayer, CDBRecordPtr&& pRecord)
    : m_pOwner(pPlayer)
    , m_pRecord(pRecord.release())
{
}

CPlayerCoolDown::~CPlayerCoolDown() {}

uint32_t CPlayerCoolDown::GetType() const
{
    __ENTER_FUNCTION
    CHECKF(m_pRecord);
    return m_pRecord->Field(TBLD_COOLDOWN::KEYTYPE);
    __LEAVE_FUNCTION
    return 0;
}

uint32_t CPlayerCoolDown::GetIdx() const
{
    __ENTER_FUNCTION
    CHECKF(m_pRecord);
    return m_pRecord->Field(TBLD_COOLDOWN::KEYIDX);
    __LEAVE_FUNCTION
    return 0;
}

uint64_t CPlayerCoolDown::GetExpireTime() const
{
    __ENTER_FUNCTION
    CHECKF(m_pRecord);
    return m_pRecord->Field(TBLD_COOLDOWN::EXPIRE_TIME);
    __LEAVE_FUNCTION
    return 0;
}

bool CPlayerCoolDown::StartCoolDown(uint32_t nMSec, bool bUpdate, bool bSync)
{
    __ENTER_FUNCTION
    CHECKF(m_pRecord);
    m_pRecord->Field(TBLD_COOLDOWN::EXPIRE_TIME) = uint32_t(TimeGetMillisecond() + nMSec);
    if(bUpdate)
        m_pRecord->Update(true);
    if(bSync)
        Sync();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerCoolDown::ClearCoolDown(bool bUpdate, bool bSync)
{
    __ENTER_FUNCTION
    CHECKF(m_pRecord);
    m_pRecord->Field(TBLD_COOLDOWN::EXPIRE_TIME) = 0;
    if(bUpdate)
        m_pRecord->Update(true);
    if(bSync)
        Sync();
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPlayerCoolDown::Sync()
{
    __ENTER_FUNCTION
    //发送给客户端
    SC_COOLDOWN msg;
    auto        pData = msg.add_datalist();

    pData->set_type(GetType());
    pData->set_idx(GetIdx());
    pData->set_expiretime(GetExpireTime());

    m_pOwner->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CPlayerCoolDown::Save()
{
    __ENTER_FUNCTION
    CHECK(m_pRecord);
    m_pRecord->Update(true);
    __LEAVE_FUNCTION
}

void CPlayerCoolDown::DeleteRecord()
{
    __ENTER_FUNCTION
    CHECK(m_pRecord);
    m_pRecord->DeleteRecord(true);
    __LEAVE_FUNCTION
}

CCoolDownSet::CCoolDownSet() {}

CCoolDownSet::~CCoolDownSet()
{
    __ENTER_FUNCTION
    m_setDataMap.clear();
    __LEAVE_FUNCTION
}

bool CCoolDownSet::Init()
{
    return true;
}

bool CCoolDownSet::IsCoolDown(uint32_t nType, uint32_t nIdx)
{
    __ENTER_FUNCTION
    uint64_t key = MAKE64(nType, nIdx);
    auto     it  = m_setDataMap.find(key);
    if(it == m_setDataMap.end())
        return false;

    return it->second->GetExpireTime() < uint32_t(TimeGetSecond());
    __LEAVE_FUNCTION
    return false;
}

bool CCoolDownSet::IsCoolDown(uint32_t nType, uint32_t nIdx, uint32_t now)
{
    __ENTER_FUNCTION
    uint64_t key = MAKE64(nType, nIdx);
    auto     it  = m_setDataMap.find(key);
    if(it == m_setDataMap.end())
        return false;

    return it->second->GetExpireTime() < now;
    __LEAVE_FUNCTION
    return false;
}

bool CCoolDownSet::StartCoolDown(uint32_t nType, uint32_t nIdx, uint32_t nMSec, bool bUpdate /*= false*/, bool bSync /* = true*/)
{
    __ENTER_FUNCTION
    uint64_t key = MAKE64(nType, nIdx);
    auto     it  = m_setDataMap.find(key);
    if(it == m_setDataMap.end())
    {
        ICoolDown* pCD = CreateData(nType, nIdx, nMSec);
        return pCD != nullptr;
    }

    return it->second->StartCoolDown(nMSec, bUpdate, bSync);
    __LEAVE_FUNCTION
    return false;
}

bool CCoolDownSet::ClearCoolDown(uint32_t nType, uint32_t nIdx, bool bUpdate /*= false*/, bool bSync /* = true*/)
{
    __ENTER_FUNCTION
    uint64_t key = MAKE64(nType, nIdx);
    auto     it  = m_setDataMap.find(key);
    if(it == m_setDataMap.end())
    {
        return true;
    }
    return it->second->ClearCoolDown(bUpdate, bSync);
    __LEAVE_FUNCTION
    return false;
}

ICoolDown* CCoolDownSet::CreateData(uint32_t nType, uint32_t nIdx, uint32_t nMSec)
{
    __ENTER_FUNCTION
    uint64_t key   = MAKE64(nType, nIdx);
    auto     pData = std::make_unique<CCoolDown>();
    pData->StartCoolDown(nMSec);
    m_setDataMap[key] = std::move(pData);
    return m_setDataMap[key].get();
    __LEAVE_FUNCTION
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

CPlayerCoolDownSet::CPlayerCoolDownSet() {}

CPlayerCoolDownSet::~CPlayerCoolDownSet() {}

bool CPlayerCoolDownSet::Init(CPlayer* pPlayer)
{
    __ENTER_FUNCTION
    CHECKF(pPlayer);
    m_pOwner     = pPlayer;
    auto* pDB    = SceneService()->GetGameDB(pPlayer->GetWorldID());
    auto  result = pDB->QueryKey<TBLD_COOLDOWN, TBLD_COOLDOWN::PLAYERID>(pPlayer->GetID());
    if(result)
    {
        for(size_t i = 0; i < result->get_num_row(); i++)
        {
            auto     row   = result->fetch_row(true);
            auto     pData = std::make_unique<CPlayerCoolDown>(pPlayer, std::move(row));
            uint64_t key   = MAKE64(pData->GetType(), pData->GetIdx());
            m_setDataMap.emplace(key, std::move(pData));
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPlayerCoolDownSet::SyncAll()
{
    __ENTER_FUNCTION
    //只需要同步时间戳超过大于now的给前端就好了
    SC_COOLDOWN msg;
    uint64_t    now = TimeGetMillisecond();
    for(auto& [k, v]: m_setDataMap)
    {
        CPlayerCoolDown* pCD = static_cast<CPlayerCoolDown*>(v.get());
        if(pCD && pCD->GetExpireTime() > now)
        {
            auto pData = msg.add_datalist();
            pData->set_type(pCD->GetType());
            pData->set_idx(pCD->GetIdx());
            pData->set_expiretime(pCD->GetExpireTime());
        }
    }
    m_pOwner->SendMsg(msg);

    __LEAVE_FUNCTION
}

void CPlayerCoolDownSet::Save()
{
    __ENTER_FUNCTION
    for(auto& [k, v]: m_setDataMap)
    {
        CPlayerCoolDown* pDataAcc = static_cast<CPlayerCoolDown*>(v.get());
        pDataAcc->Save();
    }
    __LEAVE_FUNCTION
}

ICoolDown* CPlayerCoolDownSet::CreateData(uint32_t nType, uint32_t nIdx, uint32_t nMSec)
{
    __ENTER_FUNCTION
    auto* pDB = SceneService()->GetGameDB(m_pOwner->GetWorldID());

    auto pDBRecord = pDB->MakeRecord(TBLD_COOLDOWN::table_name());

    pDBRecord->Field(TBLD_COOLDOWN::PLAYERID)    = m_pOwner->GetID();
    pDBRecord->Field(TBLD_COOLDOWN::KEYTYPE)     = nType;
    pDBRecord->Field(TBLD_COOLDOWN::KEYIDX)      = nIdx;
    pDBRecord->Field(TBLD_COOLDOWN::EXPIRE_TIME) = uint64_t(TimeGetMillisecond() + nMSec);

    CHECKF(pDBRecord->Update(true));
    uint64_t key = MAKE64(nType, nIdx);

    auto pData = std::make_unique<CPlayerCoolDown>(m_pOwner, std::move(pDBRecord));
    m_setDataMap.emplace(key, std::move(pData));
    return m_setDataMap[key].get();
    __LEAVE_FUNCTION
    return nullptr;
}
