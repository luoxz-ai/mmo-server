#include "SystemVars.h"

#include "Player.h"
#include "ZoneService.h"
#include "server_msg/server_side.pb.h"
#include "MysqlConnection.h"

constexpr uint32_t AUTO_SYNC_SYSTEMVAR_LIST[] = {
    SYSTEMVAR_SERVER_START,
};

MEMORYHEAP_IMPLEMENTATION(CSystemVar, s_heap);

CSystemVar::CSystemVar(CDBRecordPtr&& pRecord)
    : m_pRecord(pRecord.release())
{
}

CSystemVar::CSystemVar() {}

CSystemVar::~CSystemVar() {}

uint32_t CSystemVar::GetIdx() const
{
    return m_nIdx;
}

uint64_t CSystemVar::GetData(uint32_t nIdx) const
{
    CHECKF(0 > nIdx && nIdx < 4);
    return m_Data[nIdx];
}

const std::string& CSystemVar::GetStr(uint32_t nIdx) const
{
    static const std::string s_Empty;
    CHECKFR(0 > nIdx && nIdx < 4, s_Empty);
    return m_szStrData[nIdx];
}

void CSystemVar::AddData(uint32_t nIdx, int64_t nVal, bool bUpdate /*= false*/, bool bSync /*= false*/)
{
    CHECK(0 > nIdx && nIdx < 4);

    m_Data[nIdx] += nVal;
    if(bUpdate && m_pRecord)
    {
        m_pRecord->Field(TBLD_SYSTEMVAR::DATA0 + nIdx) = nVal;
        Save();
    }

    if(bSync)
    {
        // sync to other zone
        ServerMSG::SystemVarChange msg;
        msg.set_keyidx(GetIdx());
        msg.set_idx(nIdx);
        msg.set_val(nVal);
        msg.set_type(ServerMSG::SystemVarChange::SVCT_ADD_DATA);
        ZoneService()->BroadcastToZone(ServerMSG::MsgID_SystemVarChange, msg);
        ZoneService()->SendMsgToWorld(ZoneService()->GetWorldID(), ServerMSG::MsgID_SystemVarChange, msg);
    }
}

void CSystemVar::SetData(uint32_t nIdx, uint64_t nVal, bool bUpdate /*= false*/, bool bSync /*= false*/)
{
    CHECK(0 > nIdx && nIdx < 4);

    m_Data[nIdx] = nVal;
    if(bUpdate && m_pRecord)
    {
        m_pRecord->Field(TBLD_SYSTEMVAR::DATA0 + nIdx) = nVal;
        Save();
    }

    if(bSync)
    {
        ServerMSG::SystemVarChange msg;
        msg.set_keyidx(GetIdx());
        msg.set_idx(nIdx);
        msg.set_val(nVal);
        msg.set_type(ServerMSG::SystemVarChange::SVCT_SET_DATA);
        ZoneService()->BroadcastToZone(ServerMSG::MsgID_SystemVarChange, msg);
        ZoneService()->SendMsgToWorld(ZoneService()->GetWorldID(), ServerMSG::MsgID_SystemVarChange, msg);
    }
}

void CSystemVar::SetStr(uint32_t nIdx, const std::string& strVal, bool bUpdate /*= false*/, bool bSync /*= false*/)
{
    CHECK(0 > nIdx && nIdx < 4);
    m_szStrData[nIdx] = strVal;
    if(bUpdate && m_pRecord)
    {
        m_pRecord->Field(TBLD_SYSTEMVAR::STR0 + nIdx) = strVal;
        Save();
    }

    if(bSync)
    {
        ServerMSG::SystemVarChange msg;
        msg.set_keyidx(GetIdx());
        msg.set_idx(nIdx);
        msg.set_str(strVal);
        msg.set_type(ServerMSG::SystemVarChange::SVCT_SET_STR);
        ZoneService()->BroadcastToZone(ServerMSG::MsgID_SystemVarChange, msg);
        ZoneService()->SendMsgToWorld(ZoneService()->GetWorldID(), ServerMSG::MsgID_SystemVarChange, msg);
    }
}

void CSystemVar::Broadcast()
{
    //发送给客户端
    SC_SYSVAR msg;
    auto      pData = msg.add_datalist();
    pData->set_keyidx(GetIdx());
    if(GetData(0) != 0)
        pData->set_data0(GetData(0));
    if(GetData(1) != 0)
        pData->set_data1(GetData(1));
    if(GetData(2) != 0)
        pData->set_data2(GetData(2));
    if(GetData(3) != 0)
        pData->set_data3(GetData(3));

    if(GetStr(0).empty() == false)
        pData->set_str0(GetStr(0));
    if(GetStr(1).empty() == false)
        pData->set_str1(GetStr(1));
    if(GetStr(2).empty() == false)
        pData->set_str2(GetStr(2));
    if(GetStr(3).empty() == false)
        pData->set_str3(GetStr(3));

    ZoneService()->BroadcastToAllPlayer(CMD_SC_SYSVAR, msg);
}

void CSystemVar::SendToPlayer(CPlayer* pPlayer)
{
    //发送给客户端
    SC_SYSVAR msg;
    auto      pData = msg.add_datalist();
    pData->set_keyidx(GetIdx());
    if(GetData(0) != 0)
        pData->set_data0(GetData(0));
    if(GetData(1) != 0)
        pData->set_data1(GetData(1));
    if(GetData(2) != 0)
        pData->set_data2(GetData(2));
    if(GetData(3) != 0)
        pData->set_data3(GetData(3));

    if(GetStr(0).empty() == false)
        pData->set_str0(GetStr(0));
    if(GetStr(1).empty() == false)
        pData->set_str1(GetStr(1));
    if(GetStr(2).empty() == false)
        pData->set_str2(GetStr(2));
    if(GetStr(3).empty() == false)
        pData->set_str3(GetStr(3));

    pPlayer->SendMsg(CMD_SC_SYSVAR, msg);
}

void CSystemVar::Save()
{
    if(m_pRecord)
        m_pRecord->Update(true);
}

void CSystemVar::ClearDirty()
{
    if(m_pRecord)
        m_pRecord->ClearDirty();
}

void CSystemVar::DeleteRecord()
{
    if(m_pRecord)
        m_pRecord->DeleteRecord(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

CSystemVarSet::CSystemVarSet()
{
    auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
    using namespace std::placeholders;
    pNetMsgProcess->Register(ServerMSG::MsgID_SystemVarChange, std::bind(&CSystemVarSet::OnSystemVarChange, this, _1));
}

CSystemVarSet::~CSystemVarSet()
{
    m_setData.clear();
}

void CSystemVarSet::OnSystemVarChange(CNetworkMessage* pMsg)
{

    ServerMSG::SystemVarChange msg;
    if(msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
    {
        return;
    }

    auto pVar = QueryVar(msg.keyidx(), true);
    switch(msg.type())
    {
        case ServerMSG::SystemVarChange::SVCT_ADD_DATA:
        {
            pVar->AddData(msg.idx(), msg.val(), UPDATE_FALSE, false);
            pVar->ClearDirty();
        }
        break;
        case ServerMSG::SystemVarChange::SVCT_SET_DATA:
        {
            pVar->SetData(msg.idx(), msg.val(), UPDATE_FALSE, false);
            pVar->ClearDirty();
        }
        break;
        case ServerMSG::SystemVarChange::SVCT_SET_STR:
        {
            pVar->SetStr(msg.idx(), msg.str(), UPDATE_FALSE, false);
            pVar->ClearDirty();
        }
        break;
        default:
            break;
    }
}

bool CSystemVarSet::Init()
{
    auto* pDB = ZoneService()->GetGameDB(ZoneService()->GetWorldID());
    CHECKF(pDB);

    auto result = pDB->Query(TBLD_SYSTEMVAR::table_name);
    if(result)
    {
        for(size_t i = 0; i < result->get_num_row(); i++)
        {
            auto row = result->fetch_row(true);
            auto pData = std::make_unique<CSystemVar>(std::move(row));
            auto key = pData->GetIdx();
            m_setData.emplace(key, std::move(pData));
            
        }
    }
    return true;
}

CSystemVar* CSystemVarSet::QueryVar(uint32_t nIdx, bool bCreateNew /*= false*/)
{
    auto it = m_setData.find(nIdx);
    if(it != m_setData.end())
    {
        return it->second.get();
    }

    if(bCreateNew)
        return CreateVar(nIdx);
    else
        return nullptr;
}

CSystemVar* CSystemVarSet::CreateVar(uint32_t nIdx)
{
    auto* pDB = ZoneService()->GetGameDB(ZoneService()->GetWorldID());
    CHECKF(pDB);
    
    if(nIdx < SYSTEMVAR_NOT_SAVE)
    {
        auto pDBRecord                           = pDB->MakeRecord(TBLD_SYSTEMVAR::table_name);
        pDBRecord->Field(TBLD_SYSTEMVAR::KEYIDX) = nIdx;
        CHECKF(pDBRecord->Update(true));
        m_setData[nIdx] = std::make_unique<CSystemVar>(std::move(pDBRecord));
    }
    else
    {
        m_setData[nIdx] = std::make_unique<CSystemVar>();
    }

    ServerMSG::SystemVarChange msg;
    msg.set_keyidx(nIdx);
    msg.set_type(ServerMSG::SystemVarChange::SVCT_CREATE);
    ZoneService()->BroadcastToZone(ServerMSG::MsgID_SystemVarChange, msg);
    ZoneService()->SendMsgToWorld(ZoneService()->GetWorldID(), ServerMSG::MsgID_SystemVarChange, msg);

    return m_setData[nIdx].get();
}

void CSystemVarSet::DeleteVar(uint32_t nIdx)
{
    auto it = m_setData.find(nIdx);
    if(it != m_setData.end())
    {
        it->second->DeleteRecord();
        m_setData.erase(it);
    }
}

void CSystemVarSet::SyncToClient(CPlayer* pPlayer)
{
    SC_SYSVAR msg;
    for(uint32_t v: AUTO_SYNC_SYSTEMVAR_LIST)
    {
        auto it = m_setData.find(v);
        if(it != m_setData.end())
        {
            auto& pSysvars = it->second;
            auto pData    = msg.add_datalist();
            pData->set_keyidx(pSysvars->GetIdx());
            pData->set_data0(pSysvars->GetData(0));
            pData->set_data1(pSysvars->GetData(1));
            pData->set_data2(pSysvars->GetData(2));
            pData->set_data3(pSysvars->GetData(3));

            pData->set_str0(pSysvars->GetStr(0));
            pData->set_str1(pSysvars->GetStr(1));
            pData->set_str2(pSysvars->GetStr(2));
            pData->set_str3(pSysvars->GetStr(3));

            constexpr int32_t MAX_DATA_PER_MSG = 64;
            if(msg.datalist_size() > MAX_DATA_PER_MSG)
            {
                pPlayer->SendMsg(CMD_SC_SYSVAR, msg);
                msg.clear_datalist();
            }
        }
    }

    if(msg.datalist_size() > 0)
        pPlayer->SendMsg(CMD_SC_SYSVAR, msg);
}