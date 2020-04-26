#include "Account.h"
#include "AccountManager.h"
#include "GMManager.h"
#include "MD5.h"
#include "MsgWorldProcess.h"
#include "WorldService.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

ON_MSG(CWorldService, CS_LOGIN)
{
    CHECK(msg.openid().empty() == false);
    if(AccountManager()->IsAuthing(msg.openid()))
    {
        LOGLOGIN("Actor:{} IsAleardyInAuth.", msg.openid().c_str());

        //当前正在验证，通知客户端
        SC_LOGIN result_msg;
        result_msg.set_result_code(SC_LOGIN::EC_WAIT_AUTH);
        WorldService()->SendToVirtualSocket(pMsg->GetFrom(), result_msg);
        return;
    }
    auto nGMLev = GMManager()->GetGMLevel(msg.openid());
    //校验程序版本号
    if(nGMLev == 0 && (msg.prog_ver().empty() || WorldService()->CheckProgVer(msg.prog_ver()) == false))
    {
        //发送错误给前端
        LOGLOGIN("Actor:{} CheckProgVerFail.", msg.openid().c_str());
        SC_LOGIN result_msg;
        result_msg.set_result_code(SC_LOGIN::EC_PROG_VER);
        WorldService()->SendToVirtualSocket(pMsg->GetFrom(), result_msg);
        return;
    }

    if(msg.last_succ_key().empty() == false)
    {
        //曾经验证成功过， 检查2次校验串
        std::string md5str =
            md5(msg.openid() + std::to_string(TimeGetSecond() / AUTH_KEY_CANUSE_SECS) + AUTH_SERVER_SIGNATURE);
        if(nGMLev == 0 && msg.last_succ_key() != md5str)
        {
            //发送错误给前端
            LOGLOGIN("Actor:{} MD5CHECKFail.", msg.openid().c_str());

            SC_LOGIN result_msg;
            result_msg.set_result_code(SC_LOGIN::EC_LAST_KEY);
            WorldService()->SendToVirtualSocket(pMsg->GetFrom(), result_msg);
            return;
        }
        else
        {
            LOGLOGIN("Actor:{} LoginAuthByMD5.", msg.openid().c_str());
            //可以直接登陆了
            ServerMSG::SocketAuth auth_msg;
            auth_msg.set_vs(pMsg->GetFrom());
            WorldService()->SendPortMsg(pMsg->GetFrom().GetServerPort(), auth_msg);

            SC_LOGIN result_msg;
            result_msg.set_result_code(SC_LOGIN::EC_SUCC);
            result_msg.set_last_succ_key(md5str);
            WorldService()->SendToVirtualSocket(pMsg->GetFrom(), result_msg);

            CAccount* pAccount = CAccount::CreateNew(msg.openid(), pMsg->GetFrom());
            //将准备好的Account放入Result列表,等待主线程处理
            AccountManager()->Login(msg.openid(), pAccount);

            return;
        }
    }
    else if(msg.auth().empty())
    {
        SC_LOGIN result_msg;
        result_msg.set_result_code(SC_LOGIN::EC_AUTH);
        WorldService()->SendToVirtualSocket(pMsg->GetFrom(), result_msg);
        return;
    }

    LOGLOGIN("Actor:{} StartAuth.", msg.openid().c_str());
    AccountManager()->Auth(msg.openid(), msg.auth(), pMsg->GetFrom());
}

ON_MSG(CWorldService, CS_CREATEACTOR)
{
    __ENTER_FUNCTION

    CAccount* pAccount = AccountManager()->QueryAccountBySocket(pMsg->GetFrom());
    CHECK(pAccount->IsWait() == false);
    pAccount->CreateActor(msg.name(), msg.prof(), msg.baselook());

    __LEAVE_FUNCTION
}

ON_MSG(CWorldService, CS_SELECTACTOR)
{
    __ENTER_FUNCTION
    CAccount* pAccount = AccountManager()->QueryAccountBySocket(pMsg->GetFrom());
    CHECK(pAccount->IsWait() == false);
    pAccount->SelectActor(msg.actor_idx());
    __LEAVE_FUNCTION
}
