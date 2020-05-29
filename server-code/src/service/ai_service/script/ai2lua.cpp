#include "AIActor.h"
#include "AIActorManager.h"
#include "AIFuzzyLogic.h"
#include "AIGroup.h"
#include "AIMessageHandler.h"
#include "AIMonster.h"
#include "AIPathFinder.h"
#include "AIPhase.h"
#include "AIPlayer.h"
#include "AIScene.h"
#include "AISceneManagr.h"
#include "AIService.h"
#include "AISkill.h"
#include "AIType.h"
#include "ActorAI.h"
#include "MonsterGenerator.h"
#include "MonsterType.h"
#include "SkillType.h"
#include "lua_tinker.h"
#include "rule.h"
#include "rule_set.h"
#include "trapezoid.h"
void ai2lua(lua_State* L)
{
    lua_tinker::class_add<Angle>(L, "Angle", false);
    lua_tinker::class_add<AxisAlignedBox>(L, "AxisAlignedBox", false);
    lua_tinker::class_def_static<AxisAlignedBox>(L, "BOX_INFINITE", &AxisAlignedBox::BOX_INFINITE);
    lua_tinker::class_def_static<AxisAlignedBox>(L, "BOX_NULL", &AxisAlignedBox::BOX_NULL);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "contains",
        lua_tinker::args_type_overload_member_functor(lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(
                                                          const AxisAlignedBox&) const)(&AxisAlignedBox::contains)),
                                                      lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(
                                                          const Vector3&) const)(&AxisAlignedBox::contains))));
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
            lua_tinker::make_member_functor_ptr(
                (const Vector3& (AxisAlignedBox::*)() const)(&AxisAlignedBox::getMaximum))));
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "getMinimum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((Vector3 & (AxisAlignedBox::*)())(&AxisAlignedBox::getMinimum)),
            lua_tinker::make_member_functor_ptr(
                (const Vector3& (AxisAlignedBox::*)() const)(&AxisAlignedBox::getMinimum))));
    lua_tinker::class_def<AxisAlignedBox>(L, "getSize", &AxisAlignedBox::getSize);
    lua_tinker::class_def<AxisAlignedBox>(L, "intersection", &AxisAlignedBox::intersection);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "intersects",
        lua_tinker::args_type_overload_member_functor(lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(
                                                          const AxisAlignedBox&) const)(&AxisAlignedBox::intersects)),
                                                      lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(
                                                          const Vector3&) const)(&AxisAlignedBox::intersects))));
    lua_tinker::class_def<AxisAlignedBox>(L, "isFinite", &AxisAlignedBox::isFinite);
    lua_tinker::class_def<AxisAlignedBox>(L, "isInfinite", &AxisAlignedBox::isInfinite);
    lua_tinker::class_def<AxisAlignedBox>(L, "isNull", &AxisAlignedBox::isNull);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "merge",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr(
                (void (AxisAlignedBox::*)(const AxisAlignedBox&))(&AxisAlignedBox::merge)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Vector3&))(&AxisAlignedBox::merge))));
    lua_tinker::class_def<AxisAlignedBox>(L, "operator!=", &AxisAlignedBox::operator!=);
    lua_tinker::class_def<AxisAlignedBox>(L, "__eq", &AxisAlignedBox::operator==);
    lua_tinker::class_def<AxisAlignedBox>(L, "scale", &AxisAlignedBox::scale);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "setExtents",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr(
                (void (AxisAlignedBox::*)(const Vector3&, const Vector3&))(&AxisAlignedBox::setExtents)),
            lua_tinker::make_member_functor_ptr(
                (void (AxisAlignedBox::*)(float, float, float, float, float, float))(&AxisAlignedBox::setExtents))));
    lua_tinker::class_def<AxisAlignedBox>(L, "setInfinite", &AxisAlignedBox::setInfinite);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "setMaximum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr(
                (void (AxisAlignedBox::*)(const Vector3&))(&AxisAlignedBox::setMaximum)),
            lua_tinker::make_member_functor_ptr(
                (void (AxisAlignedBox::*)(float, float, float))(&AxisAlignedBox::setMaximum))));
    lua_tinker::class_def<AxisAlignedBox>(L, "setMaximumX", &AxisAlignedBox::setMaximumX);
    lua_tinker::class_def<AxisAlignedBox>(L, "setMaximumY", &AxisAlignedBox::setMaximumY);
    lua_tinker::class_def<AxisAlignedBox>(L, "setMaximumZ", &AxisAlignedBox::setMaximumZ);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "setMinimum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr(
                (void (AxisAlignedBox::*)(const Vector3&))(&AxisAlignedBox::setMinimum)),
            lua_tinker::make_member_functor_ptr(
                (void (AxisAlignedBox::*)(float, float, float))(&AxisAlignedBox::setMinimum))));
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
            lua_tinker::make_member_functor_ptr(
                (void (AxisAlignedBox::*)(const Matrix4&))(&AxisAlignedBox::transform))));
    lua_tinker::class_def<AxisAlignedBox>(L, "volume", &AxisAlignedBox::volume);
    lua_tinker::class_con<AxisAlignedBox>(
        L,
        lua_tinker::args_type_overload_constructor(
            new lua_tinker::constructor<AxisAlignedBox>(),
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
    lua_tinker::class_add<CAIGroup>(L, "CAIGroup", false);
    lua_tinker::class_def<CAIGroup>(L, "AddMember", &CAIGroup::AddMember);
    lua_tinker::class_def<CAIGroup>(L, "FindIF", &CAIGroup::FindIF);
    lua_tinker::class_def<CAIGroup>(L, "Foreach", &CAIGroup::Foreach);
    lua_tinker::class_def<CAIGroup>(L, "RemoveMember", &CAIGroup::RemoveMember);
    lua_tinker::class_add<CAIGroupManager>(L, "CAIGroupManager", false);
    lua_tinker::class_def<CAIGroupManager>(L, "GetGroup", &CAIGroupManager::GetGroup);
    lua_tinker::class_add<CAIPhase>(L, "CAIPhase", false);
    lua_tinker::class_def<CAIPhase>(L, "GetAIGroupMgr", &CAIPhase::GetAIGroupMgr);
    lua_tinker::class_def<CAIPhase>(L, "GetPhaseID", &CAIPhase::GetPhaseID);
    lua_tinker::class_add<CAIScene>(L, "CAIScene", false);
    lua_tinker::class_def<CAIScene>(
        L,
        "CreatePhase",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr(
                (CAIPhase * (CAIScene::*)(const SceneIdx&, uint64_t))(&CAIScene::CreatePhase)),
            lua_tinker::make_member_functor_ptr(
                (CAIPhase * (CAIScene::*)(const SceneIdx&, uint64_t, const PhaseData*))(&CAIScene::CreatePhase))));
    lua_tinker::class_def<CAIScene>(L, "DestoryPhase", &CAIScene::DestoryPhase);
    lua_tinker::class_def<CAIScene>(L, "DestoryPhaseByIdx", &CAIScene::DestoryPhaseByIdx);
    lua_tinker::class_def<CAIScene>(L, "QueryPhaseByID", &CAIScene::QueryPhaseByID);
    lua_tinker::class_def<CAIScene>(L, "QueryPhaseByIdx", &CAIScene::QueryPhaseByIdx);
    lua_tinker::class_add<CActorAI>(L, "CActorAI", false);
    lua_tinker::class_def<CActorAI>(L, "AddHate", &CActorAI::AddHate);
    lua_tinker::class_def<CActorAI>(L, "ChangeState", &CActorAI::ChangeState);
    lua_tinker::class_def<CActorAI>(L, "ClearHateList", &CActorAI::ClearHateList);
    lua_tinker::class_def<CActorAI>(L, "FindEnemyInHateList", &CActorAI::FindEnemyInHateList);
    lua_tinker::class_def<CActorAI>(L, "FindNextEnemy", &CActorAI::FindNextEnemy);
    lua_tinker::class_def<CActorAI>(L, "ForEachInHateList", &CActorAI::ForEachInHateList);
    lua_tinker::class_def<CActorAI>(L, "GetAIGroup", &CActorAI::GetAIGroup);
    lua_tinker::class_def<CActorAI>(L, "GetAIType", &CActorAI::GetAIType);
    lua_tinker::class_def<CActorAI>(L, "GetActor", &CActorAI::GetActor);
    lua_tinker::class_def<CActorAI>(L, "GetCurSkillTypeID", &CActorAI::GetCurSkillTypeID);
    lua_tinker::class_def<CActorAI>(L, "GetMainTarget", &CActorAI::GetMainTarget);
    lua_tinker::class_def<CActorAI>(L, "GetState", &CActorAI::GetState);
    lua_tinker::class_def<CActorAI>(L, "OrderAttack", &CActorAI::OrderAttack);
    lua_tinker::class_def<CActorAI>(L, "PathFind", &CActorAI::PathFind);
    lua_tinker::class_def<CActorAI>(L, "SearchEnemy", &CActorAI::SearchEnemy);
    lua_tinker::class_def<CActorAI>(L, "SetAISleep", &CActorAI::SetAISleep);
    lua_tinker::class_def<CActorAI>(L, "SetCurSkillTypeID", &CActorAI::SetCurSkillTypeID);
    lua_tinker::class_def<CActorAI>(L, "SetMainTarget", &CActorAI::SetMainTarget);
    lua_tinker::class_def<CActorAI>(L, "ToApproach", &CActorAI::ToApproach);
    lua_tinker::class_def<CActorAI>(L, "ToAttack", &CActorAI::ToAttack);
    lua_tinker::class_def<CActorAI>(L, "ToEscape", &CActorAI::ToEscape);
    lua_tinker::class_def<CActorAI>(L, "ToGoBack", &CActorAI::ToGoBack);
    lua_tinker::class_def<CActorAI>(L, "ToIdle", &CActorAI::ToIdle);
    lua_tinker::class_def<CActorAI>(L, "ToPatrolWait", &CActorAI::ToPatrolWait);
    lua_tinker::class_def<CActorAI>(L, "ToPratol", &CActorAI::ToPratol);
    lua_tinker::class_def<CActorAI>(L, "ToRandMove", &CActorAI::ToRandMove);
    lua_tinker::class_def<CActorAI>(L, "ToSkill", &CActorAI::ToSkill);
    lua_tinker::class_def<CActorAI>(L, "ToSkillFinish", &CActorAI::ToSkillFinish);
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
    lua_tinker::class_add<CMapManager>(L, "CMapManager", false);
    lua_tinker::class_def<CMapManager>(L, "ForEach", &CMapManager::ForEach);
    lua_tinker::class_def<CMapManager>(L, "QueryMap", &CMapManager::QueryMap);
    lua_tinker::class_def<CMapManager>(L, "QueryMapData", &CMapManager::QueryMapData);
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
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)())(&CMyTimer::IsTimeOut)),
            lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)(int32_t))(&CMyTimer::IsTimeOut))));
    lua_tinker::class_def<CMyTimer>(L, "SetInterval", &CMyTimer::SetInterval);
    lua_tinker::class_def<CMyTimer>(L, "Startup", &CMyTimer::Startup);
    lua_tinker::class_def<CMyTimer>(L, "TimeOver", &CMyTimer::TimeOver);
    lua_tinker::class_def<CMyTimer>(L, "ToNextTick", &CMyTimer::ToNextTick);
    lua_tinker::class_def<CMyTimer>(
        L,
        "ToNextTime",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)())(&CMyTimer::ToNextTime)),
            lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)(int32_t))(&CMyTimer::ToNextTime))));
    lua_tinker::class_def<CMyTimer>(L, "Update", &CMyTimer::Update);
    lua_tinker::class_con<CMyTimer>(L, lua_tinker::constructor<CMyTimer, int32_t>::invoke, 0);
    lua_tinker::class_add<CNetworkMessage>(L, "CNetworkMessage", false);
    lua_tinker::class_def<CNetworkMessage>(L, "CopyBuffer", &CNetworkMessage::CopyBuffer);
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
            lua_tinker::make_member_functor_ptr(
                (const byte* (CNetworkMessage::*)() const)(&CNetworkMessage::GetMsgBody))));
    lua_tinker::class_def<CNetworkMessage>(
        L,
        "GetMsgHead",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((MSG_HEAD * (CNetworkMessage::*)())(&CNetworkMessage::GetMsgHead)),
            lua_tinker::make_member_functor_ptr(
                (const MSG_HEAD* (CNetworkMessage::*)() const)(&CNetworkMessage::GetMsgHead))));
    lua_tinker::class_def<CNetworkMessage>(L, "GetMultiIDTo", &CNetworkMessage::GetMultiIDTo);
    lua_tinker::class_def<CNetworkMessage>(L, "GetMultiTo", &CNetworkMessage::GetMultiTo);
    lua_tinker::class_def<CNetworkMessage>(L, "GetMultiType", &CNetworkMessage::GetMultiType);
    lua_tinker::class_def<CNetworkMessage>(L, "GetSize", &CNetworkMessage::GetSize);
    lua_tinker::class_def<CNetworkMessage>(L, "GetTo", &CNetworkMessage::GetTo);
    lua_tinker::class_def<CNetworkMessage>(L, "IsBroadcast", &CNetworkMessage::IsBroadcast);
    lua_tinker::class_def<CNetworkMessage>(L, "IsMultiIDTo", &CNetworkMessage::IsMultiIDTo);
    lua_tinker::class_def<CNetworkMessage>(L, "IsMultiTo", &CNetworkMessage::IsMultiTo);
    lua_tinker::class_def<CNetworkMessage>(L, "SetBroadcast", &CNetworkMessage::SetBroadcast);
    lua_tinker::class_def<CNetworkMessage>(L, "SetForward", &CNetworkMessage::SetForward);
    lua_tinker::class_def<CNetworkMessage>(L, "SetFrom", &CNetworkMessage::SetFrom);
    lua_tinker::class_def<CNetworkMessage>(
        L,
        "SetMultiIDTo",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr(
                (void (CNetworkMessage::*)(const OBJID*, size_t))(&CNetworkMessage::SetMultiIDTo)),
            lua_tinker::make_member_functor_ptr(
                (void (CNetworkMessage::*)(const std::vector<OBJID>&))(&CNetworkMessage::SetMultiIDTo))));
    lua_tinker::class_def<CNetworkMessage>(
        L,
        "SetMultiTo",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr(
                (void (CNetworkMessage::*)(const std::vector<VirtualSocket>&))(&CNetworkMessage::SetMultiTo)),
            lua_tinker::make_member_functor_ptr(
                (void (CNetworkMessage::*)(const uint64_t*, size_t))(&CNetworkMessage::SetMultiTo))));
    lua_tinker::class_def<CNetworkMessage>(L, "SetMultiType", &CNetworkMessage::SetMultiType);
    lua_tinker::class_def<CNetworkMessage>(L, "SetTo", &CNetworkMessage::SetTo);
    lua_tinker::class_con<CNetworkMessage>(
        L,
        lua_tinker::args_type_overload_constructor(
            new lua_tinker::constructor<CNetworkMessage, byte*, size_t, VirtualSocket, VirtualSocket, VirtualSocket>(
                3 /*default_args_count*/,
                1 /*default_args_start*/),
            new lua_tinker::constructor<CNetworkMessage, const CNetworkMessage&>(),
            new lua_tinker::
                constructor<CNetworkMessage, uint16_t, byte*, size_t, VirtualSocket, VirtualSocket, VirtualSocket>(
                    3 /*default_args_count*/,
                    4 /*default_args_start*/),
            new lua_tinker::constructor<CNetworkMessage,
                                        uint16_t,
                                        const ::google::protobuf::Message&,
                                        VirtualSocket,
                                        VirtualSocket,
                                        VirtualSocket>(3 /*default_args_count*/, 7 /*default_args_start*/)),
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0);
    lua_tinker::class_add<CServiceCommon>(L, "CServiceCommon", false);
    lua_tinker::class_def<CServiceCommon>(L, "CreateUID", &CServiceCommon::CreateUID);
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
    lua_tinker::class_add<Degree>(L, "Degree", false);
    lua_tinker::class_def<Degree>(L, "operator!=", &Degree::operator!=);
    lua_tinker::class_def<Degree>(
        L,
        "__mul",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Degree&) const)(&Degree::operator*)),
            lua_tinker::make_member_functor_ptr((Degree(Degree::*)(float) const)(&Degree::operator*))));
    lua_tinker::class_def<Degree>(
        L,
        "__add",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Degree&) const)(&Degree::operator+)),
            lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Radian&) const)(&Degree::operator+)),
            lua_tinker::make_member_functor_ptr((const Degree& (Degree::*)() const)(&Degree::operator+))));
    lua_tinker::class_def<Degree>(
        L,
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
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((Degree & (Degree::*)(const Degree&))(&Degree::operator=)),
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
        lua_tinker::args_type_overload_constructor(
            new lua_tinker::constructor<Degree, const Degree&>(),
            new lua_tinker::constructor<Degree, const Radian&>(),
            new lua_tinker::constructor<Degree, float>(1 /*default_args_count*/, 1 /*default_args_start*/)),
        0);
    lua_tinker::class_add<FloatRect>(L, "FloatRect", false);
    lua_tinker::class_def<FloatRect>(L, "height", &FloatRect::height);
    lua_tinker::class_def<FloatRect>(L, "isIntersect", &FloatRect::isIntersect);
    lua_tinker::class_def<FloatRect>(L, "width", &FloatRect::width);
    lua_tinker::class_con<FloatRect>(L,
                                     lua_tinker::args_type_overload_constructor(
                                         new lua_tinker::constructor<FloatRect>(),
                                         new lua_tinker::constructor<FloatRect, const FloatRect&>(),
                                         new lua_tinker::constructor<FloatRect, float, float, float, float>()));
    lua_tinker::class_mem<FloatRect>(L, "bottom", &FloatRect::bottom);
    lua_tinker::class_mem<FloatRect>(L, "left", &FloatRect::left);
    lua_tinker::class_mem<FloatRect>(L, "right", &FloatRect::right);
    lua_tinker::class_mem<FloatRect>(L, "top", &FloatRect::top);
    lua_tinker::class_add<MapGridData>(L, "MapGridData", false);
    lua_tinker::class_con<MapGridData>(L, lua_tinker::constructor<MapGridData, uint32_t>::invoke, 0);
    lua_tinker::class_add<Math>(L, "Math", false);
    lua_tinker::class_def_static<Math>(L, "ACos", &Math::ACos);
    lua_tinker::class_def_static<Math>(L, "ASin", &Math::ASin);
    lua_tinker::class_def_static<Math>(L, "ATan", &Math::ATan);
    lua_tinker::class_def_static<Math>(L, "ATan2", &Math::ATan2);
    lua_tinker::class_def_static<Math>(
        L,
        "Abs",
        lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((Degree(*)(const Degree&))(&Math::Abs)),
                                               lua_tinker::make_functor_ptr((Radian(*)(const Radian&))(&Math::Abs)),
                                               lua_tinker::make_functor_ptr((float (*)(float))(&Math::Abs))));
    lua_tinker::class_def_static<Math>(L, "AngleUnitsToDegrees", &Math::AngleUnitsToDegrees);
    lua_tinker::class_def_static<Math>(L, "AngleUnitsToRadians", &Math::AngleUnitsToRadians);
    lua_tinker::class_def_static<Math>(L, "Ceil", &Math::Ceil);
    lua_tinker::class_def_static<Math>(L,
                                       "Cos",
                                       lua_tinker::args_type_overload_functor(
                                           lua_tinker::make_functor_ptr((float (*)(const Radian&, bool))(&Math::Cos),
                                                                        1 /*default_args_count*/,
                                                                        1 /*default_args_start*/),
                                           lua_tinker::make_functor_ptr((float (*)(float, bool))(&Math::Cos),
                                                                        1 /*default_args_count*/,
                                                                        2 /*default_args_start*/)),
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
    lua_tinker::class_def_static<Math>(
        L,
        "Sign",
        lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((Degree(*)(const Degree&))(&Math::Sign)),
                                               lua_tinker::make_functor_ptr((Radian(*)(const Radian&))(&Math::Sign)),
                                               lua_tinker::make_functor_ptr((float (*)(float))(&Math::Sign))));
    lua_tinker::class_def_static<Math>(L,
                                       "Sin",
                                       lua_tinker::args_type_overload_functor(
                                           lua_tinker::make_functor_ptr((float (*)(const Radian&, bool))(&Math::Sin),
                                                                        1 /*default_args_count*/,
                                                                        1 /*default_args_start*/),
                                           lua_tinker::make_functor_ptr((float (*)(float, bool))(&Math::Sin),
                                                                        1 /*default_args_count*/,
                                                                        2 /*default_args_start*/)),
                                       false,
                                       false);
    lua_tinker::class_def_static<Math>(L, "Sqr", &Math::Sqr);
    lua_tinker::class_def_static<Math>(
        L,
        "Sqrt",
        lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((Degree(*)(const Degree&))(&Math::Sqrt)),
                                               lua_tinker::make_functor_ptr((Radian(*)(const Radian&))(&Math::Sqrt)),
                                               lua_tinker::make_functor_ptr((float (*)(float))(&Math::Sqrt))));
    lua_tinker::class_def_static<Math>(L,
                                       "Tan",
                                       lua_tinker::args_type_overload_functor(
                                           lua_tinker::make_functor_ptr((float (*)(const Radian&, bool))(&Math::Tan),
                                                                        1 /*default_args_count*/,
                                                                        1 /*default_args_start*/),
                                           lua_tinker::make_functor_ptr((float (*)(float, bool))(&Math::Tan),
                                                                        1 /*default_args_count*/,
                                                                        2 /*default_args_start*/)),
                                       false,
                                       false);
    lua_tinker::class_def_static<Math>(L, "get2Pow", &Math::get2Pow);
    lua_tinker::class_def_static<Math>(L, "getAngleUnit", &Math::getAngleUnit);
    lua_tinker::class_def_static<Math>(L, "ilog2", &Math::ilog2);
    lua_tinker::class_def_static<Math>(L, "is2Pow", &Math::is2Pow);
    lua_tinker::class_def_static<Math>(L, "isNaN", &Math::isNaN);
    lua_tinker::class_def_static<Math>(L, "nextPow2", &Math::nextPow2);
    lua_tinker::class_def_static<Math>(
        L,
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
    lua_tinker::class_con<Ray>(
        L,
        lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Ray>(),
                                                   new lua_tinker::constructor<Ray, const Vector3&, const Vector3&>()));
    lua_tinker::class_add<Rect>(L, "Rect", false);
    lua_tinker::class_def<Rect>(L, "height", &Rect::height);
    lua_tinker::class_def<Rect>(L, "isIntersect", &Rect::isIntersect);
    lua_tinker::class_def<Rect>(L, "operator=", &Rect::operator=);
    lua_tinker::class_def<Rect>(L, "width", &Rect::width);
    lua_tinker::class_con<Rect>(
        L,
        lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Rect>(),
                                                   new lua_tinker::constructor<Rect, const Rect&>(),
                                                   new lua_tinker::constructor<Rect, long, long, long, long>()));
    lua_tinker::class_mem<Rect>(L, "bottom", &Rect::bottom);
    lua_tinker::class_mem<Rect>(L, "left", &Rect::left);
    lua_tinker::class_mem<Rect>(L, "right", &Rect::right);
    lua_tinker::class_mem<Rect>(L, "top", &Rect::top);
    lua_tinker::class_add<SceneIdx>(L, "SceneIdx", false);
    lua_tinker::class_def<SceneIdx>(L, "GetMapID", &SceneIdx::GetMapID);
    lua_tinker::class_def<SceneIdx>(L, "GetPhaseIdx", &SceneIdx::GetPhaseIdx);
    lua_tinker::class_def<SceneIdx>(L, "GetZoneID", &SceneIdx::GetZoneID);
    lua_tinker::class_def<SceneIdx>(L, "__lt", &SceneIdx::operator<);
    lua_tinker::class_def<SceneIdx>(L, "__eq", &SceneIdx::operator==);
    lua_tinker::class_con<SceneIdx>(
        L,
        lua_tinker::args_type_overload_constructor(
            new lua_tinker::constructor<SceneIdx, const SceneIdx&>(),
            new lua_tinker::constructor<SceneIdx, uint16_t, uint16_t, uint32_t>(),
            new lua_tinker::constructor<SceneIdx, uint64_t>(1 /*default_args_count*/, 1 /*default_args_start*/)),
        0);
    lua_tinker::class_add<ServerPort>(L, "ServerPort", false);
    lua_tinker::class_def<ServerPort>(L, "GetData", &ServerPort::GetData);
    lua_tinker::class_def<ServerPort>(L, "GetServiceID", &ServerPort::GetServiceID);
    lua_tinker::class_def<ServerPort>(L, "GetWorldID", &ServerPort::GetWorldID);
    lua_tinker::class_def<ServerPort>(L, "IsVaild", &ServerPort::IsVaild);
    lua_tinker::class_def<ServerPort>(L, "SetData", &ServerPort::SetData);
    lua_tinker::class_def<ServerPort>(L, "SetServiceID", &ServerPort::SetServiceID);
    lua_tinker::class_def<ServerPort>(L, "SetWorldID", &ServerPort::SetWorldID);
    lua_tinker::class_def<ServerPort>(L, "__lt", &ServerPort::operator<);
    lua_tinker::class_def<ServerPort>(L, "__eq", &ServerPort::operator==);
    lua_tinker::class_con<ServerPort>(
        L,
        lua_tinker::args_type_overload_constructor(
            new lua_tinker::constructor<ServerPort, const ServerPort&>(),
            new lua_tinker::constructor<ServerPort, uint16_t, uint16_t>(),
            new lua_tinker::constructor<ServerPort, uint32_t>(1 /*default_args_count*/, 1 /*default_args_start*/)),
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
    lua_tinker::class_con<Vector2>(
        L,
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
    lua_tinker::class_con<Vector3>(L,
                                   lua_tinker::args_type_overload_constructor(
                                       new lua_tinker::constructor<Vector3>(),
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
    lua_tinker::namespace_def(L,
                              "GameMath",
                              "getDirectAngleFrom",
                              &GameMath::getDirectAngleFrom,
                              GameMath::VECTOR2_NORTH,
                              true);
    lua_tinker::namespace_def(L,
                              "GameMath",
                              "getDirectRadianFrom",
                              &GameMath::getDirectRadianFrom,
                              GameMath::VECTOR2_NORTH,
                              true);
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
    lua_tinker::namespace_def(L,
                              "GameMath::Intersection2D",
                              "LineIntersection2D",
                              &GameMath::Intersection2D::LineIntersection2D);
    lua_tinker::namespace_def(L,
                              "GameMath::Intersection2D",
                              "getNearestPoint",
                              &GameMath::Intersection2D::getNearestPoint);
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "isInABBox", &GameMath::Intersection2D::isInABBox);
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "isInFOV", &GameMath::Intersection2D::isInFOV);
    lua_tinker::namespace_def(L,
                              "GameMath::Intersection2D",
                              "lineIntersection2D",
                              &GameMath::Intersection2D::lineIntersection2D);
    lua_tinker::scope_inner(L, "GameMath", "Intersection2D", "GameMath::Intersection2D");
    lua_tinker::def(L, "CheckSameDay", &CheckSameDay);
    lua_tinker::def(L, "ConvertEnc", &ConvertEnc);
    lua_tinker::def(L, "DayDiffLocal", &DayDiffLocal);
    lua_tinker::def(L,
                    "FindNameError",
                    lua_tinker::args_type_overload_functor(
                        lua_tinker::make_functor_ptr((bool (*)(const std::string&))(&FindNameError)),
                        lua_tinker::make_functor_ptr((bool (*)(const std::wstring&))(&FindNameError))));
    lua_tinker::def(L, "GetBasePath", &GetBasePath);
    lua_tinker::def(L, "GetFileExt", &GetFileExt);
    lua_tinker::def(L, "GetFileNameFromFullPath", &GetFileNameFromFullPath);
    lua_tinker::def(L, "GetFileNameWithoutExt", &GetFileNameWithoutExt);
    lua_tinker::def(L, "GetFullPath", &GetFullPath);
    lua_tinker::def(L, "GetHighFromU64", &GetHighFromU64);
    lua_tinker::def(L, "GetLowFromU64", &GetLowFromU64);
    lua_tinker::def(L, "GetNextDayBeginTime", &GetNextDayBeginTime);
    lua_tinker::def(L, "GetServiceName", &GetServiceName);
    lua_tinker::def(L, "GetTimeFromString", &GetTimeFromString);
    lua_tinker::def(L, "HasFlag", &HasFlag);
    lua_tinker::def(L, "IsUTF8_NoneControl", &IsUTF8_NoneControl);
    lua_tinker::def(L, "MAKE32", &MAKE32);
    lua_tinker::def(L, "MAKE64", &MAKE64);
    lua_tinker::def(L, "MakeINT64", &MakeINT64);
    lua_tinker::def(L, "MakeUINT64", &MakeUINT64);
    lua_tinker::def(L, "MonthDiffLocal", &MonthDiffLocal);
    lua_tinker::def(L, "MulDiv", &MulDiv);
    lua_tinker::def(L, "MulDivSign", &MulDivSign);
    lua_tinker::def(L, "NextDayBeginTimeStamp", &NextDayBeginTimeStamp);
    lua_tinker::def(L, "NextMonthBeginTimeStamp", &NextMonthBeginTimeStamp);
    lua_tinker::def(L, "NextWeekBeginTimeStamp", &NextWeekBeginTimeStamp);
    lua_tinker::def(L, "RegexStrCheck", &RegexStrCheck);
    lua_tinker::def(L, "RegexStrReload", &RegexStrReload);
    lua_tinker::def(L,
                    "ReplaceIllegaWords",
                    lua_tinker::args_type_overload_functor(
                        lua_tinker::make_functor_ptr((bool (*)(std::string&))(&ReplaceIllegaWords)),
                        lua_tinker::make_functor_ptr((bool (*)(std::wstring&))(&ReplaceIllegaWords))));
    lua_tinker::def(L, "ReplaceStr", &ReplaceStr);
    lua_tinker::def(L, "TimeGetMillisecond", &TimeGetMillisecond);
    lua_tinker::def(L, "TimeGetMonotonic", &TimeGetMonotonic);
    lua_tinker::def(L, "TimeGetSecond", &TimeGetSecond);
    lua_tinker::def(L, "TimeGetSecondLocal", &TimeGetSecondLocal);
    lua_tinker::def(L, "TrimPath", &TrimPath);
    lua_tinker::def(L, "URLDecode", &URLDecode);
    lua_tinker::def(L, "URLEncode", &URLEncode);
    lua_tinker::def(L, "WeekDiffLocal", &WeekDiffLocal);
    lua_tinker::def(L, "_TimeGetMillisecond", &_TimeGetMillisecond);
    lua_tinker::def(L, "_TimeGetMonotonic", &_TimeGetMonotonic);
    lua_tinker::def(L, "_TimeGetSecond", &_TimeGetSecond);
    lua_tinker::def(L, "_TimeGetSecondLocal", &_TimeGetSecondLocal);
    lua_tinker::def(L, "bit_flip", &bit_flip);
    lua_tinker::def(L, "bit_set", &bit_set);
    lua_tinker::def(L, "bit_test", &bit_test);
    lua_tinker::def(L, "gmt2local", &gmt2local);
    lua_tinker::def(L, "hex_get", &hex_get);
    lua_tinker::def(L, "hex_set", &hex_set);
    lua_tinker::def(L, "isleap", &isleap);
    lua_tinker::def(L, "local2gmt", &local2gmt);
    lua_tinker::def(L, "lower_cast", &lower_cast);
    lua_tinker::def(L, "lower_cast_copy", &lower_cast_copy);
    lua_tinker::def(L, "ltrim", &ltrim);
    lua_tinker::def(L, "ltrim_copy", &ltrim_copy);
    lua_tinker::def(L, "random_float", &random_float, 0.0f, 1.0f);
    lua_tinker::def(L, "random_hit", &random_hit);
    lua_tinker::def(L, "random_uint32", &random_uint32);
    lua_tinker::def(L, "random_uint32_range", &random_uint32_range);
    lua_tinker::def(L, "rtrim", &rtrim);
    lua_tinker::def(L, "rtrim_copy", &rtrim_copy);
    lua_tinker::def(L, "skip_utf8_bom", &skip_utf8_bom);
    lua_tinker::def(L, "split_string", &split_string);
    lua_tinker::def(L, "split_string_view", &split_string_view);
    lua_tinker::def(L, "toHex", &toHex);
    lua_tinker::def(L, "trim", &trim);
    lua_tinker::def(L, "trim_copy", &trim_copy);
    lua_tinker::def(L, "upper_cast", &upper_cast);
    lua_tinker::def(L, "upper_cast_copy", &upper_cast_copy);
    lua_tinker::def(L, "utf8_length", &utf8_length, 0);
    lua_tinker::set(L, "AI_SERICE_COUNT", AI_SERICE_COUNT);
    lua_tinker::set(L, "UPDATE_FALSE", UPDATE_FALSE);
    lua_tinker::set(L, "UPDATE_TRUE", UPDATE_TRUE);
    lua_tinker::set(L, "ZONE_SERICE_COUNT", ZONE_SERICE_COUNT);
    lua_tinker::set(L, "AI_SERVICE", AI_SERVICE);
    lua_tinker::set(L, "ATT_APPROACH", ATT_APPROACH);
    lua_tinker::set(L, "ATT_ATTACK", ATT_ATTACK);
    lua_tinker::set(L, "ATT_ESCAPE", ATT_ESCAPE);
    lua_tinker::set(L, "ATT_GOBACK", ATT_GOBACK);
    lua_tinker::set(L, "ATT_IDLE", ATT_IDLE);
    lua_tinker::set(L, "ATT_MAX", ATT_MAX);
    lua_tinker::set(L, "ATT_PRATOL", ATT_PRATOL);
    lua_tinker::set(L, "ATT_PRATOLWAIT", ATT_PRATOLWAIT);
    lua_tinker::set(L, "ATT_RANDMOVE", ATT_RANDMOVE);
    lua_tinker::set(L, "ATT_SKILL", ATT_SKILL);
    lua_tinker::set(L, "ATT_SKILLWAIT", ATT_SKILLWAIT);
    lua_tinker::set(L, "GLOBAL_ROUTE_SERVICE", GLOBAL_ROUTE_SERVICE);
    lua_tinker::set(L, "GM_PROXY_SERVICE", GM_PROXY_SERVICE);
    lua_tinker::set(L, "GM_SERVICE", GM_SERVICE);
    lua_tinker::set(L, "GM_SERVICE_ID", GM_SERVICE_ID);
    lua_tinker::set(L, "MARKET_SERVICE", MARKET_SERVICE);
    lua_tinker::set(L, "MARKET_SERVICE_ID", MARKET_SERVICE_ID);
    lua_tinker::set(L, "MAX_AI_SERVICE_ID", MAX_AI_SERVICE_ID);
    lua_tinker::set(L, "MAX_GM_PROYX_SERVICE_ID", MAX_GM_PROYX_SERVICE_ID);
    lua_tinker::set(L, "MAX_SERVICE_ID", MAX_SERVICE_ID);
    lua_tinker::set(L, "MAX_SHAREZONE_SERVICE_ID", MAX_SHAREZONE_SERVICE_ID);
    lua_tinker::set(L, "MAX_SOCKET_SERVICE_ID", MAX_SOCKET_SERVICE_ID);
    lua_tinker::set(L, "MAX_ZONE_SERVICE_ID", MAX_ZONE_SERVICE_ID);
    lua_tinker::set(L, "MIN_AI_SERVICE_ID", MIN_AI_SERVICE_ID);
    lua_tinker::set(L, "MIN_GM_PROYX_SERVICE_ID", MIN_GM_PROYX_SERVICE_ID);
    lua_tinker::set(L, "MIN_SHAREZONE_SERVICE_ID", MIN_SHAREZONE_SERVICE_ID);
    lua_tinker::set(L, "MIN_SOCKET_SERVICE_ID", MIN_SOCKET_SERVICE_ID);
    lua_tinker::set(L, "MIN_ZONE_SERVICE_ID", MIN_ZONE_SERVICE_ID);
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
    lua_tinker::set(L, "ROUTE_SERVICE_ID", ROUTE_SERVICE_ID);
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
    lua_tinker::set(L, "WORLD_SERVICE", WORLD_SERVICE);
    lua_tinker::set(L, "WORLD_SERVICE_ID", WORLD_SERVICE_ID);
    lua_tinker::set(L, "ZONE_SERVICE", ZONE_SERVICE);
    lua_tinker::class_alias<Vector2, CPos2D>(L, "CPos2D");
    lua_tinker::class_alias<Vector3, CPos3D>(L, "CPos3D");
}