//#define _WIN32_WINNT (0x0500)

#include "ResLock.hpp"



//��ʼ�������� �� �ٽ���
CResLock::CResLock()
{
#ifndef _WIN32
	if (0 != pthread_mutex_init(&m_mutex, NULL))
	{
		perror("pthread_mutex_init error");
	}	
#else
	::InitializeCriticalSection(&m_mutex);
#endif		
}


//���ٻ����� �� �ٽ���
CResLock::~CResLock()
{
#ifndef _WIN32
	if ((0 != pthread_mutex_destroy(&m_mutex)) && \
		(EBUSY != pthread_mutex_destroy(&m_mutex)))
	{
		perror("pthread_mutex_destroy error");
	}
#else
	::DeleteCriticalSection(&m_mutex);
#endif	
}


//�ӻ����� �� �����ٽ���
void CResLock::lock()
{
#ifndef _WIN32
    if (0 != pthread_mutex_lock(&m_mutex))
    {
        perror("pthread_mutex_lock error");
        switch (errno)
        {
            case EINVAL:
                printf("[ERROR]: EINVAL.\n");
                break;
            case EBUSY:
                printf("[ERROR]: EBUSY. \n");
                break;
            case EAGAIN:
                printf("[ERROR]: EAGAIN. \n");
                break;
            case EDEADLK:
                printf("[ERROR]: EDEADLK. \n");
                break;
            case EPERM:
                printf("[ERROR]:EPERM. \n");
                break;
            default:
                ;
        }
    }
#else
    ::EnterCriticalSection(&m_mutex);
#endif
}


//���������Լ��� �� ���Խ����ٽ���
int CResLock::tryLock()
{
	int iValue = 0;
#ifndef _WIN32
	iValue = pthread_mutex_trylock(&m_mutex);	//��ȡ������������ֵ
	if ((0 != iValue) && (EBUSY != iValue))
	{
		perror("pthread_mutex_trylock error");
	}
#else
	iValue = (int)(!::TryEnterCriticalSection(&m_mutex)); //�ɹ��򷵻�0
#endif
	return iValue;	//���ؼӲ�����״̬���ٽ���״̬
}


//���������� �� �뿪�ٽ���
void CResLock::unLock()
{
#ifndef _WIN32
	if (0 != pthread_mutex_unlock(&m_mutex))
	{
		perror("pthread_mutex_unlock error");
	}
#else
	::LeaveCriticalSection(&m_mutex);
#endif
}
