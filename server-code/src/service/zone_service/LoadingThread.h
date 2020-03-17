#ifndef LOADINGTHREAD_H
#define LOADINGTHREAD_H

#include <mutex>

#include "BaseCode.h"
#include "LockfreeQueue.h"
#include "NetworkDefine.h"
#include "Thread.h"

class CPlayer;

enum LOADING_PROCESS_TYPE
{
	LPT_LOADING,
	LPT_SAVE,
};

struct ST_LOADINGTHREAD_PROCESS_DATA
{
	uint32_t	  nPorcessType;
	OBJID		  idPlayer;
	bool		  bChangeZone;
	VirtualSocket socket;
	uint64_t	  idScene;
	float		  fPosX;
	float		  fPosY;
	float		  fRange;
	float		  fFace;
	CPlayer*	  pPlayer;
};

class CZoneService;
class CLoadingThread
{
public:
	CLoadingThread(CZoneService* pZoneRef);
	~CLoadingThread();

	void Destory();

	//添加玩家到等待登陆队列
	bool AddLoginPlayer(OBJID idPlayer, const VirtualSocket& socket, bool bChangeZone, uint64_t idScene, float fPosX, float fPosY, float fRange, float fFace);
	//添加玩家到等待删除队列
	bool AddClosePlayer(CPlayer* pPlayer, uint64_t idScene, float fPosX, float fPosY, float fRange, float fFace);
	//从等待登陆队列，等待删除队列，Ready队列中移除该玩家的处理
	bool CancleWaiting(OBJID idPlayer);

	void OnThreadCreate();
	void OnThreadProcess();
	void OnThreadExit();
	void OnMainThreadExec();

	uint64_t GetLoadingCount() const { return m_nLoadingCount; }
	uint64_t GetSaveingCount() const { return m_nSaveingCount; }
	size_t	 GetReadyCount();

private:
	CZoneService* m_pZone;
	CNormalThread m_Thread;

	MPSCQueue<ST_LOADINGTHREAD_PROCESS_DATA*> m_WaitingList;
	MPSCQueue<ST_LOADINGTHREAD_PROCESS_DATA*> m_ReadyList;

	std::atomic<uint64_t> m_nLoadingCount;
	std::atomic<uint64_t> m_nSaveingCount;
	std::atomic<uint64_t> m_nReadyCount;
	std::atomic<OBJID>	  m_idCurProcess;
	std::atomic<OBJID>	  m_idNeedCancle;
};
#endif /* LOADINGTHREAD_H */
