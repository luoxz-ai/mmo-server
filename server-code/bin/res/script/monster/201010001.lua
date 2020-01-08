--怪物 小鸡 1010001

--注册回调,需要的才注册，不注册不会回调
script_manager:RegistFucName(201010001, SCB_MONSTER_ONATTACK, "x201010001_OnAttack");
script_manager:RegistFucName(201010001, SCB_MONSTER_ONBEATTACK, "x201010001_OnBeAttack");
script_manager:RegistFucName(201010001, SCB_MONSTER_ONBEKILL, "x201010001_OnBeKill");
script_manager:RegistFucName(201010001, SCB_MONSTER_ONBORN, "x201010001_OnBorn");


function x201010001_OnBorn(pMonster)

end

function x201010001_OnBeKill(pMonster, pKiller)

end

function x201010001_OnAttack(pMonster, pTarget)

end

function x201010001_OnBeAttack(pMonster, pTarget)

end
