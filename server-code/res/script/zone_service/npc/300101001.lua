--NPC 小鸡 300101001

--注册回调,需要的才注册，不注册不会回调
script_manager:RegistFucName(300101001, SCB_NPC_ONACTIVE, "x300101001_OnActive");
script_manager:RegistFucName(300101001, SCB_NPC_ONBORN, "x300101001_OnBorn");


function x300101001_OnBorn(pNpc)

end

function x300101001_OnActive(pNpc, pPlayer)

end

