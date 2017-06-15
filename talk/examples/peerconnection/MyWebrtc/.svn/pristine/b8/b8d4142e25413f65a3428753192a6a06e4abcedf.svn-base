#ifndef CTHREAD_H
#define CTHREAD_H
//#include "Sower/stdafx.h"
#include <string>

using namespace std;

//#if defined(UNICODE) || defined(_UNICODE)
typedef wstring UnicodeString;
//#else
//typedef string UnicodeString;
//#endif

typedef long long int64;

#include <list>
#include <mutex>
#include <thread>


class THREAD_LOCK
{
private:
    mutex m_lock;
public:
    THREAD_LOCK();

    ~THREAD_LOCK();

    void Lock();
    void UnLock();
};

class CAutoSync
{
private:
    THREAD_LOCK* pLock;

public:
    CAutoSync(THREAD_LOCK *plock);
    ~CAutoSync();
};


struct THREAD_INFO
{
    thread::id ThreadId;
    std::thread* pThread;
};


class CThread
{
public:
    CThread();
    ~CThread();
private:
    int ThreadCount;
    list<THREAD_INFO*> threads;
    THREAD_LOCK lock_threads;
protected:
    bool isCancelThread;
public:
    void Start();
    void Stop();
    void SetThreadCount(int count);
public:
    void Execute();
protected:
    virtual void run() = 0;
};

///////////
//class CThreadComRTCThread:public CThread
//{
//public:
//	explicit CThreadComRTCThread();
//	virtual ~CThreadComRTCThread();
//public:
//	void Start();
//protected:
//	void * m_decoratorPtr;
//};

///////
#endif // CTHREAD_H
