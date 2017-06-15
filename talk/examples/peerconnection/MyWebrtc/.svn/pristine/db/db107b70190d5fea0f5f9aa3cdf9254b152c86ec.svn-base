#include "CHandleBase.h"
#include "defaults.h"
#include<process.h>
#include<algorithm>
//#include<sys/types.h>
//#include <fcntl.h>
////////////////
 CSyncMutexBase::~CSyncMutexBase()
{

}
//////////
CSyncMutex::CSyncMutex(MUTEX *cs)
{
	m_lock = NULL;
	if (!cs)
		return;
	m_lock = cs;
	m_lockFlag = false;
	Lock();
}

CSyncMutex::~CSyncMutex()
{
	UnLock();
	if (!m_lock)
		return;
}

void CSyncMutex::Lock()
{
	if (!m_lock)
		return;
	m_lock2 = new std::unique_lock<std::mutex>(*m_lock);
	m_lockFlag = true;
}

void CSyncMutex::UnLock()
{
	if (!m_lock || !m_lock2)
	{
		if (m_lock2 != NULL)
		{
			DeletePtr(m_lock2);
		}
		m_lock = NULL;
		return;
	}
	if (!m_lock2)
		return;
	m_lock2->unlock();
	DeletePtr(m_lock2);
	m_lock2 = NULL;
	m_lockFlag = false;
}

bool CSyncMutex::GetLockState()
{
	return m_lockFlag;
}

std::unique_lock<std::mutex> *CSyncMutex::GetLockPtr()
{
	return m_lock2;
}
/////////
WEBRTCP2PDLLSHARED_EXPORT CSyncMutexBase* CreateCSyncMutexBase(CSyncMutexBase*value)
{
	return value;
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseCSyncMutexBase(CSyncMutexBase * value)
{
	DeletePtr(value);
}
/////////////////
CHandleBase::~CHandleBase()
{

}
/////////////
CHandleBaseExe::CHandleBaseExe()
{
	m_handle = new std::condition_variable();
	m_lock = new MUTEX();
	m_flag = false;
}

CHandleBaseExe::~CHandleBaseExe()
{
	DeletePtr(m_handle);
	DeletePtr(m_lock);
	m_flag = false;
}


void CHandleBaseExe::SetEvent()
{
	CSyncMutex lock(m_lock);
	if (m_handle)
	{
		m_flag = true;
		m_handle->wait(*(lock.GetLockPtr()));
	}
}

void CHandleBaseExe::ResetEvent()
{
	CSyncMutex lock(m_lock);
	if (m_handle)
	{
		m_flag = false;
		m_handle->notify_all();
	}
}

bool CHandleBaseExe::GetState()
{
	CSyncMutex lock(m_lock);
	return m_flag;
}

TEVENT* CHandleBaseExe::GetHandle()
{
	return m_handle;
}
///////////////////////////////////
WEBRTCP2PDLLSHARED_EXPORT CHandleBase* CreateCHandleBase(CHandleBase* value)
{
	return value;
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseCHandleBase(CHandleBase* value)
{
	DeletePtr(value);
}
