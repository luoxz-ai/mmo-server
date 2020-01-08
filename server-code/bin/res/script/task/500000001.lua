--任务脚本 1

--注册回调,需要的才注册，不注册不会回调
script_manager:RegistFucName(500000001, SCB_TASK_CAN_ACCEPT, "x500000001_CanAccept");
local this_task_id = 1;

function x500000001_CanAccept(pPlayer)
end

function x500000001_OnAccept(pPlayer)
end

function x500000001_CanCommit(pPlayer)
end

function x500000001_OnCommit(pPlayer)
end

