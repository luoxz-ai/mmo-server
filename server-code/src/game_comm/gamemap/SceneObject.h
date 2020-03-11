#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "BaseCode.h"
#include "game_common_def.h"

class CSceneBase;
class CSceneNode;
class CSceneCollisionNode;

class CSceneObject;
typedef std::deque<OBJID>										BROADCAST_SET;
typedef std::unordered_map<uint32_t, std::unordered_set<OBJID>> BROADCAST_SET_BYTYPE;
typedef std::unordered_map<OBJID, CSceneObject*>				ACTOR_MAP;

class CSceneObject
{
protected:
	CSceneObject();

public:
	virtual ~CSceneObject();

public:
	template<typename T>
	T* ConvertToDerived()
	{
		if(CanConvertTo(T::GetActorTypeStatic()) == true)
			return static_cast<T*>(this);
		else
			return nullptr;
	}
	virtual bool CanConvertTo(ActorType actor_type) { return GetActorType() == actor_type; }

	export_lua virtual ActorType GetActorType() const = 0;
	export_lua bool				 IsPlayer() const { return GetActorType() == ACT_PLAYER; }
	export_lua bool				 IsMonster() const { return GetActorType() == ACT_MONSTER; }
	export_lua bool				 IsNpc() const { return GetActorType() == ACT_NPC; }
	export_lua bool				 IsPet() const { return GetActorType() == ACT_PET; }
	export_lua CSceneBase* GetCurrentScene() const { return m_pScene; }
	export_lua CSceneNode* GetSceneNode() const { return m_pSceneNode; }
	export_lua CSceneCollisionNode* GetCollisionNode() const { return m_pCollisionNode; }
	export_lua void					SetSceneNode(CSceneNode* val);
	export_lua void					SetCollisionNode(CSceneCollisionNode* val);

	export_lua OBJID GetID() const { return m_ID; }
	void			 SetID(OBJID v) { m_ID = v; }

	export_lua virtual const Vector2& GetPos() const { return m_Pos; }
	export_lua virtual Vector2&		  GetPosRef() { return m_Pos; }
	export_lua virtual float		  GetPosX() const { return m_Pos.x; }
	export_lua virtual float		  GetPosY() const { return m_Pos.y; }
	export_lua virtual float		  GetFace() const { return m_Face; }
	export_lua virtual void			  SetPos(const Vector2& pos);
	export_lua virtual void			  SetFace(float face)
	{
		if(Math::isNaN(face) == false)
			m_Face = face;
	}
	export_lua void FaceTo(const Vector2& pos);

public:
	// AOI
	export_lua virtual bool UpdateViewList();
	export_lua virtual void ClearViewList(bool bSendMsgToSelf) = 0;

public:
	// AOI
	export_lua virtual bool IsEnemy(CSceneObject* pTarget) const;

	export_lua bool IsInViewActor(CSceneObject* actor) const;
	export_lua bool IsInViewActorByID(OBJID idActor) const;

	export_lua uint32_t GetCurrentViewActorCount() const;
	export_lua uint32_t GetCurrentViewPlayerCount();
	export_lua uint32_t GetCurrentViewMonsterCount();
	export_lua const BROADCAST_SET& _GetViewList() const { return m_ViewActors; }
	export_lua const BROADCAST_SET_BYTYPE& _GetViewListByType() const { return m_ViewActorsByType; }
	export_lua void						   ForeachViewActorList(const std::function<void(OBJID)>& func);

protected:
	// AOI
	virtual void RemoveFromViewList(CSceneObject* pActor, OBJID idActor, bool bErase);
	virtual void AddToViewList(CSceneObject* pActor, bool bChkDuplicate, bool bSendShow);
	virtual void AOIProcessActorAddToAOI(BROADCAST_SET& setBCActorAdd, const ACTOR_MAP& mapAllViewActor) = 0;
	virtual void AOIProcessActorRemoveFromAOI(const BROADCAST_SET& setBCActorDel,
											  BROADCAST_SET&	   setBCActor,
											  int32_t			   nCanReserveDelCount,
											  uint32_t			   view_range_out_square)							 = 0;
	virtual void AOIProcessPosUpdate(){};
	virtual bool IsNeedAddToBroadCastSet(CSceneObject* pActor) { return false; }
	virtual bool IsMustAddToBroadCastSet(CSceneObject* pActor) { return false; }

public:
	virtual void OnEnterMap(CSceneBase* pScene);
	virtual void OnLeaveMap(uint64_t idTargetScene);

	export_lua void			SetHideCoude(int32_t nHideCount);
	export_lua virtual void AddHide();
	export_lua virtual void RemoveHide();

protected:
	OBJID				 m_ID			  = 0;		 // id
	CSceneBase*			 m_pScene		  = nullptr; //����
	CSceneNode*			 m_pSceneNode	  = nullptr;
	CSceneCollisionNode* m_pCollisionNode = nullptr;
	Vector2				 m_Pos;		   //��ǰ��λ��
	float				 m_Face;	   //��ǰ�ĳ���
	BROADCAST_SET		 m_ViewActors; // ��Ұ�ڵ�����
	BROADCAST_SET_BYTYPE m_ViewActorsByType;
	int32_t				 m_nHideCount = 0;
};
#endif /* SCENEOBJECT_H */
