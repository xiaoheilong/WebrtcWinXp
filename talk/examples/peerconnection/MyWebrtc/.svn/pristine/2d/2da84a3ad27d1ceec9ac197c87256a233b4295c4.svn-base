#include "CThread.h"
#include <thread>
//#include "Sower/Core/CGlbs.h"

void sleep(long long t)
{
    std::chrono::milliseconds timespan(t);
    std::this_thread::sleep_for(timespan);
}


void ThreadProc(CThread* p) {
    p->Execute();
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////
THREAD_LOCK::THREAD_LOCK()
{

}

THREAD_LOCK::~THREAD_LOCK()
{

}


void THREAD_LOCK::Lock()
{
    m_lock.lock();
}

void THREAD_LOCK::UnLock()
{
    m_lock.unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAutoSync::CAutoSync(THREAD_LOCK *plock)
{
    pLock = plock;
    pLock->Lock();
}

CAutoSync::~CAutoSync()
{
    pLock->UnLock();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




CThread::CThread()
{
    isCancelThread = false;
    ThreadCount = 1;
}

CThread::~CThread()
{
    Stop();
}

void CThread::Start()
{
    isCancelThread = false;
    CAutoSync sync(&lock_threads);
    for(size_t i=threads.size();i<(size_t)ThreadCount;i++)
    {
        THREAD_INFO* p = new THREAD_INFO();
        p->pThread = new std::thread(ThreadProc,this);
        p->ThreadId = p->pThread->get_id();
        threads.push_back(p);
        p->pThread->detach();//启动线程
    }
}

void CThread::Stop()
{
    isCancelThread = true;

    while(true)
    {
        size_t num = 0;

        {
            CAutoSync sync(&lock_threads);
            num = threads.size();
        }

        if(num==0)
            break;
        sleep(300);
    }
}

void CThread::SetThreadCount(int count)
{
    ThreadCount = count;
}

void CThread::Execute()
{
    run();

    CAutoSync sync(&lock_threads);
    thread::id threadid = this_thread::get_id();
    list<THREAD_INFO*>::iterator obs;
    for(obs=threads.begin();obs!=threads.end();obs++)
    {
        THREAD_INFO* p = *obs;
        if(threadid == p->ThreadId)
        {
            threads.erase(obs);
            delete p->pThread;
            break;
        }
    }
}

///////////////////
//CThreadComRTCThread::CThreadComRTCThread()
//{
//
//}