#ifndef _CLASS_THREAD_MGR
#define _CLASS_THREAD_MGR


#include "General.hpp"
#include "Thread.hpp"

/************************************************************
* ���º궨���Ǻ���checkThread�ķ���״ֵ̬�����庬������:
* TS_RUN_NOEXIST   : Ҫ�����̶߳����ѱ��̹߳��������;
* TS_RUN_NORMAL    : �߳���������;
* TS_RUN_EXCEPTION : �߳������쳣,�����߳���ֹ;
* TS_RUN_UNKNOWN   : �߳�δ����ҵ����ʱ��;
* TS_RUN_TIMEOUT   : �߳����г�ʱ,�ɲο�e_TS_TIMEOUT;
*************************************************************/
#define TS_RUN_NOEXIST         -1
#define TS_RUN_NORMAL           0  
#define TS_RUN_EXCEPTION        1
#define TS_RUN_UNKNOWN          2
#define TS_RUN_TIMEOUT          3

//���º����߳�״̬��ʾ��Ϣ
#define STR_STATE_NORMAL     "Normal"
#define STR_STATE_TIMEOUT    "TimeOut"
#define STR_STATE_EXCEPTION  "Error"
#define STR_STATE_UNKNOWN    "Unknown"



class CThread;

class CThreadMgr
{
	friend class CThread;
public:

	virtual ~CThreadMgr();

	static CThreadMgr* createInstance();   //�����̹߳�����ָ��;

	int  getThreadCount();

	void checkThread();  //������е��̶߳���;
	int  checkThread(CThread &threadObj);  //��ָ��������м��;

	void printStateInfo();  //��ӡ�̹߳������������̵߳�״̬��Ϣ;
	void printStateInfo(CThread &threadObj); //��ӡָ���̵߳�״̬��Ϣ;

	int  clearUp();  //�߳���������֮ǰ����ղ���;

private:    
	CThreadMgr();

private:
	static CThreadMgr *m_pThreadMgr;
	int  m_iThreadCount;
	map<unsigned, CThread*> m_ThreadList;
};

#endif
