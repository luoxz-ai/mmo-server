#include "MapVal.h"
#include "Player.h"

CMapValSet::CMapValSet()
{

}

CMapValSet::~CMapValSet()
{

}

void CMapValSet::SendAllMapValToClient(CActor* pPlayer)
{
    __ENTER_FUNCTION
    SC_MAPVAL send;
    send.set_state(SYNC_MAPVAL_ALL);
    // send map val
    for(const auto&[k,v] : m_MapVal)
    {
        auto pMapVal = send.add_map_val_set();
        pMapVal->set_key(k);
        pMapVal->set_value(k);
    }
    //根据playerid 发送MapUserVal
    auto it = m_MapUserVal.find(pPlayer->GetID());
    if(it != m_MapUserVal.end())
    {
        const auto& refMap = *it;
        for(const auto&[k,v] : refMap)
        {
            auto pMapVal = send.add_map_val_set();
            pMapVal->set_key(k);
            pMapVal->set_value(k);
        }
    }
    pPlayer->SendMsg(CMD_SC_MAPVAL, send);
    __LEAVE_FUNCTION
}

void CMapValSet::ClearAllMapVal()
{
    __ENTER_FUNCTION
    ClearMapVal();
    ClearAllMapUserVal();
    __LEAVE_FUNCTION
}

void CMapValSet::SetMapVal(uint32_t nIdx, int64_t nVal, bool bSync /*= false*/)
{
    __ENTER_FUNCTION
    m_MapVal[nIdx] = nVal;
    if(bSync)
    {
        // send msg
        SC_MAPVAL send;
        send.set_state(SYNC_MAPVAL_ONE);
        auto pMapVal = send.add_map_val_set();
        pMapVal->set_key(nIdx);
        pMapVal->set_value(nVal);
        m_pScene->SendSceneMessage(CMD_SC_MAPVAL, send);
    }
    __LEAVE_FUNCTION
}

int64_t CMapValSet::GetMapVal(uint32_t nIdx)
{
    return m_MapVal[nIdx];
}

int64_t CMapValSet::AddMapVal(uint32_t nIdx, int64_t nVal, bool bSync /*= false*/)
{
    __ENTER_FUNCTION
    auto& refData = m_MapVal[nIdx];
    refData += nVal;
    if(bSync)
    {
        // send msg
        SC_MAPVAL send;
        send.set_state(SYNC_MAPVAL_ONE);
        auto pMapVal = send.add_map_val_set();
        pMapVal->set_key(nIdx);
        pMapVal->set_value(refData);
        m_pScene->SendSceneMessage(CMD_SC_MAPVAL, send);
    }

    return refData;
    __LEAVE_FUNCTION
    return 0;
}

void CMapValSet::ClearMapVal()
{
    m_MapVal.clear();
}

void CMapValSet::SyncAllMapVal()
{
    __ENTER_FUNCTION
    SC_MAPVAL send;
    send.set_state(SYNC_MAPVAL_ALL);
    // send map val
    for(const auto&[k,v] : m_MapVal)
    {
        auto pMapVal = send.add_map_val_set();
        pMapVal->set_key(k);
        pMapVal->set_value(k);
    }
    
    pPlayer->SendMsg(CMD_SC_MAPVAL, send);
    __LEAVE_FUNCTION
}

void CMapValSet::SetMapUserVal(uint64_t idUser, uint32_t nIdx, int64_t nVal, bool bSync /*= false*/)
{
    __ENTER_FUNCTION
    m_MapUserVal[idUser][nIdx] = nVal;

    if(bSync)
    {
        // send msg
        SC_MAPVAL send;
        send.set_state(SYNC_MAPVAL_ONE);
        auto pMapVal = send.add_user_map_val_set();
        pMapVal->set_key(nIdx);
        pMapVal->set_value(nVal);
        CPlyaer* pPlayer = ActorManager()->QueryPlayer(idUser);
        if(pPlayer)
        {
            pPlayer->SendMsg(CMD_SC_MAPVAL, send);
        }
    }
    __LEAVE_FUNCTION
}

int64_t CMapValSet::GetMapUserVal(uint64_t idUser, uint32_t nIdx)
{
    return m_MapUserVal[idUser][nIdx];
}

int64_t CMapValSet::AddMapUserVal(uint64_t idUser, uint32_t nIdx, int64_t nVal, bool bSync /*= false*/)
{
    __ENTER_FUNCTION
    auto& refData = m_MapUserVal[idUser][nIdx];
    refData += nVal;
    if(bSync)
    {
        // send msg
        SC_MAPVAL send;
        send.set_state(SYNC_MAPVAL_ONE);
        auto pMapVal = send.add_user_map_val_set();
        pMapVal->set_key(nIdx);
        pMapVal->set_value(refData);
        CPlyaer* pPlayer = ActorManager()->QueryPlayer(idUser);
        if(pPlayer)
        {
            pPlayer->SendMsg(CMD_SC_MAPVAL, send);
        }
    }

    return refData;
    __LEAVE_FUNCTION
    return 0;
}

void CMapValSet::ClearMapUserVal(uint64_t idUser)
{
    __ENTER_FUNCTION
    auto itFind = m_MapUserVal.find(idUser);
    if(itFind != m_MapUserVal.end())
        m_MapUserVal.erase(itFind);
    __LEAVE_FUNCTION
}

void CMapValSet::ClearAllMapUserVal()
{
    m_MapUserVal.clear();
}