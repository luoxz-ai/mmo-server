#include "LoadingThread.h"

#include "ActorManager.h"
#include "Player.h"
#include "ZoneService.h"

CLoadingThread::CLoadingThread(CZoneService* pZoneRef)
	: m_pZone(pZoneRef)
	, m_Thread(50,
			   std::string("Loading") + std::to_string(pZoneRef->GetServerPort().GetServiceID()),
			   std::bind(&CLoadingThread::OnThreadProcess, this),
			   std::bind(&CLoadingThread::OnThreadCreate, this))
	, m_idCurProcess(0)
	, m_idNeedCancle(0)
{
}

CLoadingThread::~CLoadingThread()
{
	Destory();
}

void CLoadingThread::Destory()
{
	m_Thread.Stop();
	m_Thread.Join();

	//把剩余的东西处理完
	{
		ST_LOADINGTHREAD_PROCESS_DATA* pData = nullptr;
		while(m_WaitingList.get(pData))
		{
			if(pData->nPorcessType == LPT_SAVE)
			{
				if(pData->pPlayer)
				{
					pData->pPlayer->SaveInfo();
				}
			}
			SAFE_DELETE(pData);
		}
	}
	
	{
		ST_LOADINGTHREAD_PROCESS_DATA* pData = nullptr;
		while(m_ReadyList.get(pData))
		{
			SAFE_DELETE(pData);
		}
	}

}

bool CLoadingThread::AddLoginPlayer(OBJID idPlayer, const VirtualSocket& socket, bool bChangeZone, uint64_t idScene, float fPosX, float fPosY, float fRange, float fFace)
{
	auto pData = new ST_LOADINGTHREAD_PROCESS_DATA{LPT_LOADING, idPlayer, bChangeZone, socket, idScene, fPosX, fPosY, fRange, fFace, nullptr};
	m_nLoadingCount++;
	
	m_WaitingList.push(pData);
	return true;
}

bool CLoadingThread::AddClosePlayer(CPlayer* pPlayer, uint64_t idScene, float fPosX, float fPosY, float fRange, float fFace)
{
	auto pData = new ST_LOADINGTHREAD_PROCESS_DATA{LPT_SAVE, pPlayer->GetID(), idScene != 0, pPlayer->GetSocket(), idScene, fPosX, fPosY, fRange, fFace, pPlayer};
	m_nSaveingCount++;
	
	m_WaitingList.push(pData);
	return true;
}

bool CLoadingThread::CancleWaiting(OBJID idPlayer)
{
	//这个函数必然是在主函数上调用的
	if(m_idCurProcess == idPlayer)
	{
		m_idNeedCancle = idPlayer;
	}

	//先处理已经Ready的列表
	{
		__ENTER_FUNCTION

		//遍历Ready列表
		ST_LOADINGTHREAD_PROCESS_DATA* pData = nullptr;
		while(m_ReadyList.get(pData) )
		{
			if(pData->nPorcessType == LPT_LOADING)
			{
				// loading ready
				CPlayer* pPlayer = pData->pPlayer;
				if(pPlayer && pPlayer->GetID() == idPlayer)
				{
					SAFE_DELETE(pData->pPlayer);
					SAFE_DELETE(pData);
					LOGLOGIN("remove from loading_ready:{}", idPlayer);
				}

			}
			else
			{
				// save ready
				CPlayer* pPlayer = pData->pPlayer;
				if(pPlayer && pPlayer->GetID() == idPlayer)
				{
					SAFE_DELETE(pData->pPlayer);
					SAFE_DELETE(pData);
					LOGLOGIN("remove from saver_eady:{}", idPlayer);
				}
			}
		}
		__LEAVE_FUNCTION
	}

	//再清理等待列表
	{
		__ENTER_FUNCTION

		ST_LOADINGTHREAD_PROCESS_DATA* pLoadData = nullptr;
		while(m_WaitingList.get(pLoadData))
		{
			if(pLoadData == nullptr)
			{
				continue;
			}

			if(pLoadData->nPorcessType == LPT_LOADING)
			{
				if(pLoadData->idPlayer == idPlayer)
				{
					// remove this
					SAFE_DELETE(pLoadData->pPlayer);
					SAFE_DELETE(pLoadData);
					m_nLoadingCount--;
					LOGLOGIN("remove from loading:{}", idPlayer);
					continue;
				}
			}
			else
			{
				CPlayer* pPlayer = pLoadData->pPlayer;
				if(pPlayer == nullptr)
				{
					LOGERROR("Process WaitList PlayerIsNull:{}", pLoadData->idPlayer);
					continue;
				}

				if(pPlayer->GetID() == idPlayer)
				{
					//优先写入
					pPlayer->SaveInfo();

					//必然是后一个顶前一个,应该已经在World被Kick了
					SAFE_DELETE(pLoadData->pPlayer);
					SAFE_DELETE(pLoadData);
					m_nSaveingCount--;
					LOGLOGIN("remove from saving:{}", idPlayer);
					continue;
				}
			}
		}
		__LEAVE_FUNCTION
	}

	return true;
}

void CLoadingThread::OnThreadCreate()
{
	SetZoneServicePtr(m_pZone);
	std::string name = std::string("Zone") + std::to_string(m_pZone->GetServerPort().GetServiceID());
	BaseCode::SetNdc(name);
	LOGMESSAGE("ThreadID:{}", get_cur_thread_id());
}
void CLoadingThread::OnThreadProcess()
{
	//处理
	while(true)
	{
		__ENTER_FUNCTION
		// take from list
		ST_LOADINGTHREAD_PROCESS_DATA* pCurData = nullptr;
		if(m_WaitingList.get(pCurData) == false)
		{
			return;
		}		
		m_idCurProcess = pCurData->idPlayer;
		
		if(pCurData->nPorcessType == LPT_LOADING)
		{
			m_nLoadingCount--;
			CPlayer* pPlayer = CPlayer::CreateNew(pCurData->idPlayer, pCurData->socket);
			//如果当前处理的这个需要被cancle
			if(pPlayer == nullptr)
			{
				// log error
				LOGLOGIN("LoadingFail:{}", pCurData->idPlayer);
				SAFE_DELETE(pCurData);
			}
			else if(m_idNeedCancle == m_idCurProcess)
			{
				// log cancle
				LOGLOGIN("CacleLoading:{}", pPlayer->GetID());
				SAFE_DELETE(pCurData);
				SAFE_DELETE(pPlayer);
			}
			else
			{
				//放入ready列表
				LOGLOGIN("LoadingReady:{}", pCurData->idPlayer);
				
				pCurData->pPlayer = pPlayer;
				m_nReadyCount++;
				m_ReadyList.push(pCurData);
			}
		}
		else
		{
			m_nSaveingCount--;
			if(pCurData->pPlayer == nullptr)
			{
				SAFE_DELETE(pCurData);
			}
			else
			{
				//保存成功， 放入ready列表，准备发送ChangeZone消息给TargetZone
				pCurData->pPlayer->SaveInfo();
				LOGLOGIN("SaveingReady:{}", pCurData->idPlayer);
				if(pCurData->bChangeZone)
				{
					m_nReadyCount++;
					m_ReadyList.push(pCurData);
				}
				else
				{
					//可以直接删除了
					SAFE_DELETE(pCurData->pPlayer);
					SAFE_DELETE(pCurData);
				}
			}
		}

		m_idCurProcess = 0;
		m_idNeedCancle = 0;

		__LEAVE_FUNCTION
	}
}

void CLoadingThread::OnMainThreadExec()
{
	int32_t nCount = 0;
	while(true)
	{
		__ENTER_FUNCTION

		ST_LOADINGTHREAD_PROCESS_DATA* pData = nullptr;
		if(m_ReadyList.get(pData) == false)
			return;

		if(pData->nPorcessType == LPT_LOADING)
		{
			if(pData->pPlayer)
			{
				ActorManager()->AddActor(pData->pPlayer);
				pData->pPlayer->OnLogin(!pData->bChangeZone, pData->idScene, pData->fPosX, pData->fPosY, pData->fRange, pData->fFace);
			}
		}
		else // save ready
		{
			pData->pPlayer->OnChangeZoneSaveFinish(pData->idScene, pData->fPosX, pData->fPosY, pData->fRange, pData->fFace);
			SAFE_DELETE(pData->pPlayer);
		}
		m_nReadyCount--;
		SAFE_DELETE(pData);

		nCount++;
		static const int32_t MAX_PROCESS_READY_COUNT = 25;
		if(nCount > MAX_PROCESS_READY_COUNT)
		{
			return;
		}
		__LEAVE_FUNCTION
	}
}

size_t CLoadingThread::GetReadyCount()
{
	return m_nReadyCount;
}
