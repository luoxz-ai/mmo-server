--地图 xxx 00101

--注册回调,需要的才注册，不注册不会回调
script_manager:RegistFucName(100000101, SCB_MAP_ONCREATE, "x100000101_OnCreate");
script_manager:RegistFucName(100000101, SCB_MAP_ONENTERMAP, "x100000101_OnEnterMap");
script_manager:RegistFucName(100000101, SCB_MAP_ONLEAVEMAP, "x100000101_OnLeaveMap");
--script_manager:RegistFucName(100000101, SCB_MAP_ONACTORBEKILL, "x100000101_OnActorDead");

function x100000101_OnCreate(pMap)

end

function x100000101_OnEnterMap(pMap, pPlayer)

end

function x100000101_OnLeaveMap(pMap, pPlayer, TargetMapID)

end


function x100000101_OnActorDead(pMap, pActor)

end

