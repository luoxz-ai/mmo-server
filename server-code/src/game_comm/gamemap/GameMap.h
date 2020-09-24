#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "BaseCode.h"
#include "MapData.h"
#include "config/Cfg_Scene.pb.h"
#include "config/Cfg_Scene_EnterPoint.pb.h"
#include "config/Cfg_Scene_LeavePoint.pb.h"
#include "config/Cfg_Scene_MonsterGenerator.pb.h"
#include "config/Cfg_Scene_Patrol.pb.h"
#include "config/Cfg_Scene_Reborn.pb.h"

export_lua enum MAP_TYPE {
    MAPTYPE_NORMAL = 0, //普通地图
};

export_lua enum MAP_FLAG {
    MAPFLAG_DYNAMAP = 0x0001, //副本

    MAPFLAG_COLLISION_ENABLE = 0x0100, // 此地图上需要进行碰撞检查

    MAPFLAG_DISABLE_RANDTRANS = 0x00001000, // 禁止随机传送（包括使用随机传送道具，随机传送技能）
    MAPFLAG_DISABLE_FLYMAP    = 0x00002000, // 禁止飞地图（包括使用：飞行道具，定位技能）
    MAPFLAG_DISABLE_ATTACK    = 0x00004000, // 禁止玩家使用任何技能同时不允许玩家攻击
    MAPFLAG_DISABLE_MOUNT     = 0x00008000, // 禁止玩家使用坐骑

    MAPFLAG_DISABLE_HELP        = 0x00010000, // 禁止求救
    MAPFLAG_DISABLE_CHANGEPK    = 0x00020000, // 禁止手动切换PK模式
    MAPFLAG_DISABLE_PKPROTECTED = 0x00040000, // 禁止PK保护
    MAPFLAG_DISABLE_PK          = 0x00080000, // 禁止PK

    MAPFLAG_DISABLE_LEAVE           = 0x00100000, // 禁止使用直接离开副本的按钮
    MAPFLAG_LEVAE_TO_HOME           = 0x00200000, // 点击离开副本按钮会回到安全区而不是回来进入副本前最后的记录点
    MAPFLAG_DISABLE_REBORN_MAPPOS   = 0x00400000, // 禁止在副本复活点复活
    MAPFLAG_DISABLE_REBORN_STANDPOS = 0x00800000, // 禁止原地复活

    MAPFLAG_RECORD_DISABLE = 0x01000000, // 禁止记录下线坐标
    MAPFLAG_HOME_DISABLE   = 0x02000000, // 禁止记录回城点
    MAPFLAG_DISABLE_STALL  = 0x04000000, // 禁止摆摊
    MAPFLAG_DISABLE_PLACE  = 0x08000000, // 禁止放置地面物品

};

export_lua enum PARTOL_TYPE { PARTOL_ONCE, PARTOL_RING, PARTOL_BACK };

export_lua enum MonsterGeneratorShapeType {
    MONSTERGENERATOR_CIRCLE = 0, // x,z为中心range的圆形
    MONSTERGENERATOR_RECT   = 1, // x,z为左上角,width宽,range高的矩形
};

export_lua enum REBORN_TYPE {
    REBORN_HOME,
    REBORN_MAPPOS,
    REBORN_STANDPOS,
};

class CMapManager;
export_lua class CGameMap : public NoncopyableT<CGameMap>
{
    CGameMap();
    bool Init(CMapManager* pManager, const Cfg_Scene_Row& data, const CMapData* pMapData);

public:
    ~CGameMap();

    CreateNewImpl(CGameMap);

    export_lua bool     IsInsideMap(float x, float y) const;
    export_lua bool     IsZoneMap(uint16_t idZone) const { return m_idZone == 0 || idZone == m_idZone; }
    export_lua uint16_t GetZoneID() const { return m_idZone; }
    export_lua uint32_t GetMapID() const { return m_idMap; }
    export_lua uint32_t GetMapTemplateID() const { return m_idMapTemplate; }
    export_lua uint32_t GetMapType() const { return m_nMapType; }
    export_lua uint32_t GetMapFlag() const { return m_nMapFlag; }
    export_lua uint64_t GetScriptID() const { return m_idScript; }
    export_lua bool     HasMapFlag(uint32_t flag) const { return ::HasFlag(GetMapFlag(), flag); }
    export_lua bool     IsDynaMap() const { return HasFlag(GetMapFlag(), MAPFLAG_DYNAMAP); }

    export_lua bool IsNearLeavePoint(float x, float y, uint32_t& destMapID, uint32_t& destEnterPointIdx) const;
    export_lua bool IsNearLeavePointX(uint32_t nLeavePointIdx, float x, float y, uint32_t& destMapID, uint32_t& destEnterPointIdx) const;

    export_lua bool IsPassDisable(float x, float y) const;
    export_lua bool IsJumpDisable(float x, float y) const;
    export_lua bool IsPvPDisable(float x, float y) const;
    export_lua bool IsStallDisable(float x, float y) const;
    export_lua bool IsPlaceDisable(float x, float y) const;
    export_lua bool IsRecordDisable(float x, float y) const;
    export_lua bool IsDropDisable(float x, float y) const;
    export_lua bool IsPvPFree(float x, float y) const;
    export_lua bool IsDeadNoDrop(float x, float y) const;

    export_lua uint32_t GetSPRegionIdx(float x, float y) const;
    export_lua float    GetHigh(float x, float y) const;

    export_lua Vector2 FindPosNearby(const Vector2& pos, float range) const;
    export_lua Vector2 LineFindCanStand(const Vector2& src, const Vector2& dest) const;
    export_lua Vector2 LineFindCanJump(const Vector2& src, const Vector2& dest) const;

public:
    export_lua const CMapData* GetMapData() const { return m_pMapData; }
    export_lua const auto&     GetPhaseData() const { return m_PhaseDataSet; }
    export_lua const PhaseData* GetPhaseDataById(uint64_t idPhase) const;

    export_lua const Cfg_Scene_EnterPoint_Row* GetEnterPointByIdx(uint32_t idx) const;
    void                                       _setEnterPoint(const Cfg_Scene_EnterPoint_Row& iter);

    export_lua const Cfg_Scene_LeavePoint_Row* GetLeavePointByIdx(uint32_t idx) const;
    void                                       _setLeavePoint(const Cfg_Scene_LeavePoint_Row& iter);

    export_lua const auto& GetRebornData() const { return m_RebornDataSet; }
    export_lua const Cfg_Scene_Reborn_Row* GetRebornDataByIdx(uint32_t idx) const
    {
        auto it = m_RebornDataSet.find(idx);
        if(it == m_RebornDataSet.end())
            return nullptr;
        else
            return &(it->second);
    }

    void _AddRebornData(const Cfg_Scene_Reborn_Row& iter);

    export_lua const auto& GetGeneratorData() const { return m_MonsterGeneratorList; }
    void                   _AddMonsterGenerator(const Cfg_Scene_MonsterGenerator_Row& iter);

    export_lua const auto& GetPatrolData() const { return m_PatrolSet; }
    export_lua const Cfg_Scene_Patrol_Row* GetPatrolDataByIdx(uint32_t idx) const
    {
        auto it = m_PatrolSet.find(idx);
        if(it == m_PatrolSet.end())
            return nullptr;
        else
            return &(it->second);
    }

    void _AddPatrol(const Cfg_Scene_Patrol_Row& iter);

private:
    CMapManager* m_pManager = nullptr;
    uint32_t     m_idMap    = 0;
    std::string  m_MapName;
    uint32_t     m_idZone        = 0;
    uint32_t     m_idMapTemplate = 0;
    uint32_t     m_nMapType      = 0;
    uint32_t     m_nMapFlag      = 0;
    uint64_t     m_idScript      = 0;

    std::unordered_map<uint64_t, PhaseData>                      m_PhaseDataSet;
    std::unordered_map<uint32_t, Cfg_Scene_EnterPoint_Row>       m_EnterPointSet;
    std::unordered_map<uint32_t, Cfg_Scene_LeavePoint_Row>       m_LeavePointSet;
    std::unordered_map<uint32_t, Cfg_Scene_MonsterGenerator_Row> m_MonsterGeneratorList;
    std::unordered_map<uint32_t, Cfg_Scene_Patrol_Row>           m_PatrolSet;
    std::unordered_map<uint32_t, Cfg_Scene_Reborn_Row>           m_RebornDataSet;

    const CMapData* m_pMapData = nullptr;
};
#endif /* GAMEMAP_H */
