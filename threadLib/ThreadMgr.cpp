#include "ThreadMgr.hpp"
#include "Thread.hpp"
#include <iomanip>
#include <iostream>



CThreadMgr* CThreadMgr::m_pThreadMgr = NULL;  //�ྲ̬��Ա������ʼ��;

CThreadMgr::CThreadMgr()
{
	m_iThreadCount = 0;
}

CThreadMgr::~CThreadMgr()
{
	if (m_pThreadMgr)
	{
		delete m_pThreadMgr;
		m_pThreadMgr = NULL;
	}	
}

/********************************************************
* @����: createInstance ---�����̹߳�������ʵ��ָ��.
*
* @����: logObj ---��־����������.
*
* @����ֵ: �����̹߳����������ָ��.
********************************************************/
CThreadMgr* CThreadMgr::createInstance()
{
	if (NULL == m_pThreadMgr)
	{
		m_pThreadMgr = new CThreadMgr();	  
	}
	return m_pThreadMgr;
}

int CThreadMgr::getThreadCount()
{   
	return m_iThreadCount;
}

/********************************************************
* @����: checkThread(������) ---��������̶߳����״̬;
********************************************************/ 
void CThreadMgr::checkThread()
{
	map<unsigned, CThread*>::iterator it = m_ThreadList.begin();
	for (; it != m_ThreadList.end(); it++)
	{
		checkThread(*(it->second));	  
	}
}

/**************************************************************
* @����: checkThread(����) ---���ָ���̶߳����״̬;
*
* @����: threadObj ---�̶߳��������;
*
* @����ֵ: �����߳����е�״̬,��ο�ͷ�ļ��й��ں�Ķ���˵��;
**************************************************************/  
int CThreadMgr::checkThread(CThread &threadObj)
{	
	int iStateVal = TS_RUN_NOEXIST;		//״̬����ֵ;
	if (0 == (m_pThreadMgr->m_ThreadList).count(threadObj.getThreadId()))
		return iStateVal;

	unsigned  uiWorkTime  = threadObj.getWorkTime();
	unsigned  uiLastAlive = (unsigned)threadObj.getHeartBeat();
	unsigned  uiCurTime   = time(NULL);	
	string    szException = STR_STATE_UNKNOWN;  //״̬��ʾ;

	if (uiWorkTime != 0)
	{
		//���̶߳���״̬���жϼ�����.	
		switch (threadObj.getThreadState()) 
		{
		case e_TS_EXCEPTION:
			szException = STR_STATE_EXCEPTION; 
			iStateVal   = TS_RUN_EXCEPTION;
			break;
		case e_TS_UNKONWN:
		case e_TS_NORMAL:
		case e_TS_TIMEOUT:	
			if (uiCurTime - uiLastAlive > uiWorkTime) 	
			{
				threadObj.setThreadState(e_TS_TIMEOUT);
				szException = STR_STATE_TIMEOUT;
				iStateVal   = TS_RUN_TIMEOUT;
			}
			else
			{		  
				threadObj.setThreadState(e_TS_NORMAL);	
				iStateVal = TS_RUN_NORMAL;
				return iStateVal;
			};
			break;
		default: ;	    
		} 
	}  //end of if
	else
	{
		iStateVal = TS_RUN_UNKNOWN;
	}

	////���쳣д����־(��Ҫ����e_TS_EXCEPTION,e_TS_UNKNOWN,e_TS_TIMEOUT);
	//m_pLog->printLog(2, "%-d  %-s  %-d  %-s  %-s",
	//	           threadObj.getThreadId(), 
	//			   (lastAlive.ToString()).c_str(), 
	//			   uiWorkTime, 
	//			   szException.c_str(), 
	//			   (threadObj.getDescription()).c_str()
	//			   );

	return iStateVal;
}

/**************************************************************
* @����: printStateInfo(�ղ�) ---�ն˴�ӡ�����̵߳�����״̬;
**************************************************************/ 
void CThreadMgr::printStateInfo()
{
	map<unsigned, CThread*>::iterator it = m_ThreadList.begin();

	for (; it != m_ThreadList.end(); ++it)
	{
		printStateInfo(*(it->second));
	} 
}

/**************************************************************
* @����: printStateInfo(����) ---�ն˴�ӡָ���̵߳�����״̬;
*
* @����: threadObj ---�̶߳��������;
*
**************************************************************/ 
void CThreadMgr::printStateInfo( CThread & threadObj)
{
	unsigned uiWorkTime = threadObj.getWorkTime();
	unsigned uiLastAlive = (unsigned)threadObj.getHeartBeat();
	unsigned uiCurTime = time(NULL);
	string szState;
	string   szDescription = threadObj.getDescription();
	E_RUN_STATE eRunState = threadObj.getThreadState();//�̵߳�����״̬


	cout << setiosflags(ios::left) << threadObj.getThreadId() << "  ";
	cout << setiosflags(ios::left) << uiWorkTime << "  ";

	switch (eRunState )
	{
	case e_TS_UNKONWN : 
		szState = STR_STATE_UNKNOWN;
		break;
	case e_TS_EXCEPTION:
		szState = STR_STATE_EXCEPTION;
		break;
	case e_TS_NORMAL:
		szState = STR_STATE_NORMAL;
		break;
	case e_TS_TIMEOUT:
		szState = STR_STATE_TIMEOUT;
	}

	//Ϊ��״̬��Ϣ����һ��,�趨ͳһ���
	cout << setw(8) << setiosflags(ios::left) << szState << " ";
	cout << setiosflags(ios::left) << szDescription << endl;	
}

/******************************************************************
* @����: clearTransaction ---�ڸ����̴߳�������֮ǰ��������ղ���;
*******************************************************************/ 
int CThreadMgr::clearUp()
{
	map<unsigned, CThread*>::iterator it_ID = m_ThreadList.begin();
	map<unsigned, CThread*>::reverse_iterator it_Priority;
	map<unsigned, CThread*> map_Priority;
	unsigned uipriority = 0;

	try 
	{
		for (; it_ID != m_ThreadList.end(); it_ID++)
		{
			uipriority = (it_ID->second)->getPriority();
			map_Priority.insert(pair<unsigned, CThread*>(uipriority,it_ID->second));
		}

		it_Priority = map_Priority.rbegin();
		for ( ; it_Priority != map_Priority.rend(); it_Priority++)	
		{
			(it_Priority->second)->clearUp();
		}
		return 0;
	}
	catch(...)
	{
		return -1;
	}
}
