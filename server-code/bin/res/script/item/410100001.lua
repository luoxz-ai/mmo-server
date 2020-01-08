--物品脚本 10100001

--注册回调,需要的才注册，不注册不会回调
script_manager:RegistFucName(410100001, SCB_ITEM_ONUSE, "x410100001_OnUse");



function x410100001_OnUse(pPlayer, pItem, nCount)
	--增加数值
	pPlayer:AddProperty(PROP_HP, 100*nCount);
	return true;
end
