#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <unordered_map>

#include "GameMap.h"
#include "export_lua.h"
// max uint64_t = 1844 6744 0737 0955 1614
// 099999 00 0000 0000 0000

export_lua struct SceneID
{
    static constexpr uint64_t IDZONE_FACTOR =   1ull *10000ull *10000ull *10000ull;
    static constexpr uint64_t IDMAP_FACTOR  = 100ull *10000ull *10000ull *10000ull;

    export_lua SceneID(uint16_t _idZone, uint16_t _idMap, uint32_t _nPhaseIdx)
        : data64(_idMap * IDMAP_FACTOR + _idZone * IDZONE_FACTOR + _nPhaseIdx)
    {
        if(_idZone == 0 || _idZone > 10)
        {
            throw std::runtime_error("zoneid overflow");
        }
    }
    export_lua SceneID(uint64_t _data64 = 0)
        : data64(_data64)
    {
    }
    export_lua SceneID(const SceneID& rht)
        : data64(rht.data64)
    {
    }

    export_lua uint64_t data64;

    operator uint64_t() const { return data64; }

    export_lua bool operator==(const SceneID& rht) const { return data64 == rht.data64; }

    export_lua bool operator<(const SceneID& rht) const { return data64 < rht.data64; }

    export_lua bool IsPhaseIdxVaild() const {return GetPhaseIdx() != uint32_t(-1);}
    export_lua uint32_t GetPhaseIdx() const { return data64 % IDZONE_FACTOR; }
    export_lua SceneID  GetStaticPhaseSceneID() const { return {GetZoneID(), GetMapID(), 0};}

    export_lua uint16_t GetZoneID() const { return (data64 % IDMAP_FACTOR) / IDZONE_FACTOR; }
    export_lua uint16_t GetMapID() const { return data64 / IDMAP_FACTOR; }
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
template<>
struct hash<SceneID>
{
    typedef SceneID     argument_type;
    typedef std::size_t result_type;
    result_type         operator()(argument_type const& s) const
    {
        std::hash<uint64_t> hasher;
        return hasher(s.data64);
    }
};
} // namespace std

export_lua class CMapManager: public NoncopyableT<CMapManager>
{
    CMapManager();
public:
    CreateNewImpl(CMapManager);
public:
    ~CMapManager();
    

    bool      Init(uint16_t idZone);
    export_lua void      ForEach(const std::function<void(CGameMap*)>& func)const;
    export_lua const CGameMap* QueryMap(uint16_t idMap)const;
    export_lua const CMapData* QueryMapData(uint16_t idMapTemplate)const;
private:
    CGameMap* _QueryMap(uint16_t idMap)const;
private:
    std::unordered_map<uint16_t, std::unique_ptr<CGameMap>> m_vecMap;
    std::unordered_map<uint16_t, std::unique_ptr<CMapData>> m_vecMapData;
};

#endif /* MAPMANAGER_H */
