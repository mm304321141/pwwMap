

#ifdef WIN32
//#define  _WIN32_WINNT  (0x0400)

#include <process.h>
#include <windows.h>
#else
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#endif
#include <stdio.h>

#include "Thread.hpp"
#include <iostream>

/* *******************************************************************
* @����: CThread--- ����̶߳������ݳ�Ա�ĳ�ʼ��;
*
* @����: szDescription---�̵߳Ĺ�������,Ĭ��ֵΪ�մ�;
* @����: uiPriority---�̵߳����ȼ�;
* @����: pThreadMgr---Ҫע����̹߳���������ָ��;
* @����: uiWorkTime---�̵߳�ҵ����ʱ��,Ĭ��ֵΪ0����
*					   ʾ������ҵ����ʱ��,��λΪ��;
* *******************************************************************/
CThread::CThread(pThreadAttrib pAttrib, CThreadMgr *pThreadMgr, 
				 unsigned uiWorkTime)
{
	m_thread_handle = 0;
	m_uiThreadId = 0;
	m_pThreadMgr = pThreadMgr;
	if (NULL == pAttrib)
	{
		m_szDescription = "";
		m_uiPriority = 0;
		m_iSysPriority =0;
	}
	else
	{
		m_szDescription = pAttrib->szDescription;
		m_uiPriority = pAttrib->ui_ClrPriority; 
		m_iSysPriority = pAttrib->sysPriority;
	}		

	m_uiWorkTime = uiWorkTime;
	if (uiWorkTime > 0)
	{
		m_ThreadState = e_TS_NORMAL;
	}
	else
	{
		m_ThreadState = e_TS_UNKONWN;
	}
	m_LastHeartBeat = 0;

}

CThread::~CThread()
{	
	setThreadState(e_TS_EXIT);
#ifndef WIN32
	pthread_attr_destroy(&m_threadAttrib);
#endif
	stop();
}

/* ******************************************************************
* @������open--- ����̵߳Ĵ���������������regThread�������߳�
*		         ����������н���ע��Ǽ�;
*
* @������args--- Ĭ��Ϊ��,��������;
*
* @����ֵ��0��ʾ�ɹ�,1��ʾʧ��;
********************************************************************/
int CThread::open(void *args)
{
	if (0 == m_thread_handle) 
	{
		m_thread_args = args;
#ifdef WIN32        

		m_thread_handle = (HANDLE)_beginthreadex(0, 0, thread_proxy, (void*)this, 0, &m_uiThreadId);

		//�����߳����ȼ�
		switch (m_iSysPriority)
		{
		case THREAD_PRIORITY_IDLEX :
			SetThreadPriority(reinterpret_cast<HANDLE>(m_thread_handle), THREAD_PRIORITY_IDLE);
			break;
		case THREAD_PRIORITY_ERROR_RETURNX:
			SetThreadPriority(reinterpret_cast<HANDLE>(m_thread_handle), THREAD_PRIORITY_ERROR_RETURN);
			break;
		case THREAD_PRIORITY_LOWESTX :
			SetThreadPriority(reinterpret_cast<HANDLE>(m_thread_handle), THREAD_PRIORITY_LOWEST);
			break;
		case THREAD_PRIORITY_BELOW_NORMALX :
			SetThreadPriority(reinterpret_cast<HANDLE>(m_thread_handle), THREAD_PRIORITY_BELOW_NORMAL);
			break;
		case THREAD_PRIORITY_ABOVE_NORMALX :
			SetThreadPriority(reinterpret_cast<HANDLE>(m_thread_handle), THREAD_PRIORITY_ABOVE_NORMAL);
			break;
		case THREAD_PRIORITY_HIGHESTX :
			SetThreadPriority(reinterpret_cast<HANDLE>(m_thread_handle), THREAD_PRIORITY_HIGHEST);
			break;
		case THREAD_PRIORITY_TIME_CRITICALX:
			SetThreadPriority(reinterpret_cast<HANDLE>(m_thread_handle), THREAD_PRIORITY_TIME_CRITICAL);
			break;
		default :
			SetThreadPriority(reinterpret_cast<HANDLE>(m_thread_handle), THREAD_PRIORITY_NORMAL);
		}


		sleep(100);
		//printf("thread_id is %x,thread handle is %x\n",m_uiThreadId,m_thread_handle);		
#else
		sched_param schedParam;
		schedParam.__sched_priority = m_iSysPriority;			

		//��ʼ��pthread_attr_t�ṹ����;
		pthread_attr_init (&m_threadAttrib);

		//�������ȼ�
		pthread_attr_setschedparam(&m_threadAttrib, &schedParam);

		//�����߳�
		int res = pthread_create(&m_thread_handle, &m_threadAttrib, &CThread::thread_proxy, (void*)this);	

		//pthread_attr_destroy ( threadAttrib );
		m_uiThreadId = m_thread_handle;
		sleep(100);
		//printf("thread_id is %ud \n",m_thread_handle);	
#endif	
		fflush(stdout);

		if (m_pThreadMgr)
		{
			regThread();
		}

		return (m_thread_handle != 0) ?0 :1;
	}//end if

	return 0;
}

/*******************************************************************
* @������ stop--- �ر��߳�,ͬʱ���з�ע��;
*******************************************************************/
void CThread::stop()
{
	if (0 != m_thread_handle) 
	{
#ifdef WIN32

		WaitForSingleObject(HANDLE(m_thread_handle), INFINITE);
		CloseHandle(HANDLE(m_thread_handle));
#else
		pthread_join(m_thread_handle, 0);
#endif

		if (m_pThreadMgr)
		{
			unRegThread();
		}
		m_thread_handle = 0;
	}
}

/*********************************************************
* @������threadProxy ---�߳�ִ�к���;
*	     
* @����ֵ��Windowsƽ̨���践��ֵ���ɹ�����0;
*		   Linuxƽ̨�з���ֵ;
********************************************************/
#ifdef WIN32
unsigned __stdcall CThread::thread_proxy(void *param)
#else
void* CThread::thread_proxy(void *param)
#endif
{
#ifndef WIN32
	sigset_t block_set;
	sigfillset(&block_set);
	pthread_sigmask(SIG_BLOCK, &block_set, 0);	
#endif
	CThread *pObj = (CThread*)param;	
	pObj->svc();	
	return 0;
}

/********************************************************
* @������ sleep ---�߳����ߺ���;
*	     
* @����:  dwMilliseconds---����ʱ��,��λΪ����;
*
* @����ֵ��Windowsƽ̨���践��ֵ���ɹ�����0;
*		   Linuxƽ̨�з���ֵ;
********************************************************/
void CThread::sleep(unsigned long dwMilliseconds)
{
#ifdef WIN32
	::Sleep(dwMilliseconds);
#else
	timeval wait;
	wait.tv_sec = (dwMilliseconds / 1000);
	wait.tv_usec = (dwMilliseconds - ((long)(dwMilliseconds / 1000)) * 1000) * 1000;
	::select (0, 0, 0, 0, &wait);
#endif
}

unsigned CThread::getThreadId()
{
	return m_uiThreadId;
}

/*******************************************************
* @������getHeatBeat ---��õ�ǰ�̵߳�ĩ������;
*	     
* @����ֵ: ĩ��������ʱ��,��λ��;
*
********************************************************/
time_t  CThread::getHeartBeat()
{
	return m_LastHeartBeat;
}

string CThread::getDescription()
{
	return m_szDescription;
}

/********************************************************
* @������getWorkTime ---��õ�ǰ�̵߳�ҵ����ʱ��;
*	     
* @����ֵ: �̵߳�ҵ����ʱ��,��λ��;
*
********************************************************/
unsigned CThread::getWorkTime()
{
	return m_uiWorkTime;
}


E_RUN_STATE CThread::getThreadState()
{
	return m_ThreadState;
}

/*******************************************************************
* @������getPriority ---����߳�Ϊ�̹߳�����������ղ��������õ����ȼ�;
*	     
* @����ֵ: �߳�Ϊ��ղ��������õ����ȼ�;
*
*******************************************************************/
unsigned CThread::getPriority()
{
	return m_uiPriority;
}

//��ȡ�̹߳���ϵͳ�ں����õ����ȼ�
int  CThread::getSysPriority()
{
#ifdef WIN32
	return GetThreadPriority(m_thread_handle);
#else
	sched_param schedParam;
	pthread_attr_getschedparam(&m_threadAttrib, &schedParam);
	return schedParam.__sched_priority;
#endif
}

/********************************************************
* @������setThreadState ---�����̶߳�������״̬;
*	     
* @����: thrRunState ---�߳�����ʱ��״̬;
*
********************************************************/

void CThread::setThreadState(E_RUN_STATE thrRunState)
{
	m_ThreadState = thrRunState;
}

/********************************************************
* @������sendHeartBeat ---���̹߳�������������;
********************************************************/
void CThread::sendHeartBeat()
{
	m_LastHeartBeat = time(NULL);
}

/*******************************************************
* @������regThread ---���̹߳�����ע���߳�;
********************************************************/
void CThread::regThread( )
{
	(m_pThreadMgr->m_ThreadList).insert(pair<unsigned, CThread*>(this->getThreadId(), this));
	(m_pThreadMgr->m_iThreadCount)++;
}

/********************************************************
* @������unregThread ---���̹߳�����ע���߳�(��ע��);
********************************************************/
void CThread::unRegThread()
{
	unsigned uiThreadId = this->getThreadId();
	if ((m_pThreadMgr->m_ThreadList).count(uiThreadId))
	{
		(m_pThreadMgr->m_ThreadList).erase(this->getThreadId());
		(m_pThreadMgr->m_iThreadCount)--;
	}
}

/********************************************************
* @������clearUp ---�̵߳���մ���;
********************************************************/	
int CThread::clearUp()
{
	return 0; //�ɹ�����0;
}

