#include "AIService.h"

#include <functional>

#include "AIMonster.h"
#include "AIPlayer.h"
#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "SettingMap.h"
#include "tinyxml2/tinyxml2.h"
#include "AISceneManagr.h"
#include "AIActorManager.h"

#include "AIFuzzyLogic.h"
#include "AISkill.h"
#include "AIType.h"
#include "MonitorMgr.h"
#include "MonsterType.h"

static thread_local CAIService* tls_pService;
CAIService*                     AIService()
{
    return tls_pService;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint16_t idService)
{
    return CAIService::CreateNew(ServerPort{idWorld, idService});
}

//////////////////////////////////////////////////////////////////////////
CAIService::CAIService()
{
    m_tLastDisplayTime.Startup(60);
}

CAIService::~CAIService()
{

}

void CAIService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    StopLogicThread();
    if(m_pAISceneManager)
    {
        m_pAISceneManager->Destory();
        m_pAISceneManager.reset();
    }
    if(m_pAIActorManager)
    {
        m_pAIActorManager->Destroy();
        m_pAIActorManager.reset();
    }
    DestoryServiceCommon();
}


bool CAIService::Init(const ServerPort& nServerPort)
{
    //各种初始化
    scope_guards scope_exit;
    tls_pService = this;
    scope_exit += []() {
        tls_pService = nullptr;
    };

    CServiceCommon::Init(nServerPort);
    auto oldNdc = BaseCode::SetNdc(GetServiceName());
    scope_exit += [oldNdc]() {
        BaseCode::SetNdc(oldNdc);
        ;
    };

    extern void export_to_lua(lua_State*, void*);
    m_pScriptManager.reset(
        CLUAScriptManager::CreateNew(std::string("AIScript") + std::to_string(GetServerPort().GetServiceID()),
                                     &export_to_lua,
                                     (void*)this,
                                     "res/script/ai_service",
                                     true));

    m_pMapManager.reset(CMapManager::CreateNew(GetZoneID()));
    CHECKF(m_pMapManager.get());

    DEFINE_CONFIG_LOAD(CTargetFAMSet);
    DEFINE_CONFIG_LOAD(CSkillFAMSet);
    DEFINE_CONFIG_LOAD(CSkillTypeSet);
    DEFINE_CONFIG_LOAD(CAITypeSet);
    DEFINE_CONFIG_LOAD(CMonsterTypeSet);

    m_pAISceneManager.reset(CAISceneManager::CreateNew(GetZoneID()));
    CHECKF(m_pAISceneManager.get());
    m_pAIActorManager.reset(CAIActorManager::CreateNew());
    CHECKF(m_pAIActorManager.get());

    extern void AIServiceMessageHandlerRegister();
    AIServiceMessageHandlerRegister();

    if(CreateService(20) == false)
        return false;

    return true;
}

void CAIService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
    switch(pNetworkMsg->GetMsgHead()->usCmd)
    {
        case NETMSG_SCK_CLOSE:
        {
            MSG_SCK_CLOSE* pMsg = (MSG_SCK_CLOSE*)pNetworkMsg->GetBuf();
        }
        break;
        default:
        {
            m_pNetMsgProcess->Process(pNetworkMsg);
        }
        break;
    }
}

bool CAIService::SendMsgToZone(uint16_t nCmd, const google::protobuf::Message& msg)
{
    CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket(), GetZoneServiceVirtualSocket());
    return SendMsg(_msg);
}

void CAIService::OnLogicThreadProc()
{
    __ENTER_FUNCTION
    CServiceCommon::OnLogicThreadProc();

    AISceneManager()->OnTimer();

    if(m_tLastDisplayTime.ToNextTime())
    {
        std::string buf = std::string("\n======================================================================") +
                          fmt::format(FMT_STRING("\nEvent:{}\tMem:{}"), EventManager()->GetEventCount(), get_thread_memory_allocted());
        auto pMessagePort = GetMessageRoute()->QueryMessagePort(GetZoneServiceVirtualSocket().GetServerPort(), false);
        if(pMessagePort)
        {
            buf +=
                fmt::format(FMT_STRING("\nMsgPort:{}\tSendBuff:{}"), GetZoneID(), pMessagePort->GetWriteBufferSize());
        }
        LOGMONITOR("{}", buf.c_str());
        m_pMonitorMgr->Print();
        SetMessageProcess(0);
    }
    __LEAVE_FUNCTION
}

void CAIService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CAIService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}
