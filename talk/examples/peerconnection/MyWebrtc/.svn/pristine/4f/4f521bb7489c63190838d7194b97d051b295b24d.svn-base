#ifndef CSOCKET_BASE
#define CSOCKET_BASE
#include"DllInterface.h"
#include<string>
#include<list>
#include<map>
#include<vector>
#include<thread>
#include<mutex>
#include<condition_variable>
typedef std::mutex MUTEX;
typedef std::thread THANDLE;
typedef std::condition_variable   TEVENT;
////////////
EXTERN_C_BEGIN
class WEBRTCP2PDLLSHARED_EXPORT CSyncMutexBase
{
public:
	virtual ~CSyncMutexBase();
public:
	virtual bool GetLockState() = 0;
	virtual std::unique_lock<std::mutex> * GetLockPtr() = 0;
protected:
	virtual void Lock() = 0;
	virtual void UnLock() = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT CSyncMutex:public CSyncMutexBase
{
public:
	explicit CSyncMutex(MUTEX *cs);
	CSyncMutex() = delete;
	~CSyncMutex();
	virtual bool GetLockState() final;
	virtual std::unique_lock<std::mutex> * GetLockPtr();
protected:
	virtual void Lock() final;
	virtual void UnLock() final;
private:
	bool m_lockFlag;
	MUTEX *m_lock;
	std::unique_lock<std::mutex> *m_lock2;
};
DllExtern_Dynamic_Interface(CSyncMutexBase)
////////////
class WEBRTCP2PDLLSHARED_EXPORT CHandleBase
{
public:
	virtual ~CHandleBase();
public:
	virtual void SetEvent() = 0;
	virtual void ResetEvent() = 0;
	virtual bool GetState() = 0;
	virtual TEVENT * GetHandle() = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT CHandleBaseExe:public CHandleBase
{
public:
	CHandleBaseExe();
	virtual ~CHandleBaseExe();
public:
	virtual void SetEvent();
	virtual void ResetEvent();
	virtual bool GetState();
	virtual TEVENT* GetHandle();
private:
	TEVENT *m_handle;
	MUTEX *m_lock;
	bool m_flag;
};

DllExtern_Dynamic_Interface(CHandleBase)

EXTERN_C_END
#endif
