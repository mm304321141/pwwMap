

#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#ifndef WIN32
#include <pthread.h>
#endif


#include "ThreadMgr.hpp"


/************************************************************************
*    e_TS_NORMAL�� 	 ��ʾ�߳���������;
*    e_TS_TIMEOUT��  ��ʾ�߳��쳣,���̹߳��������ʱ�����߳�ĩ������ʱ��
*	                 ��ʱ�����˹������ʱ�����õ�ҵ����ʱ��;
*	 e_TS_UNKNOWN��  ��ʾ�߳�����״̬δ֪,��������̶߳���ʱ��û�д���
*					 ҵ����ʱ��,�̹߳���������߳�״̬�����ж�;
*	 e_TS_EXCEPTION����ʾ�����쳣����,�����߳̽���;
*    e_TS_EXIT:      ��ʾ�߳������˳�;
**************************************************************************/
enum E_RUN_STATE{e_TS_NORMAL, e_TS_UNKONWN, e_TS_TIMEOUT, e_TS_EXCEPTION, e_TS_EXIT};



//Windowsƽ̨�µ��߳����ȼ�
enum E_TS_PRIORITY {THREAD_PRIORITY_IDLEX = -15, 
THREAD_PRIORITY_ERROR_RETURNX = 0x7fffffff,
THREAD_PRIORITY_LOWESTX = -2, 
THREAD_PRIORITY_BELOW_NORMALX = -1,
THREAD_PRIORITY_NORMALX = 0, 
THREAD_PRIORITY_ABOVE_NORMALX = 1, 
THREAD_PRIORITY_HIGHESTX = 2, 
THREAD_PRIORITY_TIME_CRITICALX = 15};


/***********************�߳����Խṹ��**************************
* @��������:
*	argSzDescrip: �̵߳Ĺ���������;
*	argClrPrior : �̵߳����ȼ�;
***************************************************************/
typedef struct ThreadAttribute
{
	ThreadAttribute(string argSzDescrip = "", unsigned argClrPrior = 0, 
		int corePrior =THREAD_PRIORITY_NORMALX)
	{
		sysPriority = corePrior;
		ui_ClrPriority = argClrPrior;
		szDescription = argSzDescrip;
	}

	string        szDescription ;
	unsigned      ui_ClrPriority;		//�̹߳���������ʱ�ķ�������ȼ�;
	int           sysPriority;		//����ϵͳ�ں˵����ȼ�	  
} SThreadAttrib, *pThreadAttrib;


class CThreadMgr;

class CThread
{
public:
	CThread(pThreadAttrib pAttrib = NULL, CThreadMgr *pThreadMgr = NULL,
		unsigned uiWorkTime = 0);

	virtual ~CThread();

	virtual int open(void *args = 0);

	virtual void stop();

	virtual int clearUp();			//�߳���մ��������

	void sleep(unsigned long dwMilliseconds);

	unsigned   getThreadId();		//��ȡ�߳�ID��ʶ��;

	time_t	   getHeartBeat();	    //��ȡ�߳����һ�ε�����;

	string     getDescription();    //��ȡ�̵߳Ĺ�������;

	unsigned   getWorkTime();       //��ȡ�̵߳�ҵ����ʱ��;

	int   getSysPriority();			//��ȡ�̹߳���ϵͳ�ں����õ����ȼ�;

	unsigned   getPriority();		//��ȡ�߳�Ϊ�̹߳������ڽ�����ղ��������õ����ȼ�;

	E_RUN_STATE getThreadState();	//��ȡ�߳�����״̬;

	void  setThreadState(E_RUN_STATE thrRunState);  //�����̵߳�����״̬;

	void  sendHeartBeat();          //���̹߳�������������;


protected:
	virtual int svc() = 0;				

	void *m_thread_args;


#ifdef WIN32
	static  unsigned __stdcall thread_proxy(void*);
	void *m_thread_handle;
#else
	static void* thread_proxy(void*);
	pthread_t m_thread_handle;
	pthread_attr_t m_threadAttrib;
#endif	


private:
	CThread(const CThread&);

	const CThread& operator = (const CThread&);

	void  regThread();     //���̹߳�����ע��;

	void  unRegThread();   //���̹߳�����ע��(��ע��);

private:
	unsigned    m_uiThreadId;
	time_t      m_LastHeartBeat;
	string      m_szDescription;
	CThreadMgr  *m_pThreadMgr;
	E_RUN_STATE m_ThreadState;		//�߳�����״̬;
	unsigned    m_uiWorkTime;		//�̺߳���һ��ҵ����ʱ��;
	unsigned    m_uiPriority;		//�߳�Ϊ�̹߳������������ղ������õ����ȼ�;
	int     	m_iSysPriority;		//�̻߳���ϵͳ�ں˵����ȼ�;
};



#endif

