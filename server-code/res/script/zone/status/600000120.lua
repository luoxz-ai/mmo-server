--状态 xxx 120

--注册回调,需要的才注册，不注册不会回调
script_manager:RegistFucName(600000120, SCB_STATUS_ONATTACH, "x600000120_OnAttach");
script_manager:RegistFucName(600000120, SCB_STATUS_ONDETACH, "x600000120_OnDetach");
script_manager:RegistFucName(600000120, SCB_STATUS_ONEFFECT, "x600000120_OnEffect");
script_manager:RegistFucName(600000120, SCB_STATUS_ONATTACK, "x600000120_OnAttack");
script_manager:RegistFucName(600000120, SCB_STATUS_ONBEATTACK, "x600000120_OnBeAttack");
script_manager:RegistFucName(600000120, SCB_STATUS_ONMOVE, "x600000120_OnMove");
script_manager:RegistFucName(600000120, SCB_STATUS_ONLEAVEMAP, "x600000120_OnLeaveMap");
script_manager:RegistFucName(600000120, SCB_STATUS_ONLOGIN, "x600000120_OnLogin");
script_manager:RegistFucName(600000120, SCB_STATUS_ONLOGOUT, "x600000120_OnLogout");


--当挂接本状态时
function x600000120_OnAttach(pStatus)

end

--当结束挂接时
function x600000120_OnDetach(pStatus)

end

--当时间到需要作用时
function x600000120_OnEffect(pStatus)

end

--当自己攻击别人时
function x600000120_OnAttack(pStatus, pTarget)
	
end

--当自己被别人攻击时
function x600000120_OnBeAttack(pStatus, pAttacker)

end

--当自己移动时
function x600000120_OnMove(pStatus)

end

--当自己登陆时
function x600000120_OnLogin(pStatus)

end

--当自己退出登陆时
function x600000120_OnLogout(pStatus)

end