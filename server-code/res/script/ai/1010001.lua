--注册回调,需要的才注册，不注册不会回调
script_manager:RegistFucName(1010001, SCB_AI_ONUNDERATTACK, "x1010001_OnUnderAttack");
script_manager:RegistFucName(1010001, SCB_AI_SEARCHENEMY, "x1010001_SearchEnemy");
script_manager:RegistFucName(1010001, SCB_AI_FINDNEXTENEMY, "x1010001_FindNextEnemy");
script_manager:RegistFucName(1010001, SCB_AI_TO_ATTACK, "x1010001_ToIdle");
script_manager:RegistFucName(1010001, SCB_AI_TO_IDLE, "x1010001_ToAttack");
script_manager:RegistFucName(1010001, SCB_AI_TO_GOBACK, "x1010001_ToGoBack");


function x1010001_OnUnderAttack(pAI, idTarget, nDamage)
	--可以根据自身血量或者状态,决定是否AddHate
	return false;	--执行c++代码将会AddHate
end

function x1010001_SearchEnemy(pAI)
	--可以根据自身血量,来判断周围的玩家, 看你到底想和谁搞一搞
	return 0; --执行c++代码
end

function x1010001_FindNextEnemy(pAI)
	--可以先在仇恨列表中查找
	if pAI:FindEnemyInHateList() == true then
		return true;
	end
	
	--也可以根据自身血量,清空仇恨列表
	
	--或者将目标设置为仇恨列表第二位之类的
	
	
	
	
	return false;	--执行c++代码
end

function x1010001_ToIdle(pAI)
	
	return false;	--执行c++代码
end

function x1010001_ToAttack(pAI)
	--进行攻击决策,决定到底使用啥技能
	
	return false;	--执行c++代码
end

function x1010001_ToGoBack(pAI)
	--ai离开战斗
	--你可以让他说句话啥的
	return false;	--执行c++代码
end