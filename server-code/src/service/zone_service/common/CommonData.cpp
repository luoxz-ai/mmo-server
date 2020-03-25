#include "CommonData.h"

#include "Player.h"
#include "ZoneService.h"

MEMORYHEAP_IMPLEMENTATION(CCommonData, s_heap);

CCommonData::CCommonData(CPlayer* pPlayer, CDBRecordPtr&& pRecord)
    : m_pOwner(pPlayer)
    , m_pRecord(pRecord.release())
{
}

CCommonData::CCommonData(CPlayer* pPlayer)
    : m_pOwner(pPlayer)
{
}

CCommonData::~CCommonData() {}

uint32_t CCommonData::GetIdx() const
{
    return m_pRecord->Field(TBLD_COMMONDATA::KEYIDX);
}

uint64_t CCommonData::GetData(uint32_t nIdx) const
{
    __ENTER_FUNCTION
    CHECKF(0 > nIdx && nIdx < 4);
    return m_pRecord->Field(TBLD_COMMONDATA::DATA0 + nIdx);
    __LEAVE_FUNCTION
    return 0;
}

uint64_t CCommonData::AddData(uint32_t nIdx, uint64_t nVal, bool bUpdate /*= false*/, bool bSync /*= false*/)
{
    __ENTER_FUNCTION
    CHECKF(0 > nIdx && nIdx < 4);
    uint64_t val = GetData(nIdx);
    val += nVal;
    m_pRecord->Field(TBLD_COMMONDATA::DATA0 + nIdx) = val;
    if(bUpdate)
        Save();
    if(bSync || nIdx < COMMON_DATA_AUTO_SYNC)
        Sync();
    return val;
    __LEAVE_FUNCTION
    return 0;
}

void CCommonData::SetData(uint32_t nIdx, uint64_t nVal, bool bUpdate /*= false*/, bool bSync /*= false*/)
{
    __ENTER_FUNCTION
    CHECK(0 > nIdx && nIdx < 4);
    m_pRecord->Field(TBLD_COMMONDATA::DATA0 + nIdx) = nVal;
    if(bUpdate)
        Save();
    if(bSync || nIdx < COMMON_DATA_AUTO_SYNC)
        Sync();
    __LEAVE_FUNCTION
}

void CCommonData::Sync()
{
    __ENTER_FUNCTION
    //发送给客户端
    SC_COMMONDATA msg;
    auto          pData = msg.add_datalist();
    pData->set_keyidx(GetIdx());
    pData->set_data0(GetData(0));
    pData->set_data1(GetData(1));
    pData->set_data2(GetData(2));
    pData->set_data3(GetData(3));

    m_pOwner->SendMsg(CMD_SC_COMMONDATA, msg);
    __LEAVE_FUNCTION
}

void CCommonData::Save()
{
    __ENTER_FUNCTION
    m_pRecord->Update(true);
    __LEAVE_FUNCTION
}

void CCommonData::DeleteRecord()
{
    __ENTER_FUNCTION
    m_pRecord->DeleteRecord(true);
    __LEAVE_FUNCTION
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

CCommonDataSet::CCommonDataSet() {}

CCommonDataSet::~CCommonDataSet()
{
    __ENTER_FUNCTION
    m_setData.clear();
    __LEAVE_FUNCTION
}

bool CCommonDataSet::Init(CPlayer* pPlayer)
{
    __ENTER_FUNCTION
    m_pOwner     = pPlayer;
    auto* pDB    = ZoneService()->GetGameDB(pPlayer->GetWorldID());
    auto  result = pDB->Query(TBLD_COMMONDATA::table_name,
                             fmt::format(FMT_STRING("SELECT * FROM {} WHERE {}={}"),
                                         TBLD_COMMONDATA::table_name,
                                         TBLD_COMMONDATA::field_name[TBLD_COMMONDATA::PLAYERID],
                                         pPlayer->GetID()));
    if(result)
    {
        for(size_t i = 0; i < result->get_num_row(); i++)
        {
            auto row   = result->fetch_row(true);
            auto pData = std::make_unique<CCommonData>(pPlayer, std::move(row));

            m_setData[pData->GetIdx()] = std::move(pData);
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CCommonDataSet::SyncAll()
{
    __ENTER_FUNCTION
    if(m_setData.empty())
        return;

    SC_COMMONDATA msg;
    for(auto& [k, pData]: m_setData)
    {
        if(k >= COMMON_DATA_NOT_SYNC_CLIENT)
            break;

        auto data_ptr = msg.add_datalist();
        data_ptr->set_keyidx(pData->GetIdx());
        data_ptr->set_data0(pData->GetData(0));
        data_ptr->set_data1(pData->GetData(1));
        data_ptr->set_data2(pData->GetData(2));
        data_ptr->set_data3(pData->GetData(3));

        constexpr int32_t MAX_DATA_PER_MSG = 50;
        if(msg.datalist_size() > MAX_DATA_PER_MSG)
        {
            m_pOwner->SendMsg(CMD_SC_COMMONDATA, msg);
            msg.clear_datalist();
        }
    }

    if(msg.datalist_size() > 0)
    {
        m_pOwner->SendMsg(CMD_SC_COMMONDATA, msg);
    }
    __LEAVE_FUNCTION
}

void CCommonDataSet::Save()
{
    __ENTER_FUNCTION
    for(auto& [k, pData]: m_setData)
    {
        pData->Save();
    }
    __LEAVE_FUNCTION
}

CCommonData* CCommonDataSet::QueryData(uint32_t nIdx, bool bCreateNew /*= false*/)
{
    __ENTER_FUNCTION
    auto it = m_setData.find(nIdx);
    if(it != m_setData.end())
    {
        return it->second.get();
    }

    if(bCreateNew)
        return CreateData(nIdx);
    else
        return nullptr;
    __LEAVE_FUNCTION
    return nullptr;
}

CCommonData* CCommonDataSet::CreateData(uint32_t nIdx)
{
    __ENTER_FUNCTION
    auto* pDB = ZoneService()->GetGameDB(m_pOwner->GetWorldID());

    auto pDBRecord = pDB->MakeRecord(TBLD_COMMONDATA::table_name);

    pDBRecord->Field(TBLD_COMMONDATA::ID)       = ZoneService()->CreateUID();
    pDBRecord->Field(TBLD_COMMONDATA::PLAYERID) = m_pOwner->GetID();
    pDBRecord->Field(TBLD_COMMONDATA::KEYIDX)   = nIdx;
    CHECKF(pDBRecord->Update(true));

    m_setData[nIdx] = std::make_unique<CCommonData>(m_pOwner, std::move(pDBRecord));
    return m_setData[nIdx].get();
    __LEAVE_FUNCTION
    return nullptr;
}

void CCommonDataSet::DeleteData(uint32_t nIdx)
{
    __ENTER_FUNCTION
    auto it = m_setData.find(nIdx);
    if(it != m_setData.end())
    {
        it->second->DeleteRecord();
        m_setData.erase(it);
    }
    __LEAVE_FUNCTION
}
