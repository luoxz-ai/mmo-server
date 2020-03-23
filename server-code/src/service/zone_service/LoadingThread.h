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
	void CancleOnReadyList(OBJID idPlayer);
	void CancleOnWaitList(OBJID idPlayer);
private:
	CZoneService* m_pZone;
	CNormalThread m_Thread;

	std::atomic<bool>		m_bStop = false;
	std::mutex				m_csCV;
	std::condition_variable m_cv;

	//因为需要cancle，所以无法使用lockfree队列

	std::mutex				m_csWaitingList;
	std::mutex				m_csReadyList;
	std::deque<ST_LOADINGTHREAD_PROCESS_DATA*> m_WaitingList;
	std::deque<ST_LOADINGTHREAD_PROCESS_DATA*> m_ReadyList;

	std::atomic<uint64_t> m_nLoadingCount = 0;
	std::atomic<uint64_t> m_nSaveingCount = 0;
	std::atomic<uint64_t> m_nReadyCount = 0;
	std::atomic<OBJID>	  m_idCurProcess = 0;
	std::atomic<OBJID>	  m_idNeedCancle = 0;
};
#endif /* LOADINGTHREAD_H */
