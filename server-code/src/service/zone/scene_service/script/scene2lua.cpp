#include "AchievementType.h"
#include "Actor.h"
#include "ActorManager.h"
#include "ActorStatus.h"
#include "Bullet.h"
#include "BulletType.h"
#include "BuyLimit.h"
#include "CommonData.h"
#include "CoolDown.h"
#include "DataCount.h"
#include "DropLimit.h"
#include "DropRule.h"
#include "DynaMonster.h"
#include "DynaNpc.h"
#include "Equipment.h"
#include "Frield.h"
#include "FrieldSet.h"
#include "GMManager.h"
#include "IStatus.h"
#include "Item.h"
#include "ItemAddition.h"
#include "ItemData.h"
#include "ItemFormula.h"
#include "ItemType.h"
#include "ItemUpgrade.h"
#include "LoadingThread.h"
#include "Mail.h"
#include "MailBox.h"
#include "Mall.h"
#include "MapItem.h"
#include "MapVal.h"
#include "Monster.h"
#include "MonsterType.h"
#include "MsgSceneProcess.h"
#include "Npc.h"
#include "NpcType.h"
#include "Package.h"
#include "Pet.h"
#include "PetSet.h"
#include "PetType.h"
#include "Phase.h"
#include "Player.h"
#include "PlayerAchievement.h"
#include "PlayerTask.h"
#include "PlayerTaskData.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SceneService.h"
#include "SkillData.h"
#include "SkillFSM.h"
#include "SkillManager.h"
#include "StoragePackage.h"
#include "SuitEquip.h"
#include "SystemVars.h"
#include "TaskType.h"
#include "TeamInfoManager.h"
#include "lua_tinker.h"
void scene2lua(lua_State* L)
{
    lua_tinker::class_add<Angle>(L, "Angle", false);
    lua_tinker::class_add<AttachStatusInfo>(L, "AttachStatusInfo", false);
    lua_tinker::class_add<AxisAlignedBox>(L, "AxisAlignedBox", false);
    lua_tinker::class_def_static<AxisAlignedBox>(L, "BOX_INFINITE", &AxisAlignedBox::BOX_INFINITE);
    lua_tinker::class_def_static<AxisAlignedBox>(L, "BOX_NULL", &AxisAlignedBox::BOX_NULL);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "contains",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(const AxisAlignedBox&) const)(&AxisAlignedBox::contains)),
            lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(const Vector3&) const)(&AxisAlignedBox::contains))));
    lua_tinker::class_def<AxisAlignedBox>(L, "distance", &AxisAlignedBox::distance);
    lua_tinker::class_def<AxisAlignedBox>(L, "getAllCorners", &AxisAlignedBox::getAllCorners);
    lua_tinker::class_def<AxisAlignedBox>(L, "getCenter", &AxisAlignedBox::getCenter);
    lua_tinker::class_def<AxisAlignedBox>(L, "getCorner", &AxisAlignedBox::getCorner);
    lua_tinker::class_def<AxisAlignedBox>(L, "getHalfSize", &AxisAlignedBox::getHalfSize);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "getMaximum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((Vector3 & (AxisAlignedBox::*)())(&AxisAlignedBox::getMaximum)),
            lua_tinker::make_member_functor_ptr((const Vector3& (AxisAlignedBox::*)() const)(&AxisAlignedBox::getMaximum))));
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "getMinimum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((Vector3 & (AxisAlignedBox::*)())(&AxisAlignedBox::getMinimum)),
            lua_tinker::make_member_functor_ptr((const Vector3& (AxisAlignedBox::*)() const)(&AxisAlignedBox::getMinimum))));
    lua_tinker::class_def<AxisAlignedBox>(L, "getSize", &AxisAlignedBox::getSize);
    lua_tinker::class_def<AxisAlignedBox>(L, "intersection", &AxisAlignedBox::intersection);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "intersects",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(const AxisAlignedBox&) const)(&AxisAlignedBox::intersects)),
            lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(const Vector3&) const)(&AxisAlignedBox::intersects))));
    lua_tinker::class_def<AxisAlignedBox>(L, "isFinite", &AxisAlignedBox::isFinite);
    lua_tinker::class_def<AxisAlignedBox>(L, "isInfinite", &AxisAlignedBox::isInfinite);
    lua_tinker::class_def<AxisAlignedBox>(L, "isNull", &AxisAlignedBox::isNull);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "merge",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const AxisAlignedBox&))(&AxisAlignedBox::merge)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Vector3&))(&AxisAlignedBox::merge))));
    lua_tinker::class_def<AxisAlignedBox>(L, "operator!=", &AxisAlignedBox::operator!=);
    lua_tinker::class_def<AxisAlignedBox>(L, "__eq", &AxisAlignedBox::operator==);
    lua_tinker::class_def<AxisAlignedBox>(L, "scale", &AxisAlignedBox::scale);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "setExtents",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Vector3&, const Vector3&))(&AxisAlignedBox::setExtents)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(float, float, float, float, float, float))(&AxisAlignedBox::setExtents))));
    lua_tinker::class_def<AxisAlignedBox>(L, "setInfinite", &AxisAlignedBox::setInfinite);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "setMaximum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Vector3&))(&AxisAlignedBox::setMaximum)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(float, float, float))(&AxisAlignedBox::setMaximum))));
    lua_tinker::class_def<AxisAlignedBox>(L, "setMaximumX", &AxisAlignedBox::setMaximumX);
    lua_tinker::class_def<AxisAlignedBox>(L, "setMaximumY", &AxisAlignedBox::setMaximumY);
    lua_tinker::class_def<AxisAlignedBox>(L, "setMaximumZ", &AxisAlignedBox::setMaximumZ);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "setMinimum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Vector3&))(&AxisAlignedBox::setMinimum)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(float, float, float))(&AxisAlignedBox::setMinimum))));
    lua_tinker::class_def<AxisAlignedBox>(L, "setMinimumX", &AxisAlignedBox::setMinimumX);
    lua_tinker::class_def<AxisAlignedBox>(L, "setMinimumY", &AxisAlignedBox::setMinimumY);
    lua_tinker::class_def<AxisAlignedBox>(L, "setMinimumZ", &AxisAlignedBox::setMinimumZ);
    lua_tinker::class_def<AxisAlignedBox>(L, "setNull", &AxisAlignedBox::setNull);
    lua_tinker::class_def<AxisAlignedBox>(L, "squaredDistance", &AxisAlignedBox::squaredDistance);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "transform",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Affine3&))(&AxisAlignedBox::transform)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Matrix4&))(&AxisAlignedBox::transform))));
    lua_tinker::class_def<AxisAlignedBox>(L, "volume", &AxisAlignedBox::volume);
    lua_tinker::class_con<AxisAlignedBox>(
        L,
        lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<AxisAlignedBox>(),
                                                   new lua_tinker::constructor<AxisAlignedBox, AxisAlignedBox::Extent>(),
                                                   new lua_tinker::constructor<AxisAlignedBox, const Vector3&, const Vector3&>(),
                                                   new lua_tinker::constructor<AxisAlignedBox, float, float, float, float, float, float>()));
    lua_tinker::class_var_static<AxisAlignedBox>(L, "EXTENT_FINITE", AxisAlignedBox::EXTENT_FINITE);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "EXTENT_INFINITE", AxisAlignedBox::EXTENT_INFINITE);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "EXTENT_NULL", AxisAlignedBox::EXTENT_NULL);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "FAR_LEFT_BOTTOM", AxisAlignedBox::FAR_LEFT_BOTTOM);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "FAR_LEFT_TOP", AxisAlignedBox::FAR_LEFT_TOP);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "FAR_RIGHT_BOTTOM", AxisAlignedBox::FAR_RIGHT_BOTTOM);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "FAR_RIGHT_TOP", AxisAlignedBox::FAR_RIGHT_TOP);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "NEAR_LEFT_BOTTOM", AxisAlignedBox::NEAR_LEFT_BOTTOM);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "NEAR_LEFT_TOP", AxisAlignedBox::NEAR_LEFT_TOP);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "NEAR_RIGHT_BOTTOM", AxisAlignedBox::NEAR_RIGHT_BOTTOM);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "NEAR_RIGHT_TOP", AxisAlignedBox::NEAR_RIGHT_TOP);
    lua_tinker::class_add<CActor>(L, "CActor", false);
    lua_tinker::class_def<CActor>(L, "AddDelayAttribChange", &CActor::AddDelayAttribChange);
    lua_tinker::class_def<CActor>(L, "AddDelaySendShowToAllViewPlayer", &CActor::AddDelaySendShowToAllViewPlayer);
    lua_tinker::class_def<CActor>(L, "AddHide", &CActor::AddHide);
    lua_tinker::class_def<CActor>(L, "AddProperty", &CActor::AddProperty, SYNC_TRUE);
    lua_tinker::class_def<CActor>(L, "BeAttack", &CActor::BeAttack);
    lua_tinker::class_def<CActor>(L, "BeKillBy", &CActor::BeKillBy);
    lua_tinker::class_def<CActor>(L, "CalcDefence", &CActor::CalcDefence);
    lua_tinker::class_def<CActor>(L, "CanDamage", &CActor::CanDamage);
    lua_tinker::class_def<CActor>(L, "CanMove", &CActor::CanMove);
    lua_tinker::class_def<CActor>(L, "ChangePhase", &CActor::ChangePhase);
    lua_tinker::class_def<CActor>(L, "CheckCanMove", &CActor::CheckCanMove, true);
    lua_tinker::class_def<CActor>(L, "ClearViewList", &CActor::ClearViewList);
    lua_tinker::class_def<CActor>(L, "DelThis", &CActor::DelThis);
    lua_tinker::class_def<CActor>(L, "FlyTo", &CActor::FlyTo);
    lua_tinker::class_def<CActor>(L,
                                  "GetAttrib",
                                  lua_tinker::args_type_overload_member_functor(
                                      lua_tinker::make_member_functor_ptr((CActorAttrib & (CActor::*)())(&CActor::GetAttrib)),
                                      lua_tinker::make_member_functor_ptr((const CActorAttrib& (CActor::*)() const)(&CActor::GetAttrib))));
    lua_tinker::class_def<CActor>(L, "GetCDSet", &CActor::GetCDSet);
    lua_tinker::class_def<CActor>(L, "GetCampID", &CActor::GetCampID);
    lua_tinker::class_def<CActor>(L, "GetEventMapRef", &CActor::GetEventMapRef);
    lua_tinker::class_def<CActor>(L, "GetEventQueueRef", &CActor::GetEventQueueRef);
    lua_tinker::class_def<CActor>(L, "GetFP", &CActor::GetFP);
    lua_tinker::class_def<CActor>(L, "GetFPMax", &CActor::GetFPMax);
    lua_tinker::class_def<CActor>(L, "GetHP", &CActor::GetHP);
    lua_tinker::class_def<CActor>(L, "GetHPMax", &CActor::GetHPMax);
    lua_tinker::class_def<CActor>(L, "GetLastMoveTime", &CActor::GetLastMoveTime);
    lua_tinker::class_def<CActor>(L, "GetLev", &CActor::GetLev);
    lua_tinker::class_def<CActor>(L, "GetMP", &CActor::GetMP);
    lua_tinker::class_def<CActor>(L, "GetMPMax", &CActor::GetMPMax);
    lua_tinker::class_def<CActor>(L, "GetMapID", &CActor::GetMapID);
    lua_tinker::class_def<CActor>(L, "GetMoveSpeed", &CActor::GetMoveSpeed);
    lua_tinker::class_def<CActor>(L, "GetNP", &CActor::GetNP);
    lua_tinker::class_def<CActor>(L, "GetNPMax", &CActor::GetNPMax);
    lua_tinker::class_def<CActor>(L, "GetName", &CActor::GetName);
    lua_tinker::class_def<CActor>(L, "GetProperty", &CActor::GetProperty);
    lua_tinker::class_def<CActor>(L, "GetPropertyMax", &CActor::GetPropertyMax);
    lua_tinker::class_def<CActor>(L, "GetSceneIdx", &CActor::GetSceneIdx);
    lua_tinker::class_def<CActor>(L, "GetSkillFSM", &CActor::GetSkillFSM);
    lua_tinker::class_def<CActor>(L, "GetStatus", &CActor::GetStatus);
    lua_tinker::class_def<CActor>(L, "GetWorldID", &CActor::GetWorldID);
    lua_tinker::class_def<CActor>(L, "HitTest", &CActor::HitTest);
    lua_tinker::class_def<CActor>(L, "IsDead", &CActor::IsDead);
    lua_tinker::class_def<CActor>(L, "IsDelThis", &CActor::IsDelThis);
    lua_tinker::class_def<CActor>(L, "IsEnemy", &CActor::IsEnemy);
    lua_tinker::class_def<CActor>(L, "MoveTo", &CActor::MoveTo, true);
    lua_tinker::class_def<CActor>(L, "RecalcAttrib", &CActor::RecalcAttrib, false);
    lua_tinker::class_def<CActor>(L, "RemoveHide", &CActor::RemoveHide);
    lua_tinker::class_def<CActor>(L, "SendDelayAttribChage", &CActor::SendDelayAttribChage);
    lua_tinker::class_def<CActor>(L, "SendMsg", &CActor::SendMsg);
    lua_tinker::class_def<CActor>(L, "SendRoomMessage", &CActor::SendRoomMessage, true);
    lua_tinker::class_def<CActor>(L, "SendShowTo", &CActor::SendShowTo);
    lua_tinker::class_def<CActor>(L, "SendShowToDealyList", &CActor::SendShowToDealyList);
    lua_tinker::class_def<CActor>(L, "SendWorldMessage", &CActor::SendWorldMessage);
    lua_tinker::class_def<CActor>(L, "SetCampID", &CActor::SetCampID, SYNC_FALSE);
    lua_tinker::class_def<CActor>(L, "SetLastMoveTime", &CActor::SetLastMoveTime);
    lua_tinker::class_def<CActor>(L, "SetProperty", &CActor::SetProperty, SYNC_TRUE);
    lua_tinker::class_def<CActor>(L, "UpdateFight", &CActor::UpdateFight);
    lua_tinker::class_def<CActor>(L, "_CastSkill", &CActor::_CastSkill);
    lua_tinker::class_def<CActor>(L, "_SetFP", &CActor::_SetFP);
    lua_tinker::class_def<CActor>(L, "_SetHP", &CActor::_SetHP);
    lua_tinker::class_def<CActor>(L, "_SetMP", &CActor::_SetMP);
    lua_tinker::class_def<CActor>(L, "_SetNP", &CActor::_SetNP);
    lua_tinker::class_def<CActor>(L, "_SetProperty", &CActor::_SetProperty, SYNC_TRUE);
    lua_tinker::class_add<CActorStatus>(L, "CActorStatus", false);
    lua_tinker::class_def<CActorStatus>(
        L,
        "AttachStatus",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((bool (CActorStatus::*)(const AttachStatusInfo&))(&CActorStatus::AttachStatus)),
            lua_tinker::make_member_functor_ptr((bool (CActorStatus::*)(uint32_t, OBJID))(&CActorStatus::AttachStatus))));
    lua_tinker::class_def<CActorStatus>(L, "DetachStatus", &CActorStatus::DetachStatus);
    lua_tinker::class_def<CActorStatus>(L, "DetachStatusByFlag", &CActorStatus::DetachStatusByFlag, true);
    lua_tinker::class_def<CActorStatus>(L, "DetachStatusByType", &CActorStatus::DetachStatusByType);
    lua_tinker::class_def<CActorStatus>(L, "FillStatusMsg", &CActorStatus::FillStatusMsg);
    lua_tinker::class_def<CActorStatus>(L, "QueryStatusByType", &CActorStatus::QueryStatusByType);
    lua_tinker::class_def<CActorStatus>(L, "SyncTo", &CActorStatus::SyncTo);
    lua_tinker::class_def<CActorStatus>(L, "TestStatusByFlag", &CActorStatus::TestStatusByFlag);
    lua_tinker::class_def<CActorStatus>(L, "TestStatusByType", &CActorStatus::TestStatusByType);
    lua_tinker::class_def<CActorStatus>(L, "size", &CActorStatus::size);
    lua_tinker::class_add<CBullet>(L, "CBullet", false);
    lua_tinker::class_def<CBullet>(L, "BeKillBy", &CBullet::BeKillBy);
    lua_tinker::class_def<CBullet>(L, "CanDamage", &CBullet::CanDamage);
    lua_tinker::class_def<CBullet>(L, "GetActorType", &CBullet::GetActorType);
    lua_tinker::class_def_static<CBullet>(L, "GetActorTypeStatic", &CBullet::GetActorTypeStatic);
    lua_tinker::class_def<CBullet>(L, "GetOwnerID", &CBullet::GetOwnerID);
    lua_tinker::class_def<CBullet>(L, "IsEnemy", &CBullet::IsEnemy);
    lua_tinker::class_add<CCommonData>(L, "CCommonData", false);
    lua_tinker::class_def<CCommonData>(L, "AddData", &CCommonData::AddData, false, false);
    lua_tinker::class_def<CCommonData>(L, "DeleteRecord", &CCommonData::DeleteRecord);
    lua_tinker::class_def<CCommonData>(L, "GetData", &CCommonData::GetData);
    lua_tinker::class_def<CCommonData>(L, "GetIdx", &CCommonData::GetIdx);
    lua_tinker::class_def<CCommonData>(L, "Save", &CCommonData::Save);
    lua_tinker::class_def<CCommonData>(L, "SetData", &CCommonData::SetData, false, false);
    lua_tinker::class_def<CCommonData>(L, "Sync", &CCommonData::Sync);
    lua_tinker::class_add<CCommonDataSet>(L, "CCommonDataSet", false);
    lua_tinker::class_def<CCommonDataSet>(L, "CreateData", &CCommonDataSet::CreateData);
    lua_tinker::class_def<CCommonDataSet>(L, "DeleteData", &CCommonDataSet::DeleteData);
    lua_tinker::class_def<CCommonDataSet>(L, "QueryData", &CCommonDataSet::QueryData, false);
    lua_tinker::class_def<CCommonDataSet>(L, "Save", &CCommonDataSet::Save);
    lua_tinker::class_def<CCommonDataSet>(L, "SyncAll", &CCommonDataSet::SyncAll);
    lua_tinker::class_add<CCoolDown>(L, "CCoolDown", false);
    lua_tinker::class_add<CCoolDownSet>(L, "CCoolDownSet", false);
    lua_tinker::class_def<CCoolDownSet>(L, "ClearCoolDown", &CCoolDownSet::ClearCoolDown, false, true);
    lua_tinker::class_def<CCoolDownSet>(
        L,
        "IsCoolDown",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((bool (CCoolDownSet::*)(uint32_t, uint32_t))(&CCoolDownSet::IsCoolDown)),
            lua_tinker::make_member_functor_ptr((bool (CCoolDownSet::*)(uint32_t, uint32_t, uint32_t))(&CCoolDownSet::IsCoolDown))));
    lua_tinker::class_def<CCoolDownSet>(L, "Save", &CCoolDownSet::Save);
    lua_tinker::class_def<CCoolDownSet>(L, "StartCoolDown", &CCoolDownSet::StartCoolDown, false, true);
    lua_tinker::class_def<CCoolDownSet>(L, "SyncAll", &CCoolDownSet::SyncAll);
    lua_tinker::class_add<CDataCount>(L, "CDataCount", false);
    lua_tinker::class_def<CDataCount>(L, "AddData", &CDataCount::AddData);
    lua_tinker::class_def<CDataCount>(L, "DeleteRecord", &CDataCount::DeleteRecord);
    lua_tinker::class_def<CDataCount>(L, "GetDataNum", &CDataCount::GetDataNum);
    lua_tinker::class_def<CDataCount>(L, "GetIdx", &CDataCount::GetIdx);
    lua_tinker::class_def<CDataCount>(L, "GetNextResetTime", &CDataCount::GetNextResetTime);
    lua_tinker::class_def<CDataCount>(L, "GetType", &CDataCount::GetType);
    lua_tinker::class_def<CDataCount>(L, "Save", &CDataCount::Save);
    lua_tinker::class_def<CDataCount>(L, "SetData", &CDataCount::SetData);
    lua_tinker::class_def<CDataCount>(L, "Sync", &CDataCount::Sync);
    lua_tinker::class_add<CDataCountSet>(L, "CDataCountSet", false);
    lua_tinker::class_def<CDataCountSet>(L, "AddCount", &CDataCountSet::AddCount, false);
    lua_tinker::class_def<CDataCountSet>(L, "DeleteCount", &CDataCountSet::DeleteCount);
    lua_tinker::class_def<CDataCountSet>(L, "GetCount", &CDataCountSet::GetCount);
    lua_tinker::class_def<CDataCountSet>(L, "GetMaxCount", &CDataCountSet::GetMaxCount);
    lua_tinker::class_def<CDataCountSet>(L, "Save", &CDataCountSet::Save);
    lua_tinker::class_def<CDataCountSet>(L, "SetCount", &CDataCountSet::SetCount, false);
    lua_tinker::class_def<CDataCountSet>(L, "SyncAll", &CDataCountSet::SyncAll);
    lua_tinker::class_def<CDataCountSet>(L, "_AddCount", &CDataCountSet::_AddCount, false);
    lua_tinker::class_add<CDynaMonster>(L, "CDynaMonster", false);
    lua_tinker::class_add<CDynaNpc>(L, "CDynaNpc", false);
    lua_tinker::class_add<CEquipment>(L, "CEquipment", false);
    lua_tinker::class_def<CEquipment>(L, "AddEquipmentDurability", &CEquipment::AddEquipmentDurability);
    lua_tinker::class_def<CEquipment>(L, "CanEquip", &CEquipment::CanEquip);
    lua_tinker::class_def<CEquipment>(L, "CanEquipByItemType", &CEquipment::CanEquipByItemType);
    lua_tinker::class_def<CEquipment>(L, "DecEquipmentDurability", &CEquipment::DecEquipmentDurability, 1);
    lua_tinker::class_def<CEquipment>(L, "DeleteAll", &CEquipment::DeleteAll, true, true);
    lua_tinker::class_def<CEquipment>(L, "EquipItem", &CEquipment::EquipItem, true);
    lua_tinker::class_def<CEquipment>(L, "GetArmorTypeID", &CEquipment::GetArmorTypeID);
    lua_tinker::class_def<CEquipment>(L, "GetSuitTypeAmount", &CEquipment::GetSuitTypeAmount);
    lua_tinker::class_def<CEquipment>(L, "GetWeaponTypeID", &CEquipment::GetWeaponTypeID);
    lua_tinker::class_def<CEquipment>(L, "QueryEquipment", &CEquipment::QueryEquipment);
    lua_tinker::class_def<CEquipment>(L, "QueryEquipmentById", &CEquipment::QueryEquipmentById);
    lua_tinker::class_def<CEquipment>(L, "SaveAll", &CEquipment::SaveAll);
    lua_tinker::class_def<CEquipment>(L, "SendInfo", &CEquipment::SendInfo);
    lua_tinker::class_def<CEquipment>(L, "UnequipItem", &CEquipment::UnequipItem, true, true, true);
    lua_tinker::class_add<CGameMap>(L, "CGameMap", false);
    lua_tinker::class_def<CGameMap>(L, "FindPosNearby", &CGameMap::FindPosNearby);
    lua_tinker::class_def<CGameMap>(L, "GetEnterPointByIdx", &CGameMap::GetEnterPointByIdx);
    lua_tinker::class_def<CGameMap>(L, "GetGeneratorData", &CGameMap::GetGeneratorData);
    lua_tinker::class_def<CGameMap>(L, "GetHigh", &CGameMap::GetHigh);
    lua_tinker::class_def<CGameMap>(L, "GetLeavePointByIdx", &CGameMap::GetLeavePointByIdx);
    lua_tinker::class_def<CGameMap>(L, "GetMapData", &CGameMap::GetMapData);
    lua_tinker::class_def<CGameMap>(L, "GetMapFlag", &CGameMap::GetMapFlag);
    lua_tinker::class_def<CGameMap>(L, "GetMapID", &CGameMap::GetMapID);
    lua_tinker::class_def<CGameMap>(L, "GetMapTemplateID", &CGameMap::GetMapTemplateID);
    lua_tinker::class_def<CGameMap>(L, "GetMapType", &CGameMap::GetMapType);
    lua_tinker::class_def<CGameMap>(L, "GetPatrolData", &CGameMap::GetPatrolData);
    lua_tinker::class_def<CGameMap>(L, "GetPatrolDataByIdx", &CGameMap::GetPatrolDataByIdx);
    lua_tinker::class_def<CGameMap>(L, "GetPhaseData", &CGameMap::GetPhaseData);
    lua_tinker::class_def<CGameMap>(L, "GetPhaseDataById", &CGameMap::GetPhaseDataById);
    lua_tinker::class_def<CGameMap>(L, "GetRebornData", &CGameMap::GetRebornData);
    lua_tinker::class_def<CGameMap>(L, "GetRebornDataByIdx", &CGameMap::GetRebornDataByIdx);
    lua_tinker::class_def<CGameMap>(L, "GetSPRegionIdx", &CGameMap::GetSPRegionIdx);
    lua_tinker::class_def<CGameMap>(L, "GetScriptID", &CGameMap::GetScriptID);
    lua_tinker::class_def<CGameMap>(L, "GetZoneID", &CGameMap::GetZoneID);
    lua_tinker::class_def<CGameMap>(L, "HasMapFlag", &CGameMap::HasMapFlag);
    lua_tinker::class_def<CGameMap>(L, "IsDeadNoDrop", &CGameMap::IsDeadNoDrop);
    lua_tinker::class_def<CGameMap>(L, "IsDropDisable", &CGameMap::IsDropDisable);
    lua_tinker::class_def<CGameMap>(L, "IsDynaMap", &CGameMap::IsDynaMap);
    lua_tinker::class_def<CGameMap>(L, "IsInsideMap", &CGameMap::IsInsideMap);
    lua_tinker::class_def<CGameMap>(L, "IsJumpDisable", &CGameMap::IsJumpDisable);
    lua_tinker::class_def<CGameMap>(L, "IsNearLeavePoint", &CGameMap::IsNearLeavePoint);
    lua_tinker::class_def<CGameMap>(L, "IsNearLeavePointX", &CGameMap::IsNearLeavePointX);
    lua_tinker::class_def<CGameMap>(L, "IsPassDisable", &CGameMap::IsPassDisable);
    lua_tinker::class_def<CGameMap>(L, "IsPlaceDisable", &CGameMap::IsPlaceDisable);
    lua_tinker::class_def<CGameMap>(L, "IsPvPDisable", &CGameMap::IsPvPDisable);
    lua_tinker::class_def<CGameMap>(L, "IsPvPFree", &CGameMap::IsPvPFree);
    lua_tinker::class_def<CGameMap>(L, "IsRecordDisable", &CGameMap::IsRecordDisable);
    lua_tinker::class_def<CGameMap>(L, "IsStallDisable", &CGameMap::IsStallDisable);
    lua_tinker::class_def<CGameMap>(L, "IsZoneMap", &CGameMap::IsZoneMap);
    lua_tinker::class_def<CGameMap>(L, "LineFindCanJump", &CGameMap::LineFindCanJump);
    lua_tinker::class_def<CGameMap>(L, "LineFindCanStand", &CGameMap::LineFindCanStand);
    lua_tinker::class_add<CItem>(L, "CItem", false);
    lua_tinker::class_def<CItem>(L, "ChangeAddition", &CItem::ChangeAddition, true);
    lua_tinker::class_def<CItem>(L, "ChangeType", &CItem::ChangeType, true);
    lua_tinker::class_def<CItem>(L, "ChkEquipPosition", &CItem::ChkEquipPosition);
    lua_tinker::class_def<CItem>(L, "FillItemInfo", &CItem::FillItemInfo);
    lua_tinker::class_def<CItem>(L, "GetCanPileNum", &CItem::GetCanPileNum);
    lua_tinker::class_def<CItem>(L, "GetEquipPosition", &CItem::GetEquipPosition);
    lua_tinker::class_def<CItem>(L, "IsBuyRumor", &CItem::IsBuyRumor);
    lua_tinker::class_def<CItem>(L,
                                 "IsCombineEnable",
                                 lua_tinker::args_type_overload_member_functor(
                                     lua_tinker::make_member_functor_ptr((bool (CItem::*)(CItem*))(&CItem::IsCombineEnable)),
                                     lua_tinker::make_member_functor_ptr((bool (CItem::*)(OBJID, uint32_t))(&CItem::IsCombineEnable))));
    lua_tinker::class_def<CItem>(L, "IsDelEnable", &CItem::IsDelEnable);
    lua_tinker::class_def<CItem>(L, "IsDropEnable", &CItem::IsDropEnable);
    lua_tinker::class_def<CItem>(L, "IsEquipment", &CItem::IsEquipment);
    lua_tinker::class_def<CItem>(L, "IsExchangeEnable", &CItem::IsExchangeEnable);
    lua_tinker::class_def<CItem>(L, "IsExpire", &CItem::IsExpire);
    lua_tinker::class_def<CItem>(L, "IsForgingEnable", &CItem::IsForgingEnable);
    lua_tinker::class_def<CItem>(L, "IsGamblingRumor", &CItem::IsGamblingRumor);
    lua_tinker::class_def<CItem>(L, "IsPickRumor", &CItem::IsPickRumor);
    lua_tinker::class_def<CItem>(L, "IsPileEnable", &CItem::IsPileEnable);
    lua_tinker::class_def<CItem>(L, "IsRepairEnable", &CItem::IsRepairEnable);
    lua_tinker::class_def<CItem>(L, "IsSellEnable", &CItem::IsSellEnable);
    lua_tinker::class_def<CItem>(L, "IsStorageEnable", &CItem::IsStorageEnable);
    lua_tinker::class_def<CItem>(L, "IsSuit", &CItem::IsSuit);
    lua_tinker::class_def<CItem>(L, "IsTraceItem", &CItem::IsTraceItem);
    lua_tinker::class_def<CItem>(L, "SendDeleteMsg", &CItem::SendDeleteMsg);
    lua_tinker::class_def<CItem>(L, "SendItemInfo", &CItem::SendItemInfo);
    lua_tinker::class_def<CItem>(L, "SyncDuraData", &CItem::SyncDuraData);
    lua_tinker::class_def<CItem>(L, "SyncExpireData", &CItem::SyncExpireData);
    lua_tinker::class_def<CItem>(L, "SyncGridData", &CItem::SyncGridData);
    lua_tinker::class_def<CItem>(L, "SyncPileNum", &CItem::SyncPileNum);
    lua_tinker::class_add<CItemData>(L, "CItemData", false);
    lua_tinker::class_def<CItemData>(L, "AddPileNum", &CItemData::AddPileNum, false);
    lua_tinker::class_def<CItemData>(L, "AdditionType", &CItemData::AdditionType);
    lua_tinker::class_def<CItemData>(L, "DecPileNum", &CItemData::DecPileNum, false);
    lua_tinker::class_def<CItemData>(L, "DelRecord", &CItemData::DelRecord);
    lua_tinker::class_def<CItemData>(L, "GetAddition", &CItemData::GetAddition);
    lua_tinker::class_def<CItemData>(L, "GetDura", &CItemData::GetDura);
    lua_tinker::class_def<CItemData>(L, "GetDuraLimit", &CItemData::GetDuraLimit);
    lua_tinker::class_def<CItemData>(L, "GetExpireTime", &CItemData::GetExpireTime);
    lua_tinker::class_def<CItemData>(L, "GetExtra", &CItemData::GetExtra);
    lua_tinker::class_def<CItemData>(L, "GetFlag", &CItemData::GetFlag);
    lua_tinker::class_def<CItemData>(L, "GetGrid", &CItemData::GetGrid);
    lua_tinker::class_def<CItemData>(L, "GetID", &CItemData::GetID);
    lua_tinker::class_def<CItemData>(L, "GetMainType", &CItemData::GetMainType);
    lua_tinker::class_def_static<CItemData>(L, "GetMainTypeByID", &CItemData::GetMainTypeByID);
    lua_tinker::class_def<CItemData>(L, "GetName", &CItemData::GetName);
    lua_tinker::class_def<CItemData>(L, "GetOwnerID", &CItemData::GetOwnerID);
    lua_tinker::class_def<CItemData>(L, "GetPileNum", &CItemData::GetPileNum);
    lua_tinker::class_def<CItemData>(L, "GetPosition", &CItemData::GetPosition);
    lua_tinker::class_def<CItemData>(L, "GetSubType", &CItemData::GetSubType);
    lua_tinker::class_def_static<CItemData>(L, "GetSubTypeByID", &CItemData::GetSubTypeByID);
    lua_tinker::class_def<CItemData>(L, "GetType", &CItemData::GetType);
    lua_tinker::class_def<CItemData>(L, "HasFlag", &CItemData::HasFlag);
    lua_tinker::class_def<CItemData>(L, "ItemTypePtr", &CItemData::ItemTypePtr);
    lua_tinker::class_def<CItemData>(L, "SaveInfo", &CItemData::SaveInfo);
    lua_tinker::class_def<CItemData>(L, "SetAddition", &CItemData::SetAddition, false);
    lua_tinker::class_def<CItemData>(L, "SetDura", &CItemData::SetDura, true);
    lua_tinker::class_def<CItemData>(L, "SetDuraLimit", &CItemData::SetDuraLimit, true);
    lua_tinker::class_def<CItemData>(L, "SetExpireTime", &CItemData::SetExpireTime, false);
    lua_tinker::class_def<CItemData>(L, "SetExtra", &CItemData::SetExtra, false);
    lua_tinker::class_def<CItemData>(L, "SetFlag", &CItemData::SetFlag, false);
    lua_tinker::class_def<CItemData>(L, "SetGrid", &CItemData::SetGrid);
    lua_tinker::class_def<CItemData>(L, "SetOwnerID", &CItemData::SetOwnerID, true);
    lua_tinker::class_def<CItemData>(L, "SetPileNum", &CItemData::SetPileNum, false);
    lua_tinker::class_def<CItemData>(L, "SetPosition", &CItemData::SetPosition, true);
    lua_tinker::class_def<CItemData>(L, "SetTypeID", &CItemData::SetTypeID, true);
    lua_tinker::class_def<CItemData>(L, "_GetFlag", &CItemData::_GetFlag);
    lua_tinker::class_add<CMapData>(L, "CMapData", false);
    lua_tinker::class_def<CMapData>(L, "CanCollision", &CMapData::CanCollision);
    lua_tinker::class_def<CMapData>(L, "GetGirdCount", &CMapData::GetGirdCount);
    lua_tinker::class_def<CMapData>(L, "GetHeight", &CMapData::GetHeight);
    lua_tinker::class_def<CMapData>(L, "GetHeightMap", &CMapData::GetHeightMap);
    lua_tinker::class_def<CMapData>(L, "GetHigh", &CMapData::GetHigh);
    lua_tinker::class_def<CMapData>(L, "GetMapTemplateID", &CMapData::GetMapTemplateID);
    lua_tinker::class_def<CMapData>(L, "GetSPRegionIdx", &CMapData::GetSPRegionIdx);
    lua_tinker::class_def<CMapData>(L, "GetWidth", &CMapData::GetWidth);
    lua_tinker::class_def<CMapData>(L, "GetWidthMap", &CMapData::GetWidthMap);
    lua_tinker::class_def<CMapData>(L, "GirdHeight", &CMapData::GirdHeight);
    lua_tinker::class_def<CMapData>(L, "GirdWidth", &CMapData::GirdWidth);
    lua_tinker::class_def<CMapData>(L, "IsDeadNoDrop", &CMapData::IsDeadNoDrop);
    lua_tinker::class_def<CMapData>(L, "IsDropDisable", &CMapData::IsDropDisable);
    lua_tinker::class_def<CMapData>(L, "IsJumpDisable", &CMapData::IsJumpDisable);
    lua_tinker::class_def<CMapData>(L, "IsPVPDisable", &CMapData::IsPVPDisable);
    lua_tinker::class_def<CMapData>(L, "IsPassDisable", &CMapData::IsPassDisable);
    lua_tinker::class_def<CMapData>(L, "IsPlaceDisable", &CMapData::IsPlaceDisable);
    lua_tinker::class_def<CMapData>(L, "IsPvPFree", &CMapData::IsPvPFree);
    lua_tinker::class_def<CMapData>(L, "IsRecordDisable", &CMapData::IsRecordDisable);
    lua_tinker::class_def<CMapData>(L, "IsStallDisable", &CMapData::IsStallDisable);
    lua_tinker::class_def<CMapData>(L, "LineFindCanJump", &CMapData::LineFindCanJump);
    lua_tinker::class_def<CMapData>(L, "LineFindCanStand", &CMapData::LineFindCanStand);
    lua_tinker::class_def<CMapData>(L, "Pos2Grid", &CMapData::Pos2Grid);
    lua_tinker::class_def<CMapData>(L, "Pos2Idx", &CMapData::Pos2Idx);
    lua_tinker::class_def<CMapData>(L, "_getGridData", &CMapData::_getGridData);
    lua_tinker::class_add<CMapItem>(L, "CMapItem", false);
    lua_tinker::class_def<CMapItem>(L, "GetActorType", &CMapItem::GetActorType);
    lua_tinker::class_def_static<CMapItem>(L, "GetActorTypeStatic", &CMapItem::GetActorTypeStatic);
    lua_tinker::class_def<CMapItem>(L, "GetTypeID", &CMapItem::GetTypeID);
    lua_tinker::class_add<CMapManager>(L, "CMapManager", false);
    lua_tinker::class_def<CMapManager>(L, "ForEach", &CMapManager::ForEach);
    lua_tinker::class_def<CMapManager>(L, "QueryMap", &CMapManager::QueryMap);
    lua_tinker::class_def<CMapManager>(L, "QueryMapData", &CMapManager::QueryMapData);
    lua_tinker::class_add<CMapValSet>(L, "CMapValSet", false);
    lua_tinker::class_def<CMapValSet>(L, "AddMapUserVal", &CMapValSet::AddMapUserVal, false);
    lua_tinker::class_def<CMapValSet>(L, "AddMapVal", &CMapValSet::AddMapVal, false);
    lua_tinker::class_def<CMapValSet>(L, "ClearAllMapUserVal", &CMapValSet::ClearAllMapUserVal);
    lua_tinker::class_def<CMapValSet>(L, "ClearAllMapVal", &CMapValSet::ClearAllMapVal);
    lua_tinker::class_def<CMapValSet>(L, "ClearMapUserVal", &CMapValSet::ClearMapUserVal);
    lua_tinker::class_def<CMapValSet>(L, "ClearMapVal", &CMapValSet::ClearMapVal);
    lua_tinker::class_def<CMapValSet>(L, "GetMapUserVal", &CMapValSet::GetMapUserVal);
    lua_tinker::class_def<CMapValSet>(L, "GetMapVal", &CMapValSet::GetMapVal);
    lua_tinker::class_def<CMapValSet>(L, "SendAllMapValToClient", &CMapValSet::SendAllMapValToClient);
    lua_tinker::class_def<CMapValSet>(L, "SetMapUserVal", &CMapValSet::SetMapUserVal, false);
    lua_tinker::class_def<CMapValSet>(L, "SetMapVal", &CMapValSet::SetMapVal, false);
    lua_tinker::class_def<CMapValSet>(L, "SyncAllMapVal", &CMapValSet::SyncAllMapVal);
    lua_tinker::class_add<CMonster>(L, "CMonster", false);
    lua_tinker::class_def<CMonster>(L, "BeKillBy", &CMonster::BeKillBy);
    lua_tinker::class_def<CMonster>(L, "CanDamage", &CMonster::CanDamage);
    lua_tinker::class_def<CMonster>(L, "GetActorType", &CMonster::GetActorType);
    lua_tinker::class_def_static<CMonster>(L, "GetActorTypeStatic", &CMonster::GetActorTypeStatic);
    lua_tinker::class_def<CMonster>(L, "GetHP", &CMonster::GetHP);
    lua_tinker::class_def<CMonster>(L, "GetLev", &CMonster::GetLev);
    lua_tinker::class_def<CMonster>(L, "GetMP", &CMonster::GetMP);
    lua_tinker::class_def<CMonster>(L, "GetName", &CMonster::GetName);
    lua_tinker::class_def<CMonster>(L, "GetOwnerID", &CMonster::GetOwnerID);
    lua_tinker::class_def<CMonster>(L, "GetTypeID", &CMonster::GetTypeID);
    lua_tinker::class_def<CMonster>(L, "IsBoss", &CMonster::IsBoss);
    lua_tinker::class_def<CMonster>(L, "IsElit", &CMonster::IsElit);
    lua_tinker::class_def<CMonster>(L, "IsEnemy", &CMonster::IsEnemy);
    lua_tinker::class_def<CMonster>(L, "SendMsg", &CMonster::SendMsg);
    lua_tinker::class_def<CMonster>(L, "_SetHP", &CMonster::_SetHP);
    lua_tinker::class_def<CMonster>(L, "_SetMP", &CMonster::_SetMP);
    lua_tinker::class_add<CMyTimer>(L, "CMyTimer", false);
    lua_tinker::class_def<CMyTimer>(L, "Clear", &CMyTimer::Clear);
    lua_tinker::class_def<CMyTimer>(L, "DecInterval", &CMyTimer::DecInterval);
    lua_tinker::class_def<CMyTimer>(L, "GetRemain", &CMyTimer::GetRemain);
    lua_tinker::class_def<CMyTimer>(L, "GetTimeOut", &CMyTimer::GetTimeOut);
    lua_tinker::class_def<CMyTimer>(L, "IncInterval", &CMyTimer::IncInterval);
    lua_tinker::class_def<CMyTimer>(L, "IsActive", &CMyTimer::IsActive);
    lua_tinker::class_def<CMyTimer>(
        L,
        "IsTimeOut",
        lua_tinker::args_type_overload_member_functor(lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)())(&CMyTimer::IsTimeOut)),
                                                      lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)(int32_t))(&CMyTimer::IsTimeOut))));
    lua_tinker::class_def<CMyTimer>(L, "SetInterval", &CMyTimer::SetInterval);
    lua_tinker::class_def<CMyTimer>(L, "Startup", &CMyTimer::Startup);
    lua_tinker::class_def<CMyTimer>(L, "TimeOver", &CMyTimer::TimeOver);
    lua_tinker::class_def<CMyTimer>(L, "ToNextTick", &CMyTimer::ToNextTick);
    lua_tinker::class_def<CMyTimer>(
        L,
        "ToNextTime",
        lua_tinker::args_type_overload_member_functor(lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)())(&CMyTimer::ToNextTime)),
                                                      lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)(int32_t))(&CMyTimer::ToNextTime))));
    lua_tinker::class_def<CMyTimer>(L, "Update", &CMyTimer::Update);
    lua_tinker::class_con<CMyTimer>(L, lua_tinker::constructor<CMyTimer, int32_t>::invoke, 0);
    lua_tinker::class_add<CNetworkMessage>(L, "CNetworkMessage", false);
    lua_tinker::class_def<CNetworkMessage>(L, "AddForward", &CNetworkMessage::AddForward);
    lua_tinker::class_def<CNetworkMessage>(L, "AddMultiIDTo", &CNetworkMessage::AddMultiIDTo);
    lua_tinker::class_def<CNetworkMessage>(L, "AddMultiTo", &CNetworkMessage::AddMultiTo);
    lua_tinker::class_def<CNetworkMessage>(L, "ClearBroadcast", &CNetworkMessage::ClearBroadcast);
    lua_tinker::class_def<CNetworkMessage>(L, "ClearForward", &CNetworkMessage::ClearForward);
    lua_tinker::class_def<CNetworkMessage>(L, "ClearMultiIDTo", &CNetworkMessage::ClearMultiIDTo);
    lua_tinker::class_def<CNetworkMessage>(L, "ClearMultiTo", &CNetworkMessage::ClearMultiTo);
    lua_tinker::class_def<CNetworkMessage>(L, "CopyBuffer", &CNetworkMessage::CopyBuffer);
    lua_tinker::class_def<CNetworkMessage>(L, "CopyRawMessage", &CNetworkMessage::CopyRawMessage);
    lua_tinker::class_def<CNetworkMessage>(L, "GetBodySize", &CNetworkMessage::GetBodySize);
    lua_tinker::class_def<CNetworkMessage>(L, "GetBuf", &CNetworkMessage::GetBuf);
    lua_tinker::class_def<CNetworkMessage>(L, "GetCmd", &CNetworkMessage::GetCmd);
    lua_tinker::class_def<CNetworkMessage>(L, "GetForward", &CNetworkMessage::GetForward);
    lua_tinker::class_def<CNetworkMessage>(L, "GetFrom", &CNetworkMessage::GetFrom);
    lua_tinker::class_def<CNetworkMessage>(
        L,
        "GetMsgBody",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((byte * (CNetworkMessage::*)())(&CNetworkMessage::GetMsgBody)),
            lua_tinker::make_member_functor_ptr((const byte* (CNetworkMessage::*)() const)(&CNetworkMessage::GetMsgBody))));
    lua_tinker::class_def<CNetworkMessage>(
        L,
        "GetMsgHead",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((MSG_HEAD * (CNetworkMessage::*)())(&CNetworkMessage::GetMsgHead)),
            lua_tinker::make_member_functor_ptr((const MSG_HEAD* (CNetworkMessage::*)() const)(&CNetworkMessage::GetMsgHead))));
    lua_tinker::class_def<CNetworkMessage>(L, "GetMultiIDTo", &CNetworkMessage::GetMultiIDTo);
    lua_tinker::class_def<CNetworkMessage>(L, "GetMultiTo", &CNetworkMessage::GetMultiTo);
    lua_tinker::class_def<CNetworkMessage>(L, "GetSize", &CNetworkMessage::GetSize);
    lua_tinker::class_def<CNetworkMessage>(L, "GetTo", &CNetworkMessage::GetTo);
    lua_tinker::class_def<CNetworkMessage>(L, "IsBroadcast", &CNetworkMessage::IsBroadcast);
    lua_tinker::class_def<CNetworkMessage>(L, "IsMultiIDTo", &CNetworkMessage::IsMultiIDTo);
    lua_tinker::class_def<CNetworkMessage>(L, "IsMultiTo", &CNetworkMessage::IsMultiTo);
    lua_tinker::class_def<CNetworkMessage>(L, "PopForward", &CNetworkMessage::PopForward);
    lua_tinker::class_def<CNetworkMessage>(L, "SetBroadcast", &CNetworkMessage::SetBroadcast);
    lua_tinker::class_def<CNetworkMessage>(
        L,
        "SetForward",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (CNetworkMessage::*)(const std::deque<VirtualSocket>&))(&CNetworkMessage::SetForward)),
            lua_tinker::make_member_functor_ptr((void (CNetworkMessage::*)(const uint64_t*, size_t))(&CNetworkMessage::SetForward))));
    lua_tinker::class_def<CNetworkMessage>(L, "SetFrom", &CNetworkMessage::SetFrom);
    lua_tinker::class_def<CNetworkMessage>(
        L,
        "SetMultiIDTo",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (CNetworkMessage::*)(const OBJID*, size_t))(&CNetworkMessage::SetMultiIDTo)),
            lua_tinker::make_member_functor_ptr((void (CNetworkMessage::*)(const std::vector<OBJID>&))(&CNetworkMessage::SetMultiIDTo))));
    lua_tinker::class_def<CNetworkMessage>(
        L,
        "SetMultiTo",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (CNetworkMessage::*)(const std::vector<VirtualSocket>&))(&CNetworkMessage::SetMultiTo)),
            lua_tinker::make_member_functor_ptr((void (CNetworkMessage::*)(const uint64_t*, size_t))(&CNetworkMessage::SetMultiTo))));
    lua_tinker::class_def<CNetworkMessage>(L, "SetTo", &CNetworkMessage::SetTo);
    lua_tinker::class_con<CNetworkMessage>(
        L,
        lua_tinker::args_type_overload_constructor(
            new lua_tinker::constructor<CNetworkMessage, byte*, size_t, VirtualSocket, VirtualSocket>(2 /*default_args_count*/,
                                                                                                      1 /*default_args_start*/),
            new lua_tinker::constructor<CNetworkMessage, const CNetworkMessage&>(),
            new lua_tinker::constructor<CNetworkMessage, uint16_t, byte*, size_t, VirtualSocket, VirtualSocket>(2 /*default_args_count*/,
                                                                                                                3 /*default_args_start*/),
            new lua_tinker::constructor<CNetworkMessage, uint16_t, const proto_msg_t&, VirtualSocket, VirtualSocket>(2 /*default_args_count*/,
                                                                                                                     5 /*default_args_start*/)),
        0,
        0,
        0,
        0,
        0,
        0);
    lua_tinker::class_add<CNpc>(L, "CNpc", false);
    lua_tinker::class_def<CNpc>(L, "ActiveNpc", &CNpc::ActiveNpc);
    lua_tinker::class_def<CNpc>(L, "GetActorType", &CNpc::GetActorType);
    lua_tinker::class_def_static<CNpc>(L, "GetActorTypeStatic", &CNpc::GetActorTypeStatic);
    lua_tinker::class_def<CNpc>(L, "GetHP", &CNpc::GetHP);
    lua_tinker::class_def<CNpc>(L, "GetLev", &CNpc::GetLev);
    lua_tinker::class_def<CNpc>(L, "GetName", &CNpc::GetName);
    lua_tinker::class_def<CNpc>(L, "GetTypeID", &CNpc::GetTypeID);
    lua_tinker::class_def<CNpc>(L, "_ActiveNpc", &CNpc::_ActiveNpc);
    lua_tinker::class_def<CNpc>(L, "_SetHP", &CNpc::_SetHP);
    lua_tinker::class_add<CPackage>(L, "CPackage", false);
    lua_tinker::class_def<CPackage>(L, "AddItem", &CPackage::AddItem, false, true, true);
    lua_tinker::class_def<CPackage>(L, "AwardItem", &CPackage::AwardItem, 0);
    lua_tinker::class_def<CPackage>(L, "ChangeMaxSize", &CPackage::ChangeMaxSize);
    lua_tinker::class_def<CPackage>(
        L,
        "CombineAddItem",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((bool (CPackage::*)(CItem*, bool))(&CPackage::CombineAddItem),
                                                1 /*default_args_count*/,
                                                1 /*default_args_start*/),
            lua_tinker::make_member_functor_ptr((uint32_t(CPackage::*)(uint32_t, uint32_t, uint32_t, bool))(&CPackage::CombineAddItem))),
        true);
    lua_tinker::class_def<CPackage>(L, "CombineItem", &CPackage::CombineItem);
    lua_tinker::class_def<CPackage>(L, "DelAll", &CPackage::DelAll, false, true);
    lua_tinker::class_def<CPackage>(L, "DelItem", &CPackage::DelItem, false, true);
    lua_tinker::class_def<CPackage>(L, "DelItemByGrid", &CPackage::DelItemByGrid, false, true);
    lua_tinker::class_def<CPackage>(L, "DelItemByType", &CPackage::DelItemByType, 0, true);
    lua_tinker::class_def<CPackage>(L, "FindCombineItem", &CPackage::FindCombineItem);
    lua_tinker::class_def<CPackage>(L, "ForEach", &CPackage::ForEach);
    lua_tinker::class_def<CPackage>(L, "GetAmount", &CPackage::GetAmount);
    lua_tinker::class_def<CPackage>(L, "GetItemTypeAmount", &CPackage::GetItemTypeAmount, 0);
    lua_tinker::class_def<CPackage>(L, "GetMaxSize", &CPackage::GetMaxSize);
    lua_tinker::class_def<CPackage>(L, "GetPackageType", &CPackage::GetPackageType);
    lua_tinker::class_def<CPackage>(L, "GetSpareSpace", &CPackage::GetSpareSpace, ID_NONE, 0, 0, 0);
    lua_tinker::class_def<CPackage>(L, "HaveSoManyItem", &CPackage::HaveSoManyItem, 0);
    lua_tinker::class_def<CPackage>(L,
                                    "IsFull",
                                    lua_tinker::args_type_overload_member_functor(
                                        lua_tinker::make_member_functor_ptr((bool (CPackage::*)(CItem*))(&CPackage::IsFull)),
                                        lua_tinker::make_member_functor_ptr((bool (CPackage::*)(uint32_t))(&CPackage::IsFull),
                                                                            1 /*default_args_count*/,
                                                                            1 /*default_args_start*/),
                                        lua_tinker::make_member_functor_ptr((bool (CPackage::*)(uint32_t, uint32_t, uint32_t))(&CPackage::IsFull),
                                                                            1 /*default_args_count*/,
                                                                            2 /*default_args_start*/)),
                                    1,
                                    0);
    lua_tinker::class_def<CPackage>(L, "PopItem", &CPackage::PopItem, false);
    lua_tinker::class_def<CPackage>(L, "PopItemByGrid", &CPackage::PopItemByGrid);
    lua_tinker::class_def<CPackage>(L, "QueryItem", &CPackage::QueryItem);
    lua_tinker::class_def<CPackage>(L, "QueryItemByGrid", &CPackage::QueryItemByGrid);
    lua_tinker::class_def<CPackage>(L, "QueryItemByType", &CPackage::QueryItemByType, 0);
    lua_tinker::class_def<CPackage>(L, "SaveAll", &CPackage::SaveAll);
    lua_tinker::class_def<CPackage>(L, "SendAllItemInfo", &CPackage::SendAllItemInfo);
    lua_tinker::class_def<CPackage>(L, "SplitItem", &CPackage::SplitItem);
    lua_tinker::class_def<CPackage>(L, "SwapItem", &CPackage::SwapItem);
    lua_tinker::class_def<CPackage>(L, "TidyItem", &CPackage::TidyItem);
    lua_tinker::class_add<CPet>(L, "CPet", false);
    lua_tinker::class_def<CPet>(L, "BeKillBy", &CPet::BeKillBy);
    lua_tinker::class_def<CPet>(L, "CallBack", &CPet::CallBack);
    lua_tinker::class_def<CPet>(L, "CallOut", &CPet::CallOut);
    lua_tinker::class_def<CPet>(L, "CanDamage", &CPet::CanDamage);
    lua_tinker::class_def<CPet>(L, "GetActorType", &CPet::GetActorType);
    lua_tinker::class_def_static<CPet>(L, "GetActorTypeStatic", &CPet::GetActorTypeStatic);
    lua_tinker::class_def<CPet>(L, "GetFP", &CPet::GetFP);
    lua_tinker::class_def<CPet>(L, "GetHP", &CPet::GetHP);
    lua_tinker::class_def<CPet>(L, "GetMP", &CPet::GetMP);
    lua_tinker::class_def<CPet>(L, "GetNP", &CPet::GetNP);
    lua_tinker::class_def<CPet>(L, "GetOwnerID", &CPet::GetOwnerID);
    lua_tinker::class_def<CPet>(L, "GetPetTypeID", &CPet::GetPetTypeID);
    lua_tinker::class_def<CPet>(L, "IsEnemy", &CPet::IsEnemy);
    lua_tinker::class_def<CPet>(L, "_SetFP", &CPet::_SetFP);
    lua_tinker::class_def<CPet>(L, "_SetHP", &CPet::_SetHP);
    lua_tinker::class_def<CPet>(L, "_SetMP", &CPet::_SetMP);
    lua_tinker::class_def<CPet>(L, "_SetNP", &CPet::_SetNP);
    lua_tinker::class_add<CPhase>(L, "CPhase", false);
    lua_tinker::class_def<CPhase>(L, "AddDynaRegion", &CPhase::AddDynaRegion);
    lua_tinker::class_def<CPhase>(L, "AddTimedCallback", &CPhase::AddTimedCallback);
    lua_tinker::class_def<CPhase>(L, "CanDestory", &CPhase::CanDestory);
    lua_tinker::class_def<CPhase>(L, "ClearAllCllback", &CPhase::ClearAllCllback);
    lua_tinker::class_def<CPhase>(L, "ClearDynaRegion", &CPhase::ClearDynaRegion);
    lua_tinker::class_def<CPhase>(L, "CreateMonster", &CPhase::CreateMonster);
    lua_tinker::class_def<CPhase>(L, "CreateMultiMonster", &CPhase::CreateMultiMonster);
    lua_tinker::class_def<CPhase>(L, "CreateNpc", &CPhase::CreateNpc);
    lua_tinker::class_def<CPhase>(L, "GetMapValSet", &CPhase::GetMapValSet);
    lua_tinker::class_def<CPhase>(L, "GetPhaseID", &CPhase::GetPhaseID);
    lua_tinker::class_def<CPhase>(L, "GetPhaseIdx", &CPhase::GetPhaseIdx);
    lua_tinker::class_def<CPhase>(L, "GetScene", &CPhase::GetScene);
    lua_tinker::class_def<CPhase>(L, "GetSceneState", &CPhase::GetSceneState);
    lua_tinker::class_def<CPhase>(L, "KickAllPlayer", &CPhase::KickAllPlayer, "");
    lua_tinker::class_def<CPhase>(L, "NeedDestory", &CPhase::NeedDestory);
    lua_tinker::class_def<CPhase>(L, "SendSceneMessage", &CPhase::SendSceneMessage);
    lua_tinker::class_def<CPhase>(L, "SetSceneState", &CPhase::SetSceneState);
    lua_tinker::class_def<CPhase>(L, "_KickPlayer", &CPhase::_KickPlayer);
    lua_tinker::class_add<CPlayer>(L, "CPlayer", false);
    lua_tinker::class_def<CPlayer>(L, "ActiveNpc", &CPlayer::ActiveNpc);
    lua_tinker::class_def<CPlayer>(L, "AwardBattleExp", &CPlayer::AwardBattleExp);
    lua_tinker::class_def<CPlayer>(L, "AwardExp", &CPlayer::AwardExp);
    lua_tinker::class_def<CPlayer>(L, "AwardItem", &CPlayer::AwardItem);
    lua_tinker::class_def<CPlayer>(L, "AwardMeony", &CPlayer::AwardMeony);
    lua_tinker::class_def<CPlayer>(L, "BeKillBy", &CPlayer::BeKillBy);
    lua_tinker::class_def<CPlayer>(L, "BroadcastShow", &CPlayer::BroadcastShow);
    lua_tinker::class_def<CPlayer>(L, "CanDamage", &CPlayer::CanDamage);
    lua_tinker::class_def<CPlayer>(L, "CheckItem", &CPlayer::CheckItem, 1, 0);
    lua_tinker::class_def<CPlayer>(L, "CheckMoney", &CPlayer::CheckMoney);
    lua_tinker::class_def<CPlayer>(L, "DialogAddLink", &CPlayer::DialogAddLink, 0);
    lua_tinker::class_def<CPlayer>(L, "DialogAddText", &CPlayer::DialogAddText);
    lua_tinker::class_def<CPlayer>(L, "DialogBegin", &CPlayer::DialogBegin);
    lua_tinker::class_def<CPlayer>(L, "DialogSend", &CPlayer::DialogSend, 0);
    lua_tinker::class_def<CPlayer>(L, "FlyMap", &CPlayer::FlyMap);
    lua_tinker::class_def<CPlayer>(L, "GetAchiPoint", &CPlayer::GetAchiPoint);
    lua_tinker::class_def<CPlayer>(L, "GetAchievement", &CPlayer::GetAchievement);
    lua_tinker::class_def<CPlayer>(L, "GetActorType", &CPlayer::GetActorType);
    lua_tinker::class_def_static<CPlayer>(L, "GetActorTypeStatic", &CPlayer::GetActorTypeStatic);
    lua_tinker::class_def<CPlayer>(L, "GetBag", &CPlayer::GetBag);
    lua_tinker::class_def<CPlayer>(L, "GetBagMaxSize", &CPlayer::GetBagMaxSize);
    lua_tinker::class_def<CPlayer>(L, "GetBaseLook", &CPlayer::GetBaseLook);
    lua_tinker::class_def<CPlayer>(L, "GetCommonDataSet", &CPlayer::GetCommonDataSet);
    lua_tinker::class_def<CPlayer>(L, "GetDataCountSet", &CPlayer::GetDataCountSet);
    lua_tinker::class_def<CPlayer>(L, "GetEquipmentSet", &CPlayer::GetEquipmentSet);
    lua_tinker::class_def<CPlayer>(L, "GetExp", &CPlayer::GetExp);
    lua_tinker::class_def<CPlayer>(L, "GetFP", &CPlayer::GetFP);
    lua_tinker::class_def<CPlayer>(L, "GetFPMax", &CPlayer::GetFPMax);
    lua_tinker::class_def<CPlayer>(L, "GetGold", &CPlayer::GetGold);
    lua_tinker::class_def<CPlayer>(L, "GetGoldBind", &CPlayer::GetGoldBind);
    lua_tinker::class_def<CPlayer>(L, "GetGuildID", &CPlayer::GetGuildID);
    lua_tinker::class_def<CPlayer>(L, "GetGuildLev", &CPlayer::GetGuildLev);
    lua_tinker::class_def<CPlayer>(L, "GetHP", &CPlayer::GetHP);
    lua_tinker::class_def<CPlayer>(L, "GetHPMax", &CPlayer::GetHPMax);
    lua_tinker::class_def<CPlayer>(L, "GetHeight", &CPlayer::GetHeight);
    lua_tinker::class_def<CPlayer>(L, "GetHomeFace", &CPlayer::GetHomeFace);
    lua_tinker::class_def<CPlayer>(L, "GetHomePosX", &CPlayer::GetHomePosX);
    lua_tinker::class_def<CPlayer>(L, "GetHomePosY", &CPlayer::GetHomePosY);
    lua_tinker::class_def<CPlayer>(L, "GetHomeSceneIdx", &CPlayer::GetHomeSceneIdx);
    lua_tinker::class_def<CPlayer>(L, "GetHonor", &CPlayer::GetHonor);
    lua_tinker::class_def<CPlayer>(L, "GetLev", &CPlayer::GetLev);
    lua_tinker::class_def<CPlayer>(L, "GetMP", &CPlayer::GetMP);
    lua_tinker::class_def<CPlayer>(L, "GetMPMax", &CPlayer::GetMPMax);
    lua_tinker::class_def<CPlayer>(L, "GetMoney", &CPlayer::GetMoney);
    lua_tinker::class_def<CPlayer>(L, "GetMoneyBind", &CPlayer::GetMoneyBind);
    lua_tinker::class_def<CPlayer>(L, "GetNP", &CPlayer::GetNP);
    lua_tinker::class_def<CPlayer>(L, "GetNPMax", &CPlayer::GetNPMax);
    lua_tinker::class_def<CPlayer>(L, "GetName", &CPlayer::GetName);
    lua_tinker::class_def<CPlayer>(L, "GetOpenID", &CPlayer::GetOpenID);
    lua_tinker::class_def<CPlayer>(L, "GetPKMode", &CPlayer::GetPKMode);
    lua_tinker::class_def<CPlayer>(L, "GetPKVal", &CPlayer::GetPKVal);
    lua_tinker::class_def<CPlayer>(L, "GetPetSet", &CPlayer::GetPetSet);
    lua_tinker::class_def<CPlayer>(L, "GetProf", &CPlayer::GetProf);
    lua_tinker::class_def<CPlayer>(L, "GetProperty", &CPlayer::GetProperty);
    lua_tinker::class_def<CPlayer>(L, "GetPropertyMax", &CPlayer::GetPropertyMax);
    lua_tinker::class_def<CPlayer>(L, "GetRecordFace", &CPlayer::GetRecordFace);
    lua_tinker::class_def<CPlayer>(L, "GetRecordPosX", &CPlayer::GetRecordPosX);
    lua_tinker::class_def<CPlayer>(L, "GetRecordPosY", &CPlayer::GetRecordPosY);
    lua_tinker::class_def<CPlayer>(L, "GetRecordSceneIdx", &CPlayer::GetRecordSceneIdx);
    lua_tinker::class_def<CPlayer>(L, "GetSkillManager", &CPlayer::GetSkillManager);
    lua_tinker::class_def<CPlayer>(L, "GetSocket", &CPlayer::GetSocket);
    lua_tinker::class_def<CPlayer>(L, "GetStroagePackage", &CPlayer::GetStroagePackage);
    lua_tinker::class_def<CPlayer>(L, "GetStrogeMaxSize", &CPlayer::GetStrogeMaxSize);
    lua_tinker::class_def<CPlayer>(L, "GetTaskSet", &CPlayer::GetTaskSet);
    lua_tinker::class_def<CPlayer>(L, "GetTeamID", &CPlayer::GetTeamID);
    lua_tinker::class_def<CPlayer>(L, "GetTeamMemberCount", &CPlayer::GetTeamMemberCount);
    lua_tinker::class_def<CPlayer>(L, "GetVipLev", &CPlayer::GetVipLev);
    lua_tinker::class_def<CPlayer>(L, "GetVolume", &CPlayer::GetVolume);
    lua_tinker::class_def<CPlayer>(L, "GetWorldID", &CPlayer::GetWorldID);
    lua_tinker::class_def<CPlayer>(L, "HasTeam", &CPlayer::HasTeam);
    lua_tinker::class_def<CPlayer>(L, "IsEnemy", &CPlayer::IsEnemy);
    lua_tinker::class_def<CPlayer>(L, "IsTalkEnable", &CPlayer::IsTalkEnable);
    lua_tinker::class_def<CPlayer>(L, "QueryPackage", &CPlayer::QueryPackage);
    lua_tinker::class_def<CPlayer>(L, "Reborn", &CPlayer::Reborn);
    lua_tinker::class_def<CPlayer>(L, "RecalcAttrib", &CPlayer::RecalcAttrib, false);
    lua_tinker::class_def<CPlayer>(L, "SendMsg", &CPlayer::SendMsg);
    lua_tinker::class_def<CPlayer>(L, "SendTalkMsg", &CPlayer::SendTalkMsg);
    lua_tinker::class_def<CPlayer>(L, "SetGuildID", &CPlayer::SetGuildID);
    lua_tinker::class_def<CPlayer>(L, "SetPKMode", &CPlayer::SetPKMode);
    lua_tinker::class_def<CPlayer>(L, "SetTeamID", &CPlayer::SetTeamID);
    lua_tinker::class_def<CPlayer>(L, "SpendItem", &CPlayer::SpendItem, 1, true);
    lua_tinker::class_def<CPlayer>(L, "SpendMoney", &CPlayer::SpendMoney);
    lua_tinker::class_def<CPlayer>(L, "UseItem", &CPlayer::UseItem, 1);
    lua_tinker::class_def<CPlayer>(L, "_SetFP", &CPlayer::_SetFP);
    lua_tinker::class_def<CPlayer>(L, "_SetHP", &CPlayer::_SetHP);
    lua_tinker::class_def<CPlayer>(L, "_SetMP", &CPlayer::_SetMP);
    lua_tinker::class_def<CPlayer>(L, "_SetNP", &CPlayer::_SetNP);
    lua_tinker::class_def<CPlayer>(L, "_SetProperty", &CPlayer::_SetProperty, SYNC_TRUE);
    lua_tinker::class_add<CPlayerAchievement>(L, "CPlayerAchievement", false);
    lua_tinker::class_def<CPlayerAchievement>(L, "CheckAchiCondition", &CPlayerAchievement::CheckAchiCondition, 0, 0);
    lua_tinker::class_def<CPlayerAchievement>(L, "SyncAll", &CPlayerAchievement::SyncAll);
    lua_tinker::class_def<CPlayerAchievement>(L, "TakeAll", &CPlayerAchievement::TakeAll);
    lua_tinker::class_def<CPlayerAchievement>(L, "TakeReward", &CPlayerAchievement::TakeReward);
    lua_tinker::class_add<CPlayerCoolDown>(L, "CPlayerCoolDown", false);
    lua_tinker::class_add<CPlayerCoolDownSet>(L, "CPlayerCoolDownSet", false);
    lua_tinker::class_add<CPlayerSkillManager>(L, "CPlayerSkillManager", false);
    lua_tinker::class_def<CPlayerSkillManager>(L, "CastSkill", &CPlayerSkillManager::CastSkill);
    lua_tinker::class_def<CPlayerSkillManager>(L, "LearnSkill", &CPlayerSkillManager::LearnSkill);
    lua_tinker::class_def<CPlayerSkillManager>(L, "UpgradeSkill", &CPlayerSkillManager::UpgradeSkill);
    lua_tinker::class_def<CPlayerSkillManager>(L, "_QuerySkill", &CPlayerSkillManager::_QuerySkill);
    lua_tinker::class_add<CPlayerTask>(L, "CPlayerTask", false);
    lua_tinker::class_def<CPlayerTask>(L, "AcceptTask", &CPlayerTask::AcceptTask, true, false);
    lua_tinker::class_def<CPlayerTask>(L,
                                       "CanAccept",
                                       lua_tinker::args_type_overload_member_functor(
                                           lua_tinker::make_member_functor_ptr((bool (CPlayerTask::*)(const CTaskType*))(&CPlayerTask::CanAccept)),
                                           lua_tinker::make_member_functor_ptr((bool (CPlayerTask::*)(uint32_t))(&CPlayerTask::CanAccept))));
    lua_tinker::class_def<CPlayerTask>(L,
                                       "CanSubmit",
                                       lua_tinker::args_type_overload_member_functor(
                                           lua_tinker::make_member_functor_ptr((bool (CPlayerTask::*)(const CTaskType*))(&CPlayerTask::CanSubmit)),
                                           lua_tinker::make_member_functor_ptr((bool (CPlayerTask::*)(uint32_t))(&CPlayerTask::CanSubmit))));
    lua_tinker::class_def<CPlayerTask>(L, "GetLeftTimes", &CPlayerTask::GetLeftTimes);
    lua_tinker::class_def<CPlayerTask>(L, "GiveupTask", &CPlayerTask::GiveupTask);
    lua_tinker::class_def<CPlayerTask>(L, "IsFinished", &CPlayerTask::IsFinished);
    lua_tinker::class_def<CPlayerTask>(L, "QueryTaskData", &CPlayerTask::QueryTaskData);
    lua_tinker::class_def<CPlayerTask>(L, "QuickFinish", &CPlayerTask::QuickFinish);
    lua_tinker::class_def<CPlayerTask>(L, "SaveInfo", &CPlayerTask::SaveInfo);
    lua_tinker::class_def<CPlayerTask>(L, "SendTaskDataChange", &CPlayerTask::SendTaskDataChange);
    lua_tinker::class_def<CPlayerTask>(
        L,
        "SendTaskInfo",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (CPlayerTask::*)())(&CPlayerTask::SendTaskInfo)),
            lua_tinker::make_member_functor_ptr((void (CPlayerTask::*)(CPlayerTaskData*))(&CPlayerTask::SendTaskInfo))));
    lua_tinker::class_def<CPlayerTask>(L, "ShowTaskDialog", &CPlayerTask::ShowTaskDialog);
    lua_tinker::class_def<CPlayerTask>(L, "SubmitTask", &CPlayerTask::SubmitTask);
    lua_tinker::class_def<CPlayerTask>(L, "SubmitTaskByMessage", &CPlayerTask::SubmitTaskByMessage);
    lua_tinker::class_add<CPlayerTaskData>(L, "CPlayerTaskData", false);
    lua_tinker::class_def<CPlayerTaskData>(L, "GetAcceptTime", &CPlayerTaskData::GetAcceptTime);
    lua_tinker::class_def<CPlayerTaskData>(L, "GetAcceptUserLev", &CPlayerTaskData::GetAcceptUserLev);
    lua_tinker::class_def<CPlayerTaskData>(L, "GetDayCount", &CPlayerTaskData::GetDayCount);
    lua_tinker::class_def<CPlayerTaskData>(L, "GetDayCountMax", &CPlayerTaskData::GetDayCountMax);
    lua_tinker::class_def<CPlayerTaskData>(L, "GetExpireTime", &CPlayerTaskData::GetExpireTime);
    lua_tinker::class_def<CPlayerTaskData>(L, "GetFinishTime", &CPlayerTaskData::GetFinishTime);
    lua_tinker::class_def<CPlayerTaskData>(L, "GetNum", &CPlayerTaskData::GetNum);
    lua_tinker::class_def<CPlayerTaskData>(L, "GetState", &CPlayerTaskData::GetState);
    lua_tinker::class_def<CPlayerTaskData>(L, "GetTaskID", &CPlayerTaskData::GetTaskID);
    lua_tinker::class_def<CPlayerTaskData>(L, "IsExpire", &CPlayerTaskData::IsExpire);
    lua_tinker::class_def<CPlayerTaskData>(L, "IsTaskDoing", &CPlayerTaskData::IsTaskDoing);
    lua_tinker::class_def<CPlayerTaskData>(L, "SaveInfo", &CPlayerTaskData::SaveInfo);
    lua_tinker::class_def<CPlayerTaskData>(L, "SetAcceptTime", &CPlayerTaskData::SetAcceptTime, true);
    lua_tinker::class_def<CPlayerTaskData>(L, "SetAcceptUserLev", &CPlayerTaskData::SetAcceptUserLev, true);
    lua_tinker::class_def<CPlayerTaskData>(L, "SetDayCount", &CPlayerTaskData::SetDayCount, true);
    lua_tinker::class_def<CPlayerTaskData>(L, "SetDayCountMax", &CPlayerTaskData::SetDayCountMax, true);
    lua_tinker::class_def<CPlayerTaskData>(L, "SetExpireTime", &CPlayerTaskData::SetExpireTime, true);
    lua_tinker::class_def<CPlayerTaskData>(L, "SetFinishTime", &CPlayerTaskData::SetFinishTime, true);
    lua_tinker::class_def<CPlayerTaskData>(L, "SetNum", &CPlayerTaskData::SetNum, true);
    lua_tinker::class_def<CPlayerTaskData>(L, "SetState", &CPlayerTaskData::SetState, true);
    lua_tinker::class_add<CScene>(L, "CScene", false);
    lua_tinker::class_def<CScene>(L,
                                  "CreatePhase",
                                  lua_tinker::args_type_overload_member_functor(
                                      lua_tinker::make_member_functor_ptr((CPhase * (CScene::*)(uint64_t))(&CScene::CreatePhase)),
                                      lua_tinker::make_member_functor_ptr((CPhase * (CScene::*)(uint64_t, const PhaseData*))(&CScene::CreatePhase))));
    lua_tinker::class_def<CScene>(L, "DestoryPhase", &CScene::DestoryPhase);
    lua_tinker::class_def<CScene>(L, "ForEach", &CScene::ForEach);
    lua_tinker::class_def<CScene>(L, "GetMapID", &CScene::GetMapID);
    lua_tinker::class_def<CScene>(L, "GetStaticPhaseCount", &CScene::GetStaticPhaseCount);
    lua_tinker::class_def<CScene>(L, "QueryPhase", &CScene::QueryPhase);
    lua_tinker::class_def<CScene>(L, "QueryPhaseByIdx", &CScene::QueryPhaseByIdx);
    lua_tinker::class_def<CScene>(L, "_QueryPhase", &CScene::_QueryPhase);
    lua_tinker::class_add<CSceneManager>(L, "CSceneManager", false);
    lua_tinker::class_def<CSceneManager>(L, "CreatePhase", &CSceneManager::CreatePhase);
    lua_tinker::class_def<CSceneManager>(L, "ForEach", &CSceneManager::ForEach);
    lua_tinker::class_def<CSceneManager>(L, "GetDynaSceneCount", &CSceneManager::GetDynaSceneCount);
    lua_tinker::class_def<CSceneManager>(L, "GetSceneCount", &CSceneManager::GetSceneCount);
    lua_tinker::class_def<CSceneManager>(L, "QueryPhase", &CSceneManager::QueryPhase);
    lua_tinker::class_def<CSceneManager>(L, "QueryScene", &CSceneManager::QueryScene);
    lua_tinker::class_def<CSceneManager>(L, "_CreateStaticScene", &CSceneManager::_CreateStaticScene);
    lua_tinker::class_add<CSceneService>(L, "CSceneService", false);
    lua_tinker::class_def<CSceneService>(L, "CreateUID", &CSceneService::CreateUID);
    lua_tinker::class_def<CSceneService>(L, "GetAIServerPort", &CSceneService::GetAIServerPort);
    lua_tinker::class_def<CSceneService>(L, "GetActorManager", &CSceneService::GetActorManager);
    lua_tinker::class_def<CSceneService>(L, "GetGMManager", &CSceneService::GetGMManager);
    lua_tinker::class_def<CSceneService>(L, "GetGameDB", &CSceneService::GetGameDB);
    lua_tinker::class_def<CSceneService>(L, "GetLoadingThread", &CSceneService::GetLoadingThread);
    lua_tinker::class_def<CSceneService>(L, "GetMapManager", &CSceneService::GetMapManager);
    lua_tinker::class_def<CSceneService>(L, "GetSceneManager", &CSceneService::GetSceneManager);
    lua_tinker::class_def<CSceneService>(L, "GetScriptManager", &CSceneService::GetScriptManager);
    lua_tinker::class_def<CSceneService>(L, "GetServiceName", &CSceneService::GetServiceName);
    lua_tinker::class_def<CSceneService>(L, "GetSystemVarSet", &CSceneService::GetSystemVarSet);
    lua_tinker::class_def<CSceneService>(L, "GetTeamInfoManager", &CSceneService::GetTeamInfoManager);
    lua_tinker::class_def<CSceneService>(L, "GetZoneID", &CSceneService::GetZoneID);
    lua_tinker::class_def<CSceneService>(L, "IsSharedZone", &CSceneService::IsSharedZone);
    lua_tinker::class_def<CSceneService>(L, "SendProtoMsgToAIService", &CSceneService::SendProtoMsgToAIService);
    lua_tinker::class_def<CSceneService>(L, "SendProtoMsgToAllPlayer", &CSceneService::SendProtoMsgToAllPlayer);
    lua_tinker::class_def<CSceneService>(L, "SendProtoMsgToAllScene", &CSceneService::SendProtoMsgToAllScene);
    lua_tinker::class_def<CSceneService>(L, "SendProtoMsgToPlayer", &CSceneService::SendProtoMsgToPlayer);
    lua_tinker::class_def<CSceneService>(L, "SendProtoMsgToWorld", &CSceneService::SendProtoMsgToWorld);
    lua_tinker::class_add<CServiceCommon>(L, "CServiceCommon", false);
    lua_tinker::class_def<CServiceCommon>(L, "GetEventManager", &CServiceCommon::GetEventManager);
    lua_tinker::class_def<CServiceCommon>(L, "GetMonitorMgr", &CServiceCommon::GetMonitorMgr);
    lua_tinker::class_def<CServiceCommon>(L, "GetNetMsgProcess", &CServiceCommon::GetNetMsgProcess);
    lua_tinker::class_def<CServiceCommon>(L, "GetNetworkService", &CServiceCommon::GetNetworkService);
    lua_tinker::class_def<CServiceCommon>(L, "GetServerPort", &CServiceCommon::GetServerPort);
    lua_tinker::class_def<CServiceCommon>(L, "GetServerVirtualSocket", &CServiceCommon::GetServerVirtualSocket);
    lua_tinker::class_def<CServiceCommon>(L, "GetServiceID", &CServiceCommon::GetServiceID);
    lua_tinker::class_def<CServiceCommon>(L, "GetServiceName", &CServiceCommon::GetServiceName);
    lua_tinker::class_def<CServiceCommon>(L, "GetWorldID", &CServiceCommon::GetWorldID);
    lua_tinker::class_def<CServiceCommon>(L, "SetServerPort", &CServiceCommon::SetServerPort);
    lua_tinker::class_add<CSkillData>(L, "CSkillData", false);
    lua_tinker::class_def<CSkillData>(L, "GetSkillLev", &CSkillData::GetSkillLev);
    lua_tinker::class_def<CSkillData>(L, "GetSkillSort", &CSkillData::GetSkillSort);
    lua_tinker::class_def<CSkillData>(L, "SetSkillLev", &CSkillData::SetSkillLev, true);
    lua_tinker::class_def<CSkillData>(L, "Update", &CSkillData::Update);
    lua_tinker::class_add<CSkillFSM>(L, "CSkillFSM", false);
    lua_tinker::class_def_static<CSkillFSM>(L, "AddBullet", &CSkillFSM::AddBullet);
    lua_tinker::class_def_static<CSkillFSM>(L, "AttachStatus", &CSkillFSM::AttachStatus);
    lua_tinker::class_def<CSkillFSM>(L, "BreakIntone", &CSkillFSM::BreakIntone);
    lua_tinker::class_def<CSkillFSM>(L, "BreakLaunch", &CSkillFSM::BreakLaunch);
    lua_tinker::class_def<CSkillFSM>(L, "CanIntone", &CSkillFSM::CanIntone);
    lua_tinker::class_def<CSkillFSM>(L, "CastSkill", &CSkillFSM::CastSkill);
    lua_tinker::class_def<CSkillFSM>(L, "ClearSkillCoolDown", &CSkillFSM::ClearSkillCoolDown);
    lua_tinker::class_def_static<CSkillFSM>(L, "DoDamage", &CSkillFSM::DoDamage);
    lua_tinker::class_def<CSkillFSM>(L, "DoIntone", &CSkillFSM::DoIntone);
    lua_tinker::class_def_static<CSkillFSM>(L, "DoMultiDamage", &CSkillFSM::DoMultiDamage);
    lua_tinker::class_def_static<CSkillFSM>(L, "FindTarget", &CSkillFSM::FindTarget);
    lua_tinker::class_def<CSkillFSM>(L, "IsSkillCoolDown", &CSkillFSM::IsSkillCoolDown);
    lua_tinker::class_def<CSkillFSM>(L, "SetTargetPos", &CSkillFSM::SetTargetPos);
    lua_tinker::class_def_static<CSkillFSM>(L, "SkillEffectInRange", &CSkillFSM::SkillEffectInRange);
    lua_tinker::class_def<CSkillFSM>(L, "StartSkillCoolDown", &CSkillFSM::StartSkillCoolDown);
    lua_tinker::class_def<CSkillFSM>(L, "_BreakIntone", &CSkillFSM::_BreakIntone);
    lua_tinker::class_def<CSkillFSM>(L, "_BreakLaunch", &CSkillFSM::_BreakLaunch);
    lua_tinker::class_add<CStatus>(L, "CStatus", false);
    lua_tinker::class_def<CStatus>(L, "AddSecs", &CStatus::AddSecs);
    lua_tinker::class_def<CStatus>(L, "AddTimes", &CStatus::AddTimes);
    lua_tinker::class_def<CStatus>(L, "CancelEvent", &CStatus::CancelEvent);
    lua_tinker::class_def<CStatus>(L, "ChangeData", &CStatus::ChangeData);
    lua_tinker::class_def<CStatus>(L, "ClearEvent", &CStatus::ClearEvent);
    lua_tinker::class_def<CStatus>(L, "GetCasterID", &CStatus::GetCasterID);
    lua_tinker::class_def<CStatus>(L, "GetFlag", &CStatus::GetFlag);
    lua_tinker::class_def<CStatus>(L, "GetID", &CStatus::GetID);
    lua_tinker::class_def<CStatus>(L, "GetLastTimeStamp", &CStatus::GetLastTimeStamp);
    lua_tinker::class_def<CStatus>(L, "GetLevel", &CStatus::GetLevel);
    lua_tinker::class_def<CStatus>(L, "GetOwner", &CStatus::GetOwner);
    lua_tinker::class_def<CStatus>(L, "GetPower", &CStatus::GetPower);
    lua_tinker::class_def<CStatus>(L, "GetRemainTime", &CStatus::GetRemainTime);
    lua_tinker::class_def<CStatus>(L, "GetSecs", &CStatus::GetSecs);
    lua_tinker::class_def<CStatus>(L, "GetTimes", &CStatus::GetTimes);
    lua_tinker::class_def<CStatus>(L, "GetTypeID", &CStatus::GetTypeID);
    lua_tinker::class_def<CStatus>(L, "IsPaused", &CStatus::IsPaused);
    lua_tinker::class_def<CStatus>(L, "IsValid", &CStatus::IsValid);
    lua_tinker::class_def<CStatus>(L, "OnAttach", &CStatus::OnAttach);
    lua_tinker::class_def<CStatus>(L, "OnAttack", &CStatus::OnAttack);
    lua_tinker::class_def<CStatus>(L, "OnBeAttack", &CStatus::OnBeAttack);
    lua_tinker::class_def<CStatus>(L, "OnDead", &CStatus::OnDead);
    lua_tinker::class_def<CStatus>(L, "OnDeatch", &CStatus::OnDeatch);
    lua_tinker::class_def<CStatus>(L, "OnLeaveMap", &CStatus::OnLeaveMap);
    lua_tinker::class_def<CStatus>(L, "OnMove", &CStatus::OnMove);
    lua_tinker::class_def<CStatus>(L, "OnSkill", &CStatus::OnSkill);
    lua_tinker::class_def<CStatus>(L, "Pause", &CStatus::Pause, true);
    lua_tinker::class_def<CStatus>(L, "ProcessEvent", &CStatus::ProcessEvent);
    lua_tinker::class_def<CStatus>(L, "Resume", &CStatus::Resume, true);
    lua_tinker::class_def<CStatus>(L, "SaveInfo", &CStatus::SaveInfo);
    lua_tinker::class_def<CStatus>(L, "ScheduleEvent", &CStatus::ScheduleEvent, 0);
    lua_tinker::class_def<CStatus>(L, "SendStatus", &CStatus::SendStatus);
    lua_tinker::class_def<CStatus>(L, "SetPower", &CStatus::SetPower);
    lua_tinker::class_def<CStatus>(L, "Type", &CStatus::Type);
    lua_tinker::class_add<CStatusType>(L, "CStatusType", false);
    lua_tinker::class_def<CStatusType>(L, "CloneInfo", &CStatusType::CloneInfo);
    lua_tinker::class_def<CStatusType>(L, "GetAttribChangeList", &CStatusType::GetAttribChangeList);
    lua_tinker::class_def<CStatusType>(L, "GetExpireType", &CStatusType::GetExpireType);
    lua_tinker::class_def<CStatusType>(L, "GetFlag", &CStatusType::GetFlag);
    lua_tinker::class_def<CStatusType>(L, "GetID", &CStatusType::GetID);
    lua_tinker::class_def<CStatusType>(L, "GetLevel", &CStatusType::GetLevel);
    lua_tinker::class_def<CStatusType>(L, "GetMaxSecs", &CStatusType::GetMaxSecs);
    lua_tinker::class_def<CStatusType>(L, "GetMaxTimes", &CStatusType::GetMaxTimes);
    lua_tinker::class_def<CStatusType>(L, "GetPower", &CStatusType::GetPower);
    lua_tinker::class_def<CStatusType>(L, "GetScirptID", &CStatusType::GetScirptID);
    lua_tinker::class_def<CStatusType>(L, "GetSecs", &CStatusType::GetSecs);
    lua_tinker::class_def<CStatusType>(L, "GetTimes", &CStatusType::GetTimes);
    lua_tinker::class_def<CStatusType>(L, "GetTypeID", &CStatusType::GetTypeID);
    lua_tinker::class_add<CStoragePackage>(L, "CStoragePackage", false);
    lua_tinker::class_def<CStoragePackage>(L, "CheckIn", &CStoragePackage::CheckIn);
    lua_tinker::class_def<CStoragePackage>(L, "CheckOut", &CStoragePackage::CheckOut);
    lua_tinker::class_add<CSystemVar>(L, "CSystemVar", false);
    lua_tinker::class_def<CSystemVar>(L, "AddData", &CSystemVar::AddData, false, false);
    lua_tinker::class_def<CSystemVar>(L, "Broadcast", &CSystemVar::Broadcast);
    lua_tinker::class_def<CSystemVar>(L, "ClearDirty", &CSystemVar::ClearDirty);
    lua_tinker::class_def<CSystemVar>(L, "DeleteRecord", &CSystemVar::DeleteRecord);
    lua_tinker::class_def<CSystemVar>(L, "GetData", &CSystemVar::GetData);
    lua_tinker::class_def<CSystemVar>(L, "GetIdx", &CSystemVar::GetIdx);
    lua_tinker::class_def<CSystemVar>(L, "GetStr", &CSystemVar::GetStr);
    lua_tinker::class_def<CSystemVar>(L, "Save", &CSystemVar::Save);
    lua_tinker::class_def<CSystemVar>(L, "SendToPlayer", &CSystemVar::SendToPlayer);
    lua_tinker::class_def<CSystemVar>(L, "SetData", &CSystemVar::SetData, false, false);
    lua_tinker::class_def<CSystemVar>(L, "SetStr", &CSystemVar::SetStr, false, false);
    lua_tinker::class_add<CSystemVarSet>(L, "CSystemVarSet", false);
    lua_tinker::class_def<CSystemVarSet>(L, "CreateVar", &CSystemVarSet::CreateVar);
    lua_tinker::class_def<CSystemVarSet>(L, "DeleteVar", &CSystemVarSet::DeleteVar);
    lua_tinker::class_def<CSystemVarSet>(L, "QueryVar", &CSystemVarSet::QueryVar, false);
    lua_tinker::class_def<CSystemVarSet>(L, "SyncToClient", &CSystemVarSet::SyncToClient);
    lua_tinker::class_add<CTeamInfo>(L, "CTeamInfo", false);
    lua_tinker::class_def<CTeamInfo>(L, "GetMemberIDByIdx", &CTeamInfo::GetMemberIDByIdx);
    lua_tinker::class_def<CTeamInfo>(L, "GetMemeberAmount", &CTeamInfo::GetMemeberAmount);
    lua_tinker::class_def<CTeamInfo>(L, "GetTeamLeaderID", &CTeamInfo::GetTeamLeaderID);
    lua_tinker::class_def<CTeamInfo>(L, "IsTeamLeader", &CTeamInfo::IsTeamLeader);
    lua_tinker::class_def<CTeamInfo>(L, "IsTeamMember", &CTeamInfo::IsTeamMember);
    lua_tinker::class_add<CTeamInfoManager>(L, "CTeamInfoManager", false);
    lua_tinker::class_def<CTeamInfoManager>(L, "QueryTeam", &CTeamInfoManager::QueryTeam);
    lua_tinker::class_add<CreateMonsterParam>(L, "CreateMonsterParam", false);
    lua_tinker::class_add<Degree>(L, "Degree", false);
    lua_tinker::class_def<Degree>(L, "operator!=", &Degree::operator!=);
    lua_tinker::class_def<Degree>(L,
                                  "__mul",
                                  lua_tinker::args_type_overload_member_functor(
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Degree&) const)(&Degree::operator*)),
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(float) const)(&Degree::operator*))));
    lua_tinker::class_def<Degree>(L,
                                  "__add",
                                  lua_tinker::args_type_overload_member_functor(
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Degree&) const)(&Degree::operator+)),
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Radian&) const)(&Degree::operator+)),
                                      lua_tinker::make_member_functor_ptr((const Degree& (Degree::*)() const)(&Degree::operator+))));
    lua_tinker::class_def<Degree>(L,
                                  "__sub",
                                  lua_tinker::args_type_overload_member_functor(
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)() const)(&Degree::operator-)),
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Degree&) const)(&Degree::operator-)),
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Radian&) const)(&Degree::operator-))));
    lua_tinker::class_def<Degree>(L, "__div", &Degree::operator/);
    lua_tinker::class_def<Degree>(L, "__lt", &Degree::operator<);
    lua_tinker::class_def<Degree>(L, "__le", &Degree::operator<=);
    lua_tinker::class_def<Degree>(
        L,
        "operator=",
        lua_tinker::args_type_overload_member_functor(lua_tinker::make_member_functor_ptr((Degree & (Degree::*)(const Degree&))(&Degree::operator=)),
                                                      lua_tinker::make_member_functor_ptr((Degree & (Degree::*)(const Radian&))(&Degree::operator=)),
                                                      lua_tinker::make_member_functor_ptr((Degree & (Degree::*)(const float&))(&Degree::operator=))));
    lua_tinker::class_def<Degree>(L, "__eq", &Degree::operator==);
    lua_tinker::class_def<Degree>(L, "operator>", &Degree::operator>);
    lua_tinker::class_def<Degree>(L, "operator>=", &Degree::operator>=);
    lua_tinker::class_def<Degree>(L, "valueAngleUnits", &Degree::valueAngleUnits);
    lua_tinker::class_def<Degree>(L, "valueDegrees", &Degree::valueDegrees);
    lua_tinker::class_def<Degree>(L, "valueRadians", &Degree::valueRadians);
    lua_tinker::class_con<Degree>(
        L,
        lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Degree, const Degree&>(),
                                                   new lua_tinker::constructor<Degree, const Radian&>(),
                                                   new lua_tinker::constructor<Degree, float>(1 /*default_args_count*/, 1 /*default_args_start*/)),
        0);
    lua_tinker::class_add<FloatRect>(L, "FloatRect", false);
    lua_tinker::class_def<FloatRect>(L, "height", &FloatRect::height);
    lua_tinker::class_def<FloatRect>(L, "isIntersect", &FloatRect::isIntersect);
    lua_tinker::class_def<FloatRect>(L, "width", &FloatRect::width);
    lua_tinker::class_con<FloatRect>(
        L,
        lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<FloatRect>(),
                                                   new lua_tinker::constructor<FloatRect, const FloatRect&>(),
                                                   new lua_tinker::constructor<FloatRect, float, float, float, float>()));
    lua_tinker::class_mem<FloatRect>(L, "bottom", &FloatRect::bottom);
    lua_tinker::class_mem<FloatRect>(L, "left", &FloatRect::left);
    lua_tinker::class_mem<FloatRect>(L, "right", &FloatRect::right);
    lua_tinker::class_mem<FloatRect>(L, "top", &FloatRect::top);
    lua_tinker::class_add<ICoolDown>(L, "ICoolDown", false);
    lua_tinker::class_def<ICoolDown>(L, "ClearCoolDown", &ICoolDown::ClearCoolDown, false, true);
    lua_tinker::class_def<ICoolDown>(L, "GetExpireTime", &ICoolDown::GetExpireTime);
    lua_tinker::class_def<ICoolDown>(L, "StartCoolDown", &ICoolDown::StartCoolDown, false, true);
    lua_tinker::class_add<MapGridData>(L, "MapGridData", false);
    lua_tinker::class_con<MapGridData>(L, lua_tinker::constructor<MapGridData, uint32_t>::invoke, 0);
    lua_tinker::class_add<Math>(L, "Math", false);
    lua_tinker::class_def_static<Math>(L, "ACos", &Math::ACos);
    lua_tinker::class_def_static<Math>(L, "ASin", &Math::ASin);
    lua_tinker::class_def_static<Math>(L, "ATan", &Math::ATan);
    lua_tinker::class_def_static<Math>(L, "ATan2", &Math::ATan2);
    lua_tinker::class_def_static<Math>(L,
                                       "Abs",
                                       lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((Degree(*)(const Degree&))(&Math::Abs)),
                                                                              lua_tinker::make_functor_ptr((Radian(*)(const Radian&))(&Math::Abs)),
                                                                              lua_tinker::make_functor_ptr((float (*)(float))(&Math::Abs))));
    lua_tinker::class_def_static<Math>(L, "AngleUnitsToDegrees", &Math::AngleUnitsToDegrees);
    lua_tinker::class_def_static<Math>(L, "AngleUnitsToRadians", &Math::AngleUnitsToRadians);
    lua_tinker::class_def_static<Math>(L, "Ceil", &Math::Ceil);
    lua_tinker::class_def_static<Math>(
        L,
        "Cos",
        lua_tinker::args_type_overload_functor(
            lua_tinker::make_functor_ptr((float (*)(const Radian&, bool))(&Math::Cos), 1 /*default_args_count*/, 1 /*default_args_start*/),
            lua_tinker::make_functor_ptr((float (*)(float, bool))(&Math::Cos), 1 /*default_args_count*/, 2 /*default_args_start*/)),
        false,
        false);
    lua_tinker::class_def_static<Math>(L, "DegreesToAngleUnits", &Math::DegreesToAngleUnits);
    lua_tinker::class_def_static<Math>(L, "DegreesToRadians", &Math::DegreesToRadians);
    lua_tinker::class_def_static<Math>(L, "Exp", &Math::Exp);
    lua_tinker::class_def_static<Math>(L, "FloatEqual", &Math::FloatEqual, std::numeric_limits<float>::epsilon());
    lua_tinker::class_def_static<Math>(L, "Floor", &Math::Floor);
    lua_tinker::class_def_static<Math>(L, "IAbs", &Math::IAbs);
    lua_tinker::class_def_static<Math>(L, "ICeil", &Math::ICeil);
    lua_tinker::class_def_static<Math>(L, "IFloor", &Math::IFloor);
    lua_tinker::class_def_static<Math>(L, "ISign", &Math::ISign);
    lua_tinker::class_def_static<Math>(L, "InvSqrt", &Math::InvSqrt);
    lua_tinker::class_def_static<Math>(L, "Log", &Math::Log);
    lua_tinker::class_def_static<Math>(L, "Log2", &Math::Log2);
    lua_tinker::class_def_static<Math>(L, "LogN", &Math::LogN);
    lua_tinker::class_def_static<Math>(L, "Pow", &Math::Pow);
    lua_tinker::class_def_static<Math>(L, "RadiansToAngleUnits", &Math::RadiansToAngleUnits);
    lua_tinker::class_def_static<Math>(L, "RadiansToDegrees", &Math::RadiansToDegrees);
    lua_tinker::class_def_static<Math>(L,
                                       "Sign",
                                       lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((Degree(*)(const Degree&))(&Math::Sign)),
                                                                              lua_tinker::make_functor_ptr((Radian(*)(const Radian&))(&Math::Sign)),
                                                                              lua_tinker::make_functor_ptr((float (*)(float))(&Math::Sign))));
    lua_tinker::class_def_static<Math>(
        L,
        "Sin",
        lua_tinker::args_type_overload_functor(
            lua_tinker::make_functor_ptr((float (*)(const Radian&, bool))(&Math::Sin), 1 /*default_args_count*/, 1 /*default_args_start*/),
            lua_tinker::make_functor_ptr((float (*)(float, bool))(&Math::Sin), 1 /*default_args_count*/, 2 /*default_args_start*/)),
        false,
        false);
    lua_tinker::class_def_static<Math>(L, "Sqr", &Math::Sqr);
    lua_tinker::class_def_static<Math>(L,
                                       "Sqrt",
                                       lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((Degree(*)(const Degree&))(&Math::Sqrt)),
                                                                              lua_tinker::make_functor_ptr((Radian(*)(const Radian&))(&Math::Sqrt)),
                                                                              lua_tinker::make_functor_ptr((float (*)(float))(&Math::Sqrt))));
    lua_tinker::class_def_static<Math>(
        L,
        "Tan",
        lua_tinker::args_type_overload_functor(
            lua_tinker::make_functor_ptr((float (*)(const Radian&, bool))(&Math::Tan), 1 /*default_args_count*/, 1 /*default_args_start*/),
            lua_tinker::make_functor_ptr((float (*)(float, bool))(&Math::Tan), 1 /*default_args_count*/, 2 /*default_args_start*/)),
        false,
        false);
    lua_tinker::class_def_static<Math>(L, "get2Pow", &Math::get2Pow);
    lua_tinker::class_def_static<Math>(L, "getAngleUnit", &Math::getAngleUnit);
    lua_tinker::class_def_static<Math>(L, "ilog2", &Math::ilog2);
    lua_tinker::class_def_static<Math>(L, "is2Pow", &Math::is2Pow);
    lua_tinker::class_def_static<Math>(L, "isNaN", &Math::isNaN);
    lua_tinker::class_def_static<Math>(L, "nextPow2", &Math::nextPow2);
    lua_tinker::class_def_static<Math>(L,
                                       "saturate",
                                       lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((double (*)(double))(&Math::saturate)),
                                                                              lua_tinker::make_functor_ptr((float (*)(float))(&Math::saturate))));
    lua_tinker::class_mem_static_readonly<Math>(L, "HALF_PI", &Math::HALF_PI);
    lua_tinker::class_mem_static_readonly<Math>(L, "PI", &Math::PI);
    lua_tinker::class_mem_static_readonly<Math>(L, "TWO_PI", &Math::TWO_PI);
    lua_tinker::class_add<Ray>(L, "Ray", false);
    lua_tinker::class_def<Ray>(L, "getDirection", &Ray::getDirection);
    lua_tinker::class_def<Ray>(L, "getOrigin", &Ray::getOrigin);
    lua_tinker::class_def<Ray>(L, "getPoint", &Ray::getPoint);
    lua_tinker::class_def<Ray>(L, "__mul", &Ray::operator*);
    lua_tinker::class_def<Ray>(L, "setDirection", &Ray::setDirection);
    lua_tinker::class_def<Ray>(L, "setOrigin", &Ray::setOrigin);
    lua_tinker::class_con<Ray>(L,
                               lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Ray>(),
                                                                          new lua_tinker::constructor<Ray, const Vector3&, const Vector3&>()));
    lua_tinker::class_add<Rect>(L, "Rect", false);
    lua_tinker::class_def<Rect>(L, "height", &Rect::height);
    lua_tinker::class_def<Rect>(L, "isIntersect", &Rect::isIntersect);
    lua_tinker::class_def<Rect>(L, "operator=", &Rect::operator=);
    lua_tinker::class_def<Rect>(L, "width", &Rect::width);
    lua_tinker::class_con<Rect>(L,
                                lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Rect>(),
                                                                           new lua_tinker::constructor<Rect, const Rect&>(),
                                                                           new lua_tinker::constructor<Rect, long, long, long, long>()));
    lua_tinker::class_mem<Rect>(L, "bottom", &Rect::bottom);
    lua_tinker::class_mem<Rect>(L, "left", &Rect::left);
    lua_tinker::class_mem<Rect>(L, "right", &Rect::right);
    lua_tinker::class_mem<Rect>(L, "top", &Rect::top);
    lua_tinker::class_add<ST_ITEMINFO>(L, "ST_ITEMINFO", false);
    lua_tinker::class_mem<ST_ITEMINFO>(L, "dwFlag", &ST_ITEMINFO::dwFlag);
    lua_tinker::class_mem<ST_ITEMINFO>(L, "idItem", &ST_ITEMINFO::idItem);
    lua_tinker::class_mem<ST_ITEMINFO>(L, "idOwner", &ST_ITEMINFO::idOwner);
    lua_tinker::class_mem<ST_ITEMINFO>(L, "idType", &ST_ITEMINFO::idType);
    lua_tinker::class_mem<ST_ITEMINFO>(L, "nAddition", &ST_ITEMINFO::nAddition);
    lua_tinker::class_mem<ST_ITEMINFO>(L, "nGrid", &ST_ITEMINFO::nGrid);
    lua_tinker::class_mem<ST_ITEMINFO>(L, "nNum", &ST_ITEMINFO::nNum);
    lua_tinker::class_mem<ST_ITEMINFO>(L, "nPosition", &ST_ITEMINFO::nPosition);
    lua_tinker::class_add<ST_STATUS_INFO>(L, "ST_STATUS_INFO", false);
    lua_tinker::class_add<SceneIdx>(L, "SceneIdx", false);
    lua_tinker::class_def<SceneIdx>(L, "GetMapID", &SceneIdx::GetMapID);
    lua_tinker::class_def<SceneIdx>(L, "GetPhaseIdx", &SceneIdx::GetPhaseIdx);
    lua_tinker::class_def<SceneIdx>(L, "GetZoneID", &SceneIdx::GetZoneID);
    lua_tinker::class_def<SceneIdx>(L, "__lt", &SceneIdx::operator<);
    lua_tinker::class_def<SceneIdx>(L, "__eq", &SceneIdx::operator==);
    lua_tinker::class_con<SceneIdx>(L,
                                    lua_tinker::args_type_overload_constructor(
                                        new lua_tinker::constructor<SceneIdx, const SceneIdx&>(),
                                        new lua_tinker::constructor<SceneIdx, uint16_t, uint16_t, uint32_t>(),
                                        new lua_tinker::constructor<SceneIdx, uint64_t>(1 /*default_args_count*/, 1 /*default_args_start*/)),
                                    0);
    lua_tinker::class_add<ServerPort>(L, "ServerPort", false);
    lua_tinker::class_def<ServerPort>(L, "GetData", &ServerPort::GetData);
    lua_tinker::class_def<ServerPort>(L, "GetServiceID", &ServerPort::GetServiceID);
    lua_tinker::class_def<ServerPort>(L, "GetServiceIdx", &ServerPort::GetServiceIdx);
    lua_tinker::class_def<ServerPort>(L, "GetServiceType", &ServerPort::GetServiceType);
    lua_tinker::class_def<ServerPort>(L, "GetWorldID", &ServerPort::GetWorldID);
    lua_tinker::class_def<ServerPort>(L, "IsVaild", &ServerPort::IsVaild);
    lua_tinker::class_def<ServerPort>(L, "SetData", &ServerPort::SetData);
    lua_tinker::class_def<ServerPort>(L, "SetServiceID", &ServerPort::SetServiceID);
    lua_tinker::class_def<ServerPort>(L, "SetServiceIdx", &ServerPort::SetServiceIdx);
    lua_tinker::class_def<ServerPort>(L, "SetServiceType", &ServerPort::SetServiceType);
    lua_tinker::class_def<ServerPort>(L, "SetWorldID", &ServerPort::SetWorldID);
    lua_tinker::class_def<ServerPort>(L, "__lt", &ServerPort::operator<);
    lua_tinker::class_def<ServerPort>(L, "__eq", &ServerPort::operator==);
    lua_tinker::class_con<ServerPort>(L,
                                      lua_tinker::args_type_overload_constructor(
                                          new lua_tinker::constructor<ServerPort, const ServerPort&>(),
                                          new lua_tinker::constructor<ServerPort, uint16_t, const ServiceID&>(),
                                          new lua_tinker::constructor<ServerPort, uint16_t, uint8_t, uint8_t>(),
                                          new lua_tinker::constructor<ServerPort, uint32_t>(1 /*default_args_count*/, 1 /*default_args_start*/)),
                                      0);
    lua_tinker::class_add<ServiceID>(L, "ServiceID", false);
    lua_tinker::class_def<ServiceID>(L, "GetData", &ServiceID::GetData);
    lua_tinker::class_def<ServiceID>(L, "GetServiceIdx", &ServiceID::GetServiceIdx);
    lua_tinker::class_def<ServiceID>(L, "GetServiceType", &ServiceID::GetServiceType);
    lua_tinker::class_def<ServiceID>(L, "IsVaild", &ServiceID::IsVaild);
    lua_tinker::class_def<ServiceID>(L, "SetData", &ServiceID::SetData);
    lua_tinker::class_def<ServiceID>(L, "SetServiceIdx", &ServiceID::SetServiceIdx);
    lua_tinker::class_def<ServiceID>(L, "SetServiceType", &ServiceID::SetServiceType);
    lua_tinker::class_def<ServiceID>(L, "__lt", &ServiceID::operator<);
    lua_tinker::class_def<ServiceID>(L, "__eq", &ServiceID::operator==);
    lua_tinker::class_con<ServiceID>(L,
                                     lua_tinker::args_type_overload_constructor(
                                         new lua_tinker::constructor<ServiceID, const ServiceID&>(),
                                         new lua_tinker::constructor<ServiceID, uint16_t>(1 /*default_args_count*/, 1 /*default_args_start*/),
                                         new lua_tinker::constructor<ServiceID, uint8_t, uint8_t>()),
                                     0);
    lua_tinker::class_add<TargetSceneID>(L, "TargetSceneID", false);
    lua_tinker::class_def<TargetSceneID>(L, "GetMapID", &TargetSceneID::GetMapID);
    lua_tinker::class_def<TargetSceneID>(L, "GetPhaseID", &TargetSceneID::GetPhaseID);
    lua_tinker::class_def<TargetSceneID>(L, "GetZoneID", &TargetSceneID::GetZoneID);
    lua_tinker::class_def<TargetSceneID>(L, "IsGuildPhaseID", &TargetSceneID::IsGuildPhaseID);
    lua_tinker::class_def<TargetSceneID>(L, "IsSelfPhaseID", &TargetSceneID::IsSelfPhaseID);
    lua_tinker::class_def<TargetSceneID>(L, "IsTeamPhaseID", &TargetSceneID::IsTeamPhaseID);
    lua_tinker::class_def<TargetSceneID>(L, "__lt", &TargetSceneID::operator<);
    lua_tinker::class_def<TargetSceneID>(L, "__eq", &TargetSceneID::operator==);
    lua_tinker::class_con<TargetSceneID>(
        L,
        lua_tinker::args_type_overload_constructor(
            new lua_tinker::constructor<TargetSceneID, const TargetSceneID&>(),
            new lua_tinker::constructor<TargetSceneID, uint16_t, uint16_t, int32_t>(),
            new lua_tinker::constructor<TargetSceneID, uint64_t>(1 /*default_args_count*/, 1 /*default_args_start*/)),
        0);
    lua_tinker::class_add<Vector2>(L, "Vector2", false);
    lua_tinker::class_def_static<Vector2>(L, "NEGATIVE_UNIT_X", &Vector2::NEGATIVE_UNIT_X);
    lua_tinker::class_def_static<Vector2>(L, "NEGATIVE_UNIT_Y", &Vector2::NEGATIVE_UNIT_Y);
    lua_tinker::class_def_static<Vector2>(L, "UNIT_SCALE", &Vector2::UNIT_SCALE);
    lua_tinker::class_def_static<Vector2>(L, "UNIT_X", &Vector2::UNIT_X);
    lua_tinker::class_def_static<Vector2>(L, "UNIT_Y", &Vector2::UNIT_Y);
    lua_tinker::class_def_static<Vector2>(L, "ZERO", &Vector2::ZERO);
    lua_tinker::class_def<Vector2>(L, "crossProduct", &Vector2::crossProduct);
    lua_tinker::class_def<Vector2>(L, "distance", &Vector2::distance);
    lua_tinker::class_def<Vector2>(L, "dotProduct", &Vector2::dotProduct);
    lua_tinker::class_def<Vector2>(L, "isNaN", &Vector2::isNaN);
    lua_tinker::class_def<Vector2>(L, "isZeroLength", &Vector2::isZeroLength);
    lua_tinker::class_def<Vector2>(L, "length", &Vector2::length);
    lua_tinker::class_def<Vector2>(L, "makeCeil", &Vector2::makeCeil);
    lua_tinker::class_def<Vector2>(L, "makeFloor", &Vector2::makeFloor);
    lua_tinker::class_def<Vector2>(L, "midPoint", &Vector2::midPoint);
    lua_tinker::class_def<Vector2>(L, "normalise", &Vector2::normalise);
    lua_tinker::class_def<Vector2>(L, "normalisedCopy", &Vector2::normalisedCopy);
    lua_tinker::class_def<Vector2>(L, "operator!=", &Vector2::operator!=);
    lua_tinker::class_def<Vector2>(L, "__lt", &Vector2::operator<);
    lua_tinker::class_def<Vector2>(L, "__eq", &Vector2::operator==);
    lua_tinker::class_def<Vector2>(L, "perpendicular", &Vector2::perpendicular);
    lua_tinker::class_def<Vector2>(L, "randomDeviant", &Vector2::randomDeviant);
    lua_tinker::class_def<Vector2>(L, "reflect", &Vector2::reflect);
    lua_tinker::class_def<Vector2>(L, "squaredDistance", &Vector2::squaredDistance);
    lua_tinker::class_def<Vector2>(L, "squaredLength", &Vector2::squaredLength);
    lua_tinker::class_def<Vector2>(L, "swap", &Vector2::swap);
    lua_tinker::class_con<Vector2>(L,
                                   lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Vector2>(),
                                                                              new lua_tinker::constructor<Vector2, const float>(),
                                                                              new lua_tinker::constructor<Vector2, const float, const float>()));
    lua_tinker::class_mem<Vector2>(L, "x", &Vector2::x);
    lua_tinker::class_mem<Vector2>(L, "y", &Vector2::y);
    lua_tinker::class_add<Vector3>(L, "Vector3", false);
    lua_tinker::class_def_static<Vector3>(L, "NEGATIVE_UNIT_X", &Vector3::NEGATIVE_UNIT_X);
    lua_tinker::class_def_static<Vector3>(L, "NEGATIVE_UNIT_Y", &Vector3::NEGATIVE_UNIT_Y);
    lua_tinker::class_def_static<Vector3>(L, "NEGATIVE_UNIT_Z", &Vector3::NEGATIVE_UNIT_Z);
    lua_tinker::class_def_static<Vector3>(L, "UNIT_SCALE", &Vector3::UNIT_SCALE);
    lua_tinker::class_def_static<Vector3>(L, "UNIT_X", &Vector3::UNIT_X);
    lua_tinker::class_def_static<Vector3>(L, "UNIT_Y", &Vector3::UNIT_Y);
    lua_tinker::class_def_static<Vector3>(L, "UNIT_Z", &Vector3::UNIT_Z);
    lua_tinker::class_def_static<Vector3>(L, "ZERO", &Vector3::ZERO);
    lua_tinker::class_def<Vector3>(L, "absDotProduct", &Vector3::absDotProduct);
    lua_tinker::class_def<Vector3>(L, "crossProduct", &Vector3::crossProduct);
    lua_tinker::class_def<Vector3>(L, "distance", &Vector3::distance);
    lua_tinker::class_def<Vector3>(L, "dotProduct", &Vector3::dotProduct);
    lua_tinker::class_def<Vector3>(L, "isNaN", &Vector3::isNaN);
    lua_tinker::class_def<Vector3>(L, "isZeroLength", &Vector3::isZeroLength);
    lua_tinker::class_def<Vector3>(L, "length", &Vector3::length);
    lua_tinker::class_def<Vector3>(L, "makeCeil", &Vector3::makeCeil);
    lua_tinker::class_def<Vector3>(L, "makeFloor", &Vector3::makeFloor);
    lua_tinker::class_def<Vector3>(L, "midPoint", &Vector3::midPoint);
    lua_tinker::class_def<Vector3>(L, "normalise", &Vector3::normalise);
    lua_tinker::class_def<Vector3>(L, "normalisedCopy", &Vector3::normalisedCopy);
    lua_tinker::class_def<Vector3>(L, "operator!=", &Vector3::operator!=);
    lua_tinker::class_def<Vector3>(L, "__lt", &Vector3::operator<);
    lua_tinker::class_def<Vector3>(L, "__eq", &Vector3::operator==);
    lua_tinker::class_def<Vector3>(L, "operator>", &Vector3::operator>);
    lua_tinker::class_def<Vector3>(L, "perpendicular", &Vector3::perpendicular);
    lua_tinker::class_def<Vector3>(L, "positionCloses", &Vector3::positionCloses, 1e-03f);
    lua_tinker::class_def<Vector3>(L, "positionEquals", &Vector3::positionEquals, 1e-03);
    lua_tinker::class_def<Vector3>(L, "primaryAxis", &Vector3::primaryAxis);
    lua_tinker::class_def<Vector3>(L, "reflect", &Vector3::reflect);
    lua_tinker::class_def<Vector3>(L, "squaredDistance", &Vector3::squaredDistance);
    lua_tinker::class_def<Vector3>(L, "squaredLength", &Vector3::squaredLength);
    lua_tinker::class_def<Vector3>(L, "swap", &Vector3::swap);
    lua_tinker::class_con<Vector3>(
        L,
        lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Vector3>(),
                                                   new lua_tinker::constructor<Vector3, const float>(),
                                                   new lua_tinker::constructor<Vector3, const float, const float, const float>()));
    lua_tinker::class_mem<Vector3>(L, "x", &Vector3::x);
    lua_tinker::class_mem<Vector3>(L, "y", &Vector3::y);
    lua_tinker::class_mem<Vector3>(L, "z", &Vector3::z);
    lua_tinker::class_add<VirtualSocket>(L, "VirtualSocket", false);
    lua_tinker::class_def_static<VirtualSocket>(L, "CreateVirtualSocket", &VirtualSocket::CreateVirtualSocket);
    lua_tinker::class_def<VirtualSocket>(L, "GetData64", &VirtualSocket::GetData64);
    lua_tinker::class_def<VirtualSocket>(L, "GetServerPort", &VirtualSocket::GetServerPort);
    lua_tinker::class_def<VirtualSocket>(L, "GetServiceID", &VirtualSocket::GetServiceID);
    lua_tinker::class_def<VirtualSocket>(L, "GetServiceIdx", &VirtualSocket::GetServiceIdx);
    lua_tinker::class_def<VirtualSocket>(L, "GetServiceType", &VirtualSocket::GetServiceType);
    lua_tinker::class_def<VirtualSocket>(L, "GetSocketIdx", &VirtualSocket::GetSocketIdx);
    lua_tinker::class_def<VirtualSocket>(L, "GetWorldID", &VirtualSocket::GetWorldID);
    lua_tinker::class_def<VirtualSocket>(L, "IsVaild", &VirtualSocket::IsVaild);
    lua_tinker::class_def<VirtualSocket>(L, "SetServerPort", &VirtualSocket::SetServerPort);
    lua_tinker::class_def<VirtualSocket>(L, "SetSocketIdx", &VirtualSocket::SetSocketIdx);
    lua_tinker::class_def<VirtualSocket>(L, "__lt", &VirtualSocket::operator<);
    lua_tinker::class_def<VirtualSocket>(L, "__eq", &VirtualSocket::operator==);
    lua_tinker::class_con<VirtualSocket>(
        L,
        lua_tinker::args_type_overload_constructor(
            new lua_tinker::constructor<VirtualSocket, const ServerPort&>(),
            new lua_tinker::constructor<VirtualSocket, const ServerPort&, uint16_t>(),
            new lua_tinker::constructor<VirtualSocket, const VirtualSocket&>(),
            new lua_tinker::constructor<VirtualSocket, uint64_t>(1 /*default_args_count*/, 1 /*default_args_start*/)),
        0);
    lua_tinker::namespace_add(L, "GameMath");
    lua_tinker::namespace_def(L, "GameMath", "DDALineEx", &GameMath::DDALineEx);
    lua_tinker::namespace_def(L, "GameMath", "DDALineForeach", &GameMath::DDALineForeach);
    lua_tinker::namespace_def(L, "GameMath", "correctRect", &GameMath::correctRect);
    lua_tinker::namespace_def(L, "GameMath", "distance", &GameMath::distance);
    lua_tinker::namespace_def(L, "GameMath", "distance2D", &GameMath::distance2D);
    lua_tinker::namespace_def(L, "GameMath", "getAngleFrom", &GameMath::getAngleFrom, GameMath::VECTOR2_NORTH);
    lua_tinker::namespace_def(L, "GameMath", "getDirectAngleFrom", &GameMath::getDirectAngleFrom, GameMath::VECTOR2_NORTH, true);
    lua_tinker::namespace_def(L, "GameMath", "getDirectRadianFrom", &GameMath::getDirectRadianFrom, GameMath::VECTOR2_NORTH, true);
    lua_tinker::namespace_def(L, "GameMath", "getRadianFrom", &GameMath::getRadianFrom, GameMath::VECTOR2_NORTH);
    lua_tinker::namespace_def(L, "GameMath", "isIntersect", &GameMath::isIntersect);
    lua_tinker::namespace_def(L, "GameMath", "manhattanDistance", &GameMath::manhattanDistance);
    lua_tinker::namespace_def(L, "GameMath", "random_vector2", &GameMath::random_vector2);
    lua_tinker::namespace_def(L, "GameMath", "rotate", &GameMath::rotate);
    lua_tinker::namespace_def(L, "GameMath", "rotateByRadian", &GameMath::rotateByRadian);
    lua_tinker::namespace_def(L, "GameMath", "simpleDistance", &GameMath::simpleDistance);
    lua_tinker::namespace_set(L, "GameMath", "VECTOR2_EAST", GameMath::VECTOR2_EAST);
    lua_tinker::namespace_set(L, "GameMath", "VECTOR2_NORTH", GameMath::VECTOR2_NORTH);
    lua_tinker::namespace_set(L, "GameMath", "VECTOR2_SOUTH", GameMath::VECTOR2_SOUTH);
    lua_tinker::namespace_set(L, "GameMath", "VECTOR2_WEST", GameMath::VECTOR2_WEST);
    lua_tinker::namespace_add(L, "GameMath::Intersection");
    lua_tinker::scope_inner(L, "GameMath", "Intersection", "GameMath::Intersection");
    lua_tinker::namespace_add(L, "GameMath::Intersection2D");
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "LineIntersection2D", &GameMath::Intersection2D::LineIntersection2D);
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "getNearestPoint", &GameMath::Intersection2D::getNearestPoint);
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "isInABBox", &GameMath::Intersection2D::isInABBox);
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "isInFOV", &GameMath::Intersection2D::isInFOV);
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "lineIntersection2D", &GameMath::Intersection2D::lineIntersection2D);
    lua_tinker::scope_inner(L, "GameMath", "Intersection2D", "GameMath::Intersection2D");
    lua_tinker::def(L, "AchievementTypeSet", &AchievementTypeSet);
    lua_tinker::def(L, "ActorManager", &ActorManager);
    lua_tinker::def(L, "BulletTypeSet", &BulletTypeSet);
    lua_tinker::def(L, "CheckSameDay", &CheckSameDay);
    lua_tinker::def(L, "DataCountLimitSet", &DataCountLimitSet);
    lua_tinker::def(L, "DayDiffLocal", &DayDiffLocal);
    lua_tinker::def(L, "EventManager", &EventManager);
    lua_tinker::def(L, "GMManager", &GMManager);
    lua_tinker::def(L, "GetHighFromU64", &GetHighFromU64);
    lua_tinker::def(L, "GetLowFromU64", &GetLowFromU64);
    lua_tinker::def(L, "GetNextDayBeginTime", &GetNextDayBeginTime);
    lua_tinker::def(L, "GetTimeFromString", &GetTimeFromString);
    lua_tinker::def(L, "HasFlag", &HasFlag);
    lua_tinker::def(L, "ItemAdditionSet", &ItemAdditionSet);
    lua_tinker::def(L, "ItemFormulaDataSet", &ItemFormulaDataSet);
    lua_tinker::def(L, "ItemTypeSet", &ItemTypeSet);
    lua_tinker::def(L, "ItemUpgradeDataSet", &ItemUpgradeDataSet);
    lua_tinker::def(L, "MAKE32", &MAKE32);
    lua_tinker::def(L, "MAKE64", &MAKE64);
    lua_tinker::def(L, "MakeUINT16", &MakeUINT16);
    lua_tinker::def(L, "MakeUINT32", &MakeUINT32);
    lua_tinker::def(L, "MakeUINT64", &MakeUINT64);
    lua_tinker::def(L, "MapManager", &MapManager);
    lua_tinker::def(L, "MonitorMgr", &MonitorMgr);
    lua_tinker::def(L, "MonsterTypeSet", &MonsterTypeSet);
    lua_tinker::def(L, "MonthDiffLocal", &MonthDiffLocal);
    lua_tinker::def(L, "MulDiv", &MulDiv);
    lua_tinker::def(L, "MulDivSign", &MulDivSign);
    lua_tinker::def(L, "NetMsgProcess", &NetMsgProcess);
    lua_tinker::def(L, "NextDayBeginTimeStamp", &NextDayBeginTimeStamp);
    lua_tinker::def(L, "NextMonthBeginTimeStamp", &NextMonthBeginTimeStamp);
    lua_tinker::def(L, "NextWeekBeginTimeStamp", &NextWeekBeginTimeStamp);
    lua_tinker::def(L, "NpcTypeSet", &NpcTypeSet);
    lua_tinker::def(L, "PetTypeSet", &PetTypeSet);
    lua_tinker::def(L, "SceneManager", &SceneManager);
    lua_tinker::def(L, "SceneService", &SceneService);
    lua_tinker::def(L, "ScriptManager", &ScriptManager);
    lua_tinker::def(L, "SkillTypeSet", &SkillTypeSet);
    lua_tinker::def(L, "SplitUINT16", &SplitUINT16);
    lua_tinker::def(L, "SplitUINT32", &SplitUINT32);
    lua_tinker::def(L, "SplitUINT64", &SplitUINT64);
    lua_tinker::def(L, "StatusTypeSet", &StatusTypeSet);
    lua_tinker::def(L, "SuitEquipSet", &SuitEquipSet);
    lua_tinker::def(L, "SystemVarSet", &SystemVarSet);
    lua_tinker::def(L, "TaskTypeSet", &TaskTypeSet);
    lua_tinker::def(L, "TeamManager", &TeamManager);
    lua_tinker::def(L, "TimeGetMillisecond", &TimeGetMillisecond);
    lua_tinker::def(L, "TimeGetMonotonic", &TimeGetMonotonic);
    lua_tinker::def(L, "TimeGetSecond", &TimeGetSecond);
    lua_tinker::def(L, "TimeGetSecondLocal", &TimeGetSecondLocal);
    lua_tinker::def(L, "UserAttrSet", &UserAttrSet);
    lua_tinker::def(L, "WeekDiffLocal", &WeekDiffLocal);
    lua_tinker::def(L, "_TimeGetMillisecond", &_TimeGetMillisecond);
    lua_tinker::def(L, "_TimeGetMonotonic", &_TimeGetMonotonic);
    lua_tinker::def(L, "_TimeGetSecond", &_TimeGetSecond);
    lua_tinker::def(L, "_TimeGetSecondFrom2K2K", &_TimeGetSecondFrom2K2K);
    lua_tinker::def(L, "_TimeGetSecondLocal", &_TimeGetSecondLocal);
    lua_tinker::def(L, "bit_flip", &bit_flip);
    lua_tinker::def(L, "bit_set", &bit_set);
    lua_tinker::def(L, "bit_test", &bit_test);
    lua_tinker::def(L, "convert_enc", &convert_enc);
    lua_tinker::def(L,
                    "find_name_error",
                    lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((bool (*)(const std::string&))(&find_name_error)),
                                                           lua_tinker::make_functor_ptr((bool (*)(const std::wstring&))(&find_name_error))));
    lua_tinker::def(L, "get_basepath", &get_basepath);
    lua_tinker::def(L, "get_file_ext", &get_file_ext);
    lua_tinker::def(L, "get_filename_from_fullpath", &get_filename_from_fullpath);
    lua_tinker::def(L, "get_filename_without_ext", &get_filename_without_ext);
    lua_tinker::def(L, "get_fullpath", &get_fullpath);
    lua_tinker::def(L, "gmt2local", &gmt2local);
    lua_tinker::def(L, "hex_get", &hex_get);
    lua_tinker::def(L, "hex_set", &hex_set);
    lua_tinker::def(L, "is_utf8_none_control", &is_utf8_none_control);
    lua_tinker::def(L, "isleap", &isleap);
    lua_tinker::def(L, "local2gmt", &local2gmt);
    lua_tinker::def(L, "lower_cast", &lower_cast);
    lua_tinker::def(L, "lower_cast_copy", &lower_cast_copy);
    lua_tinker::def(L, "ltrim", &ltrim);
    lua_tinker::def(L, "ltrim_copy", &ltrim_copy);
    lua_tinker::def(L, "now", &now);
    lua_tinker::def(L, "random_float", &random_float, 0.0f, 1.0f);
    lua_tinker::def(L, "random_hit", &random_hit);
    lua_tinker::def(L, "random_uint32", &random_uint32);
    lua_tinker::def(L, "random_uint32_range", &random_uint32_range);
    lua_tinker::def(L, "regex_str_check", &regex_str_check);
    lua_tinker::def(L, "regex_str_reload", &regex_str_reload);
    lua_tinker::def(L,
                    "replace_illegawords",
                    lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((bool (*)(std::string&))(&replace_illegawords)),
                                                           lua_tinker::make_functor_ptr((bool (*)(std::wstring&))(&replace_illegawords))));
    lua_tinker::def(L, "replace_str", &replace_str);
    lua_tinker::def(L, "replace_str_copy", &replace_str_copy);
    lua_tinker::def(L, "rtrim", &rtrim);
    lua_tinker::def(L, "rtrim_copy", &rtrim_copy);
    lua_tinker::def(L, "skip_utf8_bom", &skip_utf8_bom);
    lua_tinker::def(L, "split_string", &split_string);
    lua_tinker::def(L, "split_string_view", &split_string_view);
    lua_tinker::def(L, "string_concat", &string_concat);
    lua_tinker::def(L, "timeToLocalTime", &timeToLocalTime);
    lua_tinker::def(L, "toHex", &toHex);
    lua_tinker::def(L, "trim", &trim);
    lua_tinker::def(L, "trim_copy", &trim_copy);
    lua_tinker::def(L, "trim_path", &trim_path);
    lua_tinker::def(L, "upper_cast", &upper_cast);
    lua_tinker::def(L, "upper_cast_copy", &upper_cast_copy);
    lua_tinker::def(L, "url_decode", &url_decode);
    lua_tinker::def(L, "url_encode", &url_encode);
    lua_tinker::def(L, "utf8_length", &utf8_length, 0);
    lua_tinker::set(L, "DEFAULT_BAG_SIZE", DEFAULT_BAG_SIZE);
    lua_tinker::set(L, "DEFAULT_STROGE_SIZE", DEFAULT_STROGE_SIZE);
    lua_tinker::set(L, "ITEM_MAINTYPE_MASK", ITEM_MAINTYPE_MASK);
    lua_tinker::set(L, "ITEM_SUBTYPE_MASK", ITEM_SUBTYPE_MASK);
    lua_tinker::set(L, "MAX_ACCEPT_TASK", MAX_ACCEPT_TASK);
    lua_tinker::set(L, "MAX_ITEM_EXTRDATA_NUM", MAX_ITEM_EXTRDATA_NUM);
    lua_tinker::set(L, "STR_TASK_ACCEPT", STR_TASK_ACCEPT);
    lua_tinker::set(L, "STR_TASK_QUICK_FINISH", STR_TASK_QUICK_FINISH);
    lua_tinker::set(L, "STR_TASK_SUBMIT", STR_TASK_SUBMIT);
    lua_tinker::set(L, "UPDATE_FALSE", UPDATE_FALSE);
    lua_tinker::set(L, "UPDATE_TRUE", UPDATE_TRUE);
    lua_tinker::set(L, "AI_SERVICE", AI_SERVICE);
    lua_tinker::set(L, "AUTH_SERVICE", AUTH_SERVICE);
    lua_tinker::set(L, "COLLDOWN_TYPE_COMMON", COLLDOWN_TYPE_COMMON);
    lua_tinker::set(L, "COMMON_DATA_AUTO_SYNC", COMMON_DATA_AUTO_SYNC);
    lua_tinker::set(L, "COMMON_DATA_BEGIN", COMMON_DATA_BEGIN);
    lua_tinker::set(L, "COMMON_DATA_CLIENT_BEGIN", COMMON_DATA_CLIENT_BEGIN);
    lua_tinker::set(L, "COMMON_DATA_CLIENT_END", COMMON_DATA_CLIENT_END);
    lua_tinker::set(L, "COMMON_DATA_CONTINUELOGIN", COMMON_DATA_CONTINUELOGIN);
    lua_tinker::set(L, "COMMON_DATA_NOT_SYNC_CLIENT", COMMON_DATA_NOT_SYNC_CLIENT);
    lua_tinker::set(L, "COOLDOWN_TYPE_ITEM", COOLDOWN_TYPE_ITEM);
    lua_tinker::set(L, "COOLDOWN_TYPE_SKILL", COOLDOWN_TYPE_SKILL);
    lua_tinker::set(L, "COOLDOWN_TYPE_TALK", COOLDOWN_TYPE_TALK);
    lua_tinker::set(L, "DATA_ACC_BUYITEM", DATA_ACC_BUYITEM);
    lua_tinker::set(L, "DATA_ACC_MONSTERKILL", DATA_ACC_MONSTERKILL);
    lua_tinker::set(L, "DATA_ACC_SYSTEM", DATA_ACC_SYSTEM);
    lua_tinker::set(L, "DATA_ACC_SYSTEM_BEKILL_BYPLAYER", DATA_ACC_SYSTEM_BEKILL_BYPLAYER);
    lua_tinker::set(L, "DATA_ACC_SYSTEM_KILLPLAYER", DATA_ACC_SYSTEM_KILLPLAYER);
    lua_tinker::set(L, "DATA_ACC_SYSTEM_KILL_BOSS", DATA_ACC_SYSTEM_KILL_BOSS);
    lua_tinker::set(L, "DATA_ACC_SYSTEM_KILL_MONSTER", DATA_ACC_SYSTEM_KILL_MONSTER);
    lua_tinker::set(L, "DATA_ACC_SYSTEM_LOGINDAY", DATA_ACC_SYSTEM_LOGINDAY);
    lua_tinker::set(L, "DATA_ACC_SYSTEM_TXTATR_GLOBAL", DATA_ACC_SYSTEM_TXTATR_GLOBAL);
    lua_tinker::set(L, "DATA_ACC_TASKID", DATA_ACC_TASKID);
    lua_tinker::set(L, "DATA_ACC_USEITEM", DATA_ACC_USEITEM);
    lua_tinker::set(L, "DATA_COUNT_RESET_BY_DAY", DATA_COUNT_RESET_BY_DAY);
    lua_tinker::set(L, "DATA_COUNT_RESET_BY_MONTH", DATA_COUNT_RESET_BY_MONTH);
    lua_tinker::set(L, "DATA_COUNT_RESET_BY_WEEK", DATA_COUNT_RESET_BY_WEEK);
    lua_tinker::set(L, "DATA_COUNT_RESET_NEVER", DATA_COUNT_RESET_NEVER);
    lua_tinker::set(L, "DIALOGLINK_TYPE_BUTTON", DIALOGLINK_TYPE_BUTTON);
    lua_tinker::set(L, "DIALOGLINK_TYPE_LIST", DIALOGLINK_TYPE_LIST);
    lua_tinker::set(L, "DIALOGTYPE_ACCEPT_TASK", DIALOGTYPE_ACCEPT_TASK);
    lua_tinker::set(L, "DIALOGTYPE_NORMAL", DIALOGTYPE_NORMAL);
    lua_tinker::set(L, "DIALOGTYPE_SUBMIT_TASK", DIALOGTYPE_SUBMIT_TASK);
    lua_tinker::set(L, "DIALOGTYPE_WAITFINISH_TASK", DIALOGTYPE_WAITFINISH_TASK);
    lua_tinker::set(L, "DIALOG_FUNC_ACCEPTTASK", DIALOG_FUNC_ACCEPTTASK);
    lua_tinker::set(L, "DIALOG_FUNC_OPENSHOP", DIALOG_FUNC_OPENSHOP);
    lua_tinker::set(L, "DIALOG_FUNC_QUICKFINISHTASK", DIALOG_FUNC_QUICKFINISHTASK);
    lua_tinker::set(L, "DIALOG_FUNC_SCRIPT", DIALOG_FUNC_SCRIPT);
    lua_tinker::set(L, "DIALOG_FUNC_SHOWTASK", DIALOG_FUNC_SHOWTASK);
    lua_tinker::set(L, "DIALOG_FUNC_SUBMITTASK", DIALOG_FUNC_SUBMITTASK);
    lua_tinker::set(L, "EQUIPPOSITION_ARMOR", EQUIPPOSITION_ARMOR);
    lua_tinker::set(L, "EQUIPPOSITION_FASHION_DRESS", EQUIPPOSITION_FASHION_DRESS);
    lua_tinker::set(L, "EQUIPPOSITION_FASHION_WEAPON", EQUIPPOSITION_FASHION_WEAPON);
    lua_tinker::set(L, "EQUIPPOSITION_GIRDLE", EQUIPPOSITION_GIRDLE);
    lua_tinker::set(L, "EQUIPPOSITION_HAND_L", EQUIPPOSITION_HAND_L);
    lua_tinker::set(L, "EQUIPPOSITION_HAND_R", EQUIPPOSITION_HAND_R);
    lua_tinker::set(L, "EQUIPPOSITION_HELMET", EQUIPPOSITION_HELMET);
    lua_tinker::set(L, "EQUIPPOSITION_NECKLACE", EQUIPPOSITION_NECKLACE);
    lua_tinker::set(L, "EQUIPPOSITION_NONE", EQUIPPOSITION_NONE);
    lua_tinker::set(L, "EQUIPPOSITION_RING_L", EQUIPPOSITION_RING_L);
    lua_tinker::set(L, "EQUIPPOSITION_RING_R", EQUIPPOSITION_RING_R);
    lua_tinker::set(L, "EQUIPPOSITION_RUNE", EQUIPPOSITION_RUNE);
    lua_tinker::set(L, "EQUIPPOSITION_SHOES", EQUIPPOSITION_SHOES);
    lua_tinker::set(L, "EQUIPPOSITION_WEAPON", EQUIPPOSITION_WEAPON);
    lua_tinker::set(L, "EQUIPPOSITION_WING", EQUIPPOSITION_WING);
    lua_tinker::set(L, "GM_PROXY_SERVICE", GM_PROXY_SERVICE);
    lua_tinker::set(L, "GM_SERVICE", GM_SERVICE);
    lua_tinker::set(L, "GUILD_SERVICE", GUILD_SERVICE);
    lua_tinker::set(L, "ITEMFLAG_BATCH_USE_CHECK", ITEMFLAG_BATCH_USE_CHECK);
    lua_tinker::set(L, "ITEMFLAG_BUY_RUMOR", ITEMFLAG_BUY_RUMOR);
    lua_tinker::set(L, "ITEMFLAG_DELITEM_EXPIRE", ITEMFLAG_DELITEM_EXPIRE);
    lua_tinker::set(L, "ITEMFLAG_DEL_DISABLE", ITEMFLAG_DEL_DISABLE);
    lua_tinker::set(L, "ITEMFLAG_DROP_DISABLE", ITEMFLAG_DROP_DISABLE);
    lua_tinker::set(L, "ITEMFLAG_DROP_RUMOR", ITEMFLAG_DROP_RUMOR);
    lua_tinker::set(L, "ITEMFLAG_EQUIPEED_DISABLE_EXCHANGE", ITEMFLAG_EQUIPEED_DISABLE_EXCHANGE);
    lua_tinker::set(L, "ITEMFLAG_EXCHANGE_DISABLE", ITEMFLAG_EXCHANGE_DISABLE);
    lua_tinker::set(L, "ITEMFLAG_FORGING_DISABLE", ITEMFLAG_FORGING_DISABLE);
    lua_tinker::set(L, "ITEMFLAG_GAMBLING_RUMOR", ITEMFLAG_GAMBLING_RUMOR);
    lua_tinker::set(L, "ITEMFLAG_PICK_RUMOR", ITEMFLAG_PICK_RUMOR);
    lua_tinker::set(L, "ITEMFLAG_REPAIR_DISABLE", ITEMFLAG_REPAIR_DISABLE);
    lua_tinker::set(L, "ITEMFLAG_SELL_DISABLE", ITEMFLAG_SELL_DISABLE);
    lua_tinker::set(L, "ITEMFLAG_SPEND_DURA", ITEMFLAG_SPEND_DURA);
    lua_tinker::set(L, "ITEMFLAG_STORAGE_DISABLE", ITEMFLAG_STORAGE_DISABLE);
    lua_tinker::set(L, "ITEMFLAG_TRACE_ITEM", ITEMFLAG_TRACE_ITEM);
    lua_tinker::set(L, "ITEMPOSITION_BAG", ITEMPOSITION_BAG);
    lua_tinker::set(L, "ITEMPOSITION_BUYBACK", ITEMPOSITION_BUYBACK);
    lua_tinker::set(L, "ITEMPOSITION_EQUIP", ITEMPOSITION_EQUIP);
    lua_tinker::set(L, "ITEMPOSITION_EXCHANGE", ITEMPOSITION_EXCHANGE);
    lua_tinker::set(L, "ITEMPOSITION_GUILDSTORAGE", ITEMPOSITION_GUILDSTORAGE);
    lua_tinker::set(L, "ITEMPOSITION_MAILL", ITEMPOSITION_MAILL);
    lua_tinker::set(L, "ITEMPOSITION_MARKET", ITEMPOSITION_MARKET);
    lua_tinker::set(L, "ITEMPOSITION_STORAGE", ITEMPOSITION_STORAGE);
    lua_tinker::set(L, "ITEM_MAINTYPE_EQUIP", ITEM_MAINTYPE_EQUIP);
    lua_tinker::set(L, "ITEM_MAINTYPE_ITEM", ITEM_MAINTYPE_ITEM);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_ARMOR", ITEM_SUBTYPE_EQUIP_ARMOR);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_FASHION_DRESS", ITEM_SUBTYPE_EQUIP_FASHION_DRESS);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_FASHION_WEAPON", ITEM_SUBTYPE_EQUIP_FASHION_WEAPON);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_GIRDLE", ITEM_SUBTYPE_EQUIP_GIRDLE);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_HAND", ITEM_SUBTYPE_EQUIP_HAND);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_HELMET", ITEM_SUBTYPE_EQUIP_HELMET);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_HORSE", ITEM_SUBTYPE_EQUIP_HORSE);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_NECKLACE", ITEM_SUBTYPE_EQUIP_NECKLACE);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_RING", ITEM_SUBTYPE_EQUIP_RING);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_SHOES", ITEM_SUBTYPE_EQUIP_SHOES);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_WEAPON", ITEM_SUBTYPE_EQUIP_WEAPON);
    lua_tinker::set(L, "ITEM_SUBTYPE_EQUIP_WING", ITEM_SUBTYPE_EQUIP_WING);
    lua_tinker::set(L, "ITEM_SUBTYPE_ITEM_ADDITION", ITEM_SUBTYPE_ITEM_ADDITION);
    lua_tinker::set(L, "ITEM_SUBTYPE_ITEM_ADDITION_LUCKY", ITEM_SUBTYPE_ITEM_ADDITION_LUCKY);
    lua_tinker::set(L, "ITEM_SUBTYPE_ITEM_ADDITION_PROTECT", ITEM_SUBTYPE_ITEM_ADDITION_PROTECT);
    lua_tinker::set(L, "ITEM_SUBTYPE_ITEM_HORSE", ITEM_SUBTYPE_ITEM_HORSE);
    lua_tinker::set(L, "ITEM_SUBTYPE_ITEM_MEDICINE", ITEM_SUBTYPE_ITEM_MEDICINE);
    lua_tinker::set(L, "ITEM_SUBTYPE_ITEM_OTHER", ITEM_SUBTYPE_ITEM_OTHER);
    lua_tinker::set(L, "ITEM_SUBTYPE_ITEM_TASK", ITEM_SUBTYPE_ITEM_TASK);
    lua_tinker::set(L, "MARKET_SERVICE", MARKET_SERVICE);
    lua_tinker::set(L, "MULTITYPE_BROADCAST", MULTITYPE_BROADCAST);
    lua_tinker::set(L, "MULTITYPE_GROUPID", MULTITYPE_GROUPID);
    lua_tinker::set(L, "MULTITYPE_NONE", MULTITYPE_NONE);
    lua_tinker::set(L, "MULTITYPE_USERID", MULTITYPE_USERID);
    lua_tinker::set(L, "MULTITYPE_VIRTUALSOCKET", MULTITYPE_VIRTUALSOCKET);
    lua_tinker::set(L, "REGION_DEAD_NO_DROP", REGION_DEAD_NO_DROP);
    lua_tinker::set(L, "REGION_DROP_DISABLE", REGION_DROP_DISABLE);
    lua_tinker::set(L, "REGION_JUMP_DISABLE", REGION_JUMP_DISABLE);
    lua_tinker::set(L, "REGION_PASS_DISABLE", REGION_PASS_DISABLE);
    lua_tinker::set(L, "REGION_PLACE_DISABLE", REGION_PLACE_DISABLE);
    lua_tinker::set(L, "REGION_PVP_DISABLE", REGION_PVP_DISABLE);
    lua_tinker::set(L, "REGION_PVP_FREE", REGION_PVP_FREE);
    lua_tinker::set(L, "REGION_RECORD_DISABLE", REGION_RECORD_DISABLE);
    lua_tinker::set(L, "REGION_STALL_DISABLE", REGION_STALL_DISABLE);
    lua_tinker::set(L, "ROUTE_SERVICE", ROUTE_SERVICE);
    lua_tinker::set(L, "SCB_AI_FINDNEXTENEMY", SCB_AI_FINDNEXTENEMY);
    lua_tinker::set(L, "SCB_AI_ONUNDERATTACK", SCB_AI_ONUNDERATTACK);
    lua_tinker::set(L, "SCB_AI_PROCESS_ATTACK", SCB_AI_PROCESS_ATTACK);
    lua_tinker::set(L, "SCB_AI_SEARCHENEMY", SCB_AI_SEARCHENEMY);
    lua_tinker::set(L, "SCB_AI_TO_GOBACK", SCB_AI_TO_GOBACK);
    lua_tinker::set(L, "SCB_AI_TO_IDLE", SCB_AI_TO_IDLE);
    lua_tinker::set(L, "SCB_ITEM_ONBATCHUSECHECK", SCB_ITEM_ONBATCHUSECHECK);
    lua_tinker::set(L, "SCB_ITEM_ONTIMEOUT", SCB_ITEM_ONTIMEOUT);
    lua_tinker::set(L, "SCB_ITEM_ONUSE", SCB_ITEM_ONUSE);
    lua_tinker::set(L, "SCB_MAP_ONACTORBEKILL", SCB_MAP_ONACTORBEKILL);
    lua_tinker::set(L, "SCB_MAP_ONCREATE", SCB_MAP_ONCREATE);
    lua_tinker::set(L, "SCB_MAP_ONENTERMAP", SCB_MAP_ONENTERMAP);
    lua_tinker::set(L, "SCB_MAP_ONLEAVEMAP", SCB_MAP_ONLEAVEMAP);
    lua_tinker::set(L, "SCB_MONSTER_ONATTACK", SCB_MONSTER_ONATTACK);
    lua_tinker::set(L, "SCB_MONSTER_ONBEATTACK", SCB_MONSTER_ONBEATTACK);
    lua_tinker::set(L, "SCB_MONSTER_ONBEKILL", SCB_MONSTER_ONBEKILL);
    lua_tinker::set(L, "SCB_MONSTER_ONBORN", SCB_MONSTER_ONBORN);
    lua_tinker::set(L, "SCB_NPC_ONACTIVE", SCB_NPC_ONACTIVE);
    lua_tinker::set(L, "SCB_NPC_ONBORN", SCB_NPC_ONBORN);
    lua_tinker::set(L, "SCB_SKILL_DOAPPLY", SCB_SKILL_DOAPPLY);
    lua_tinker::set(L, "SCB_SKILL_DODAMAGE", SCB_SKILL_DODAMAGE);
    lua_tinker::set(L, "SCB_SKILL_DOINTONE", SCB_SKILL_DOINTONE);
    lua_tinker::set(L, "SCB_SKILL_DOLAUNCH", SCB_SKILL_DOLAUNCH);
    lua_tinker::set(L, "SCB_SKILL_DOSTUN", SCB_SKILL_DOSTUN);
    lua_tinker::set(L, "SCB_SKILL_ISTARGET", SCB_SKILL_ISTARGET);
    lua_tinker::set(L, "SCB_STATUS_ONATTACH", SCB_STATUS_ONATTACH);
    lua_tinker::set(L, "SCB_STATUS_ONATTACK", SCB_STATUS_ONATTACK);
    lua_tinker::set(L, "SCB_STATUS_ONBEATTACK", SCB_STATUS_ONBEATTACK);
    lua_tinker::set(L, "SCB_STATUS_ONDEAD", SCB_STATUS_ONDEAD);
    lua_tinker::set(L, "SCB_STATUS_ONDETACH", SCB_STATUS_ONDETACH);
    lua_tinker::set(L, "SCB_STATUS_ONEFFECT", SCB_STATUS_ONEFFECT);
    lua_tinker::set(L, "SCB_STATUS_ONLEAVEMAP", SCB_STATUS_ONLEAVEMAP);
    lua_tinker::set(L, "SCB_STATUS_ONLOGIN", SCB_STATUS_ONLOGIN);
    lua_tinker::set(L, "SCB_STATUS_ONLOGOUT", SCB_STATUS_ONLOGOUT);
    lua_tinker::set(L, "SCB_STATUS_ONMOVE", SCB_STATUS_ONMOVE);
    lua_tinker::set(L, "SCB_STATUS_ONSKILL", SCB_STATUS_ONSKILL);
    lua_tinker::set(L, "SCB_TASK_CAN_ACCEPT", SCB_TASK_CAN_ACCEPT);
    lua_tinker::set(L, "SCB_TASK_CAN_COMMIT", SCB_TASK_CAN_COMMIT);
    lua_tinker::set(L, "SCB_TASK_ON_ACCEPT", SCB_TASK_ON_ACCEPT);
    lua_tinker::set(L, "SCB_TASK_ON_COMMIT", SCB_TASK_ON_COMMIT);
    lua_tinker::set(L, "SCB_TASK_ON_GIVEUP", SCB_TASK_ON_GIVEUP);
    lua_tinker::set(L, "SCB_TASK_SHOW_TASKDIALOG", SCB_TASK_SHOW_TASKDIALOG);
    lua_tinker::set(L, "SCENESTATE_NORMAL", SCENESTATE_NORMAL);
    lua_tinker::set(L, "SCENESTATE_WAIT_DESTORY", SCENESTATE_WAIT_DESTORY);
    lua_tinker::set(L, "SCENESTATE_WAIT_LOADING", SCENESTATE_WAIT_LOADING);
    lua_tinker::set(L, "SCENE_SERVICE", SCENE_SERVICE);
    lua_tinker::set(L, "SCT_TARGET_OTHER", SCT_TARGET_OTHER);
    lua_tinker::set(L, "SCT_TARGET_POS", SCT_TARGET_POS);
    lua_tinker::set(L, "SCT_TARGET_SELF", SCT_TARGET_SELF);
    lua_tinker::set(L, "SKILLFLAG_AUTOLEARN", SKILLFLAG_AUTOLEARN);
    lua_tinker::set(L, "SKILLFLAG_AUTOUSE", SKILLFLAG_AUTOUSE);
    lua_tinker::set(L, "SKILLFLAG_BREAK_INTONE", SKILLFLAG_BREAK_INTONE);
    lua_tinker::set(L, "SKILLFLAG_BREAK_LAUNCH", SKILLFLAG_BREAK_LAUNCH);
    lua_tinker::set(L, "SKILLFLAG_CANCHANGE_DESTPOS", SKILLFLAG_CANCHANGE_DESTPOS);
    lua_tinker::set(L, "SKILLFLAG_CANNOT_REFLECT", SKILLFLAG_CANNOT_REFLECT);
    lua_tinker::set(L, "SKILLFLAG_CANNOT_RESILIENCE", SKILLFLAG_CANNOT_RESILIENCE);
    lua_tinker::set(L, "SKILLFLAG_DAMAGE_AFTER_LAUNCH", SKILLFLAG_DAMAGE_AFTER_LAUNCH);
    lua_tinker::set(L, "SKILLFLAG_DAMAGE_BEFORE_LAUNCH", SKILLFLAG_DAMAGE_BEFORE_LAUNCH);
    lua_tinker::set(L, "SKILLFLAG_DAMAGE_BY_DIST", SKILLFLAG_DAMAGE_BY_DIST);
    lua_tinker::set(L, "SKILLFLAG_IGNORE_DAMAGE", SKILLFLAG_IGNORE_DAMAGE);
    lua_tinker::set(L, "SKILLFLAG_IGNORE_DEFENCE", SKILLFLAG_IGNORE_DEFENCE);
    lua_tinker::set(L, "SKILLFLAG_IGNORE_HITRATE", SKILLFLAG_IGNORE_HITRATE);
    lua_tinker::set(L, "SKILLFLAG_INTONE_CANBREAK", SKILLFLAG_INTONE_CANBREAK);
    lua_tinker::set(L, "SKILLFLAG_INTONE_CANMOVE", SKILLFLAG_INTONE_CANMOVE);
    lua_tinker::set(L, "SKILLFLAG_INTONE_CANREINTONE", SKILLFLAG_INTONE_CANREINTONE);
    lua_tinker::set(L, "SKILLFLAG_LAUNCH_CANBREAK", SKILLFLAG_LAUNCH_CANBREAK);
    lua_tinker::set(L, "SKILLFLAG_LAUNCH_CANMOVE", SKILLFLAG_LAUNCH_CANMOVE);
    lua_tinker::set(L, "SKILLFLAG_MOVE_BREAK_INTONE", SKILLFLAG_MOVE_BREAK_INTONE);
    lua_tinker::set(L, "SKILLFLAG_NORMAL", SKILLFLAG_NORMAL);
    lua_tinker::set(L, "SKILLFLAG_NOT_UPDATEFIGHTING", SKILLFLAG_NOT_UPDATEFIGHTING);
    lua_tinker::set(L, "SKILLFLAG_SPENDITEM", SKILLFLAG_SPENDITEM);
    lua_tinker::set(L, "SKILLTYPE_ATTACK", SKILLTYPE_ATTACK);
    lua_tinker::set(L, "SKILLTYPE_BULLET", SKILLTYPE_BULLET);
    lua_tinker::set(L, "SKILLTYPE_CALLMON", SKILLTYPE_CALLMON);
    lua_tinker::set(L, "SKILLTYPE_NONE", SKILLTYPE_NONE);
    lua_tinker::set(L, "SKILLTYPE_PASSIVE", SKILLTYPE_PASSIVE);
    lua_tinker::set(L, "SKILLTYPE_RUSH", SKILLTYPE_RUSH);
    lua_tinker::set(L, "SKILLTYPE_TRANS", SKILLTYPE_TRANS);
    lua_tinker::set(L, "SOCKET_SERVICE", SOCKET_SERVICE);
    lua_tinker::set(L, "STATUSEXPIRETYPE_NEVER", STATUSEXPIRETYPE_NEVER);
    lua_tinker::set(L, "STATUSEXPIRETYPE_POINT", STATUSEXPIRETYPE_POINT);
    lua_tinker::set(L, "STATUSEXPIRETYPE_TIME", STATUSEXPIRETYPE_TIME);
    lua_tinker::set(L, "STATUSFLAG_ALLOW_PAUSE", STATUSFLAG_ALLOW_PAUSE);
    lua_tinker::set(L, "STATUSFLAG_BREAK_SKILL", STATUSFLAG_BREAK_SKILL);
    lua_tinker::set(L, "STATUSFLAG_DEATCH_ATTACK", STATUSFLAG_DEATCH_ATTACK);
    lua_tinker::set(L, "STATUSFLAG_DEATCH_BEATTACK", STATUSFLAG_DEATCH_BEATTACK);
    lua_tinker::set(L, "STATUSFLAG_DEATCH_LEAVEMAP", STATUSFLAG_DEATCH_LEAVEMAP);
    lua_tinker::set(L, "STATUSFLAG_DEATCH_MOVE", STATUSFLAG_DEATCH_MOVE);
    lua_tinker::set(L, "STATUSFLAG_DEATCH_OFFLINE", STATUSFLAG_DEATCH_OFFLINE);
    lua_tinker::set(L, "STATUSFLAG_DEATCH_SKILL", STATUSFLAG_DEATCH_SKILL);
    lua_tinker::set(L, "STATUSFLAG_DISABLE_ATTACK", STATUSFLAG_DISABLE_ATTACK);
    lua_tinker::set(L, "STATUSFLAG_DISABLE_BEATTACK", STATUSFLAG_DISABLE_BEATTACK);
    lua_tinker::set(L, "STATUSFLAG_DISABLE_MOVE", STATUSFLAG_DISABLE_MOVE);
    lua_tinker::set(L, "STATUSFLAG_EXCEPT_DEATCH_DEAD", STATUSFLAG_EXCEPT_DEATCH_DEAD);
    lua_tinker::set(L, "STATUSFLAG_NONE", STATUSFLAG_NONE);
    lua_tinker::set(L, "STATUSFLAG_OFFLINE_PAUSE", STATUSFLAG_OFFLINE_PAUSE);
    lua_tinker::set(L, "STATUSFLAG_ONLINE_RESUME", STATUSFLAG_ONLINE_RESUME);
    lua_tinker::set(L, "STATUSFLAG_OVERLAP", STATUSFLAG_OVERLAP);
    lua_tinker::set(L, "STATUSFLAG_OVERRIDE_LEV", STATUSFLAG_OVERRIDE_LEV);
    lua_tinker::set(L, "STATUSFLAG_PAUSE_ATTACH", STATUSFLAG_PAUSE_ATTACH);
    lua_tinker::set(L, "STATUSTYPE_COMP", STATUSTYPE_COMP);
    lua_tinker::set(L, "STATUSTYPE_CRIME", STATUSTYPE_CRIME);
    lua_tinker::set(L, "STATUSTYPE_DAMAGE", STATUSTYPE_DAMAGE);
    lua_tinker::set(L, "STATUSTYPE_DEAD", STATUSTYPE_DEAD);
    lua_tinker::set(L, "STATUSTYPE_HIDE", STATUSTYPE_HIDE);
    lua_tinker::set(L, "STATUSTYPE_NORMAL", STATUSTYPE_NORMAL);
    lua_tinker::set(L, "STATUSTYPE_TRANSFORM", STATUSTYPE_TRANSFORM);
    lua_tinker::set(L, "STF_TARGET_CUSTOM", STF_TARGET_CUSTOM);
    lua_tinker::set(L, "STF_TARGET_DEAD", STF_TARGET_DEAD);
    lua_tinker::set(L, "STF_TARGET_FAN", STF_TARGET_FAN);
    lua_tinker::set(L, "STF_TARGET_FRIEND", STF_TARGET_FRIEND);
    lua_tinker::set(L, "STF_TARGET_MONSTER", STF_TARGET_MONSTER);
    lua_tinker::set(L, "STF_TARGET_PET", STF_TARGET_PET);
    lua_tinker::set(L, "STF_TARGET_PLAYER", STF_TARGET_PLAYER);
    lua_tinker::set(L, "STF_TARGET_RECT", STF_TARGET_RECT);
    lua_tinker::set(L, "STF_TARGET_RECT_DIR", STF_TARGET_RECT_DIR);
    lua_tinker::set(L, "STF_TARGET_ROUND", STF_TARGET_ROUND);
    lua_tinker::set(L, "STF_TARGET_SINGLE", STF_TARGET_SINGLE);
    lua_tinker::set(L, "SYNC_ALL", SYNC_ALL);
    lua_tinker::set(L, "SYNC_ALL_DELAY", SYNC_ALL_DELAY);
    lua_tinker::set(L, "SYNC_FALSE", SYNC_FALSE);
    lua_tinker::set(L, "SYNC_TRUE", SYNC_TRUE);
    lua_tinker::set(L, "SYSTEMVAR_BEGIN", SYSTEMVAR_BEGIN);
    lua_tinker::set(L, "SYSTEMVAR_MERGE_END", SYSTEMVAR_MERGE_END);
    lua_tinker::set(L, "SYSTEMVAR_NOT_SAVE", SYSTEMVAR_NOT_SAVE);
    lua_tinker::set(L, "SYSTEMVAR_SERVER_START", SYSTEMVAR_SERVER_START);
    lua_tinker::set(L, "TASKFLAG_ACCEPT_ADDCOUNT", TASKFLAG_ACCEPT_ADDCOUNT);
    lua_tinker::set(L, "TASKFLAG_AUTO_ACITVE_NPC", TASKFLAG_AUTO_ACITVE_NPC);
    lua_tinker::set(L, "TASKFLAG_CAN_GIVEUP", TASKFLAG_CAN_GIVEUP);
    lua_tinker::set(L, "TASKFLAG_HIDE", TASKFLAG_HIDE);
    lua_tinker::set(L, "TASKFLAG_MSGSUBMIT", TASKFLAG_MSGSUBMIT);
    lua_tinker::set(L, "TASKFLAG_NEXTDAY_EXPIRE", TASKFLAG_NEXTDAY_EXPIRE);
    lua_tinker::set(L, "TASKFLAG_REPEATABLE", TASKFLAG_REPEATABLE);
    lua_tinker::set(L, "TASKFLAG_SUBMIT_ADDCOUNT", TASKFLAG_SUBMIT_ADDCOUNT);
    lua_tinker::set(L, "TASKFLAG_SUBMIT_DELITEM", TASKFLAG_SUBMIT_DELITEM);
    lua_tinker::set(L, "UNKNOW_SERVICE", UNKNOW_SERVICE);
    lua_tinker::set(L, "WORLD_SERVICE", WORLD_SERVICE);
    lua_tinker::class_inh<CBullet, CActor>(L);
    lua_tinker::class_inh<CCoolDown, ICoolDown>(L);
    lua_tinker::class_inh<CDynaMonster, CMonster>(L);
    lua_tinker::class_inh<CDynaNpc, CNpc>(L);
    lua_tinker::class_inh<CItem, CItemData>(L);
    lua_tinker::class_inh<CMapItem, CActor>(L);
    lua_tinker::class_inh<CMonster, CActor>(L);
    lua_tinker::class_inh<CNpc, CActor>(L);
    lua_tinker::class_inh<CPet, CActor>(L);
    lua_tinker::class_inh<CPlayer, CActor>(L);
    lua_tinker::class_inh<CPlayerCoolDown, ICoolDown>(L);
    lua_tinker::class_inh<CPlayerCoolDownSet, CCoolDownSet>(L);
    lua_tinker::class_inh<CSceneService, CServiceCommon>(L);
    lua_tinker::class_inh<CStoragePackage, CPackage>(L);
    lua_tinker::class_inh<ST_STATUS_INFO, AttachStatusInfo>(L);
    lua_tinker::class_alias<Vector2, CPos2D>(L, "CPos2D");
    lua_tinker::class_alias<Vector3, CPos3D>(L, "CPos3D");
}