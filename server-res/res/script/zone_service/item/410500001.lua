--物品脚本 10500001

--注册回调,需要的才注册，不注册不会回调
script_manager:RegistFucName(410500001, SCB_ITEM_ONUSE, "x410500001_OnUse");
script_manager:RegistFucName(410500001, SCB_ITEM_ONBATCHUSECHECK, "x410500001_OnCheckUse");



function x410500001_OnUse(pPlayer, pItem, nCount)
	--增加数值
	pPlayer:AddProperty(PROP_MONEY, 100*nCount);
	return true;
end

function x410500001_OnCheckUse(pPlayer, pItem, nCount)
	--增加数值
	local nMoney = pPlayer:GetProperty(PROP_MONEY);
	local nMaxMoney = pPlayer:GetPropertyMax(PROP_MONEY);
	local nCanUse = match:floor((nMaxMoney - nMoney) / 100);
	return (nCanUse > nCount) and nCount or nCanUse;
	
end