#ifndef AISCENE_H
#define AISCENE_H

#include "MonsterGenerator.h"
#include "NetworkMessage.h"
#include "SceneBase.h"

class CAIScene : public CSceneBase
{
protected:
    CAIScene();
 public:
    CreateNewImpl(CAIScene);
public:
    virtual ~CAIScene();

public:
    bool               Init(const SceneID& idScene);
    CMonsterGenerator& GetMonsterGen() { return m_MonsterGen; }

private:
    CMonsterGenerator m_MonsterGen;
};

class CAISceneManager: public Noncopyable<CAISceneManager>
{
    CAISceneManager();
public:
    CreateNewImpl(CAISceneManager);
public:
    ~CAISceneManager();
    

    bool Init(uint32_t idZone);
    void Destory();
    void RegisterMessageHandler();

    CAIScene* CreateScene(CNetworkMessage* pMsg);
    void      DestoryDynaScene(CNetworkMessage* pMsg);

    CAIScene* QueryScene(const SceneID& idScene);
    void      OnTimer();

protected:
private:
    std::unordered_map<SceneID, CAIScene*> m_mapScene;
    size_t                                 m_nStaticScene;
};
#endif /* AISCENE_H */
