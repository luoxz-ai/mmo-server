#ifndef NPCTYPE_H
#define NPCTYPE_H
#ifndef NPCTYPE_H
#define NPCTYPE_H


#include "ActorAttrib.h"
#include "config/Cfg_Npc.pb.h"

enum NpcTypeFlag
{
	NPC_TYPE_FLAG_TASK	   = 0x00, //任务NPC
	NPC_TYPE_FLAG_SHOP	   = 0x01, //商店NPC
	NPC_TYPE_FLAG_INTERACT = 0x02, //交互型NPC
};

class CNpcType
{
	CNpcType() {}

  public:
	~CNpcType() {}
	CREATE_NEW_IMPL(CNpcType);
	bool Init(const Cfg_Npc_Row& row)
	{
		m_Data = row;

		return true;
	}

	using PB_T = Cfg_Npc;
	uint32_t GetID() const { return m_Data.id(); }
	UCHAR	 GetLevel() const { return m_Data.level(); }
	uint32_t GetTypeFlag() const { return m_Data.type_flag(); }

	uint32_t		   GetCampID() const { return m_Data.idcamp(); }
	uint32_t		   GetShopID() const { return m_Data.shop_id(); }
	const std::string& GetName() const { return m_Data.name(); }
	const std::string& GetShopLinkName() const { return m_Data.shop_linkname(); }
	const std::string& GetDialogText() const { return m_Data.dialog_txt(); }
	float			   GetHigh() const { return m_Data.high(); }
	float			   GetVolume() const { return m_Data.volume(); }

	OBJID	 GetScriptID() const { return m_Data.scriptid(); }
	uint32_t GetMapID() const { return m_Data.map_id(); }
	float	 GetPosX() const { return m_Data.posx(); }
	float	 GetPosY() const { return m_Data.posy(); }
	float	 GetFace() const { return m_Data.face(); }

  private:
	Cfg_Npc_Row m_Data;
};

//////////////////////////////////////////////////////////////////////
class CNpcTypeSet
{
	CNpcTypeSet();

  public:
	virtual ~CNpcTypeSet();

  public:
	CREATE_NEW_IMPL(CNpcTypeSet);
	bool Init(const char* szFileName);
	bool Reload(const char* szFileName);
	void Destroy();

	CNpcType*					  QueryObj(uint32_t idType);
	const std::vector<CNpcType*>* QueryObjByMapID(uint32_t idMap);

  protected:
	std::unordered_map<uint32_t, CNpcType*>				 m_setData;
	std::unordered_map<uint32_t, std::vector<CNpcType*>> m_setDataByMapID;
};

#endif /* NPCTYPE_H */

#endif /* NPCTYPE_H */
