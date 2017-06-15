#include"WebrtcInitInterface.h"
#include"defaults.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"
#include "webrtc/base/win32socketserver.h"
#include "webrtc/base/logging.h"
///////////
//WebrtcInitEnviBase::~WebrtcInitEnviBase()
//{
//
//}
///////////
WebrtcInitEnvironment::WebrtcInitEnvironment()
{
	m_thread = NULL;
	rtc::EnsureWinsockInit();
	//must in another thread, so it will not been block
	rtc::InitializeSSL();
}

WebrtcInitEnvironment::~WebrtcInitEnvironment()
{
	LOG(INFO) << __FUNCTION__ << "IS   RELEASE!";
	rtc::CleanupSSL();
	ReleaseWebrtcWin32ThreadBase(m_thread);
	LOG(INFO) << __FUNCTION__ << "IS   RELEASE FINISH!";
}

void WebrtcInitEnvironment::Register(WebrtcWin32ThreadBase * thread)
{
	m_thread = thread;
	if (!m_thread)
		return;
	rtc::ThreadManager::Instance()->SetCurrentThread(m_thread->GetThread());
}

///////////////
WEBRTCP2PDLLSHARED_EXPORT WebrtcInitEnviBase* CreateWebrtcInitEnviBase()
{
	WebrtcInitEnviBase *rtc = new WebrtcInitEnvironment();
	rtc->Register(CreateWebrtcWin32ThreadBase());
	return rtc;
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseWebrtcInitEnviBase(WebrtcInitEnviBase * value)
{
	DeletePtr(value);
}
///////
WebrtcWin32ThreadBase::~WebrtcWin32ThreadBase()
{
	
}

////////////
WebrtcWin32Thread::WebrtcWin32Thread()
{
	m_thread = new rtc::Win32Thread();
}

WebrtcWin32Thread::~WebrtcWin32Thread()
{
	m_thread->Quit();
	m_thread->UnwrapCurrent();
	LOG(INFO) <<__FUNCTION__<< "IS   RELEASE!" ;
	DeletePtr(m_thread);
	LOG(INFO) << __FUNCTION__ << "IS   RELEASE FINISH!";
}

rtc::Win32Thread * WebrtcWin32Thread::GetThread()
{
	return m_thread;
}
///////////
WEBRTCP2PDLLSHARED_EXPORT WebrtcWin32ThreadBase* CreateWebrtcWin32ThreadBase()
{
	return new WebrtcWin32Thread();
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseWebrtcWin32ThreadBase(WebrtcWin32ThreadBase * value)
{
	DeletePtr(value);
}
