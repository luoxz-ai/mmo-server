#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <unordered_map>

#include "GameMap.h"

// max uint64_t = 1844 6744 0737 0955 1614
// 099999 00 000000000000

struct SceneID
{
	static const uint64_t IDZONE_FACTOR = 100000000000ull;
	static const uint64_t IDMAP_FACTOR	= 100000000000000ull;

	SceneID(uint16_t _idZone, uint16_t _idMap, uint32_t _nDynaMapIdx)
		: data64(_idMap * IDMAP_FACTOR + _idZone * IDZONE_FACTOR + _nDynaMapIdx)
	{
	}
	SceneID(uint64_t _data64 = 0)
		: data64(_data64)
	{
	}
	SceneID(const SceneID& rht)
		: data64(rht.data64)
	{
	}

	uint64_t data64;

	operator uint64_t() const { return data64; }

	bool operator==(const SceneID& rht) const { return data64 == rht.data64; }

	bool operator<(const SceneID& rht) const { return data64 < rht.data64; }

	uint32_t GetDynaMapIdx() const { return data64 % IDZONE_FACTOR; }
	uint16_t GetZoneID() const { return (data64 % IDMAP_FACTOR) / IDZONE_FACTOR; }
	uint16_t GetMapID() const { return data64 / IDMAP_FACTOR; }
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
template<>
struct hash<SceneID>
{
	typedef SceneID		argument_type;
	typedef std::size_t result_type;
	result_type			operator()(argument_type const& s) const
	{
		std::hash<uint64_t> hasher;
		return hasher(s.data64);
	}
};
} // namespace std

class CMapManager
{
	CMapManager();
public:
	~CMapManager();
	CREATE_NEW_IMPL(CMapManager);
	
	bool	  Init(uint16_t idZone);
	void	  ForEach(const std::function<void(CGameMap*)>& func);
	CGameMap* QueryMap(uint16_t idMap);
	CMapData* QueryMapData(uint16_t idMapTemplate);

private:
	std::unordered_map<uint16_t, CGameMap*> m_vecMap;
	std::unordered_map<uint16_t, CMapData*> m_vecMapData;
};

#endif /* MAPMANAGER_H */
