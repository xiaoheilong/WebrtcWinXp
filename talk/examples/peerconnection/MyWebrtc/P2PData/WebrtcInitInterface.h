#ifndef WEBRTC_INIT_INTERFACE
#define WEBRTC_INIT_INTERFACE
#pragma once
#include "DllInterface.h"
EXTERN_C_BEGIN
namespace rtc
{
	class Win32Thread;
}

class WebrtcWin32ThreadBase;
class WEBRTCP2PDLLSHARED_EXPORT WebrtcInitEnviBase
{
public:
	virtual ~WebrtcInitEnviBase() {}
public:
	virtual void Register(WebrtcWin32ThreadBase * thread) = 0;
};

class WebrtcWin32ThreadBase;
class WEBRTCP2PDLLSHARED_EXPORT WebrtcInitEnvironment:public WebrtcInitEnviBase
{
public:
	explicit WebrtcInitEnvironment();
	virtual ~WebrtcInitEnvironment();
public:
	virtual void Register(WebrtcWin32ThreadBase * thread);
private:
	WebrtcWin32ThreadBase * m_thread;
};
//DllExtern_Fixed_Interface(WebrtcInitEnviBase);
WEBRTCP2PDLLSHARED_EXPORT WebrtcInitEnviBase* CreateWebrtcInitEnviBase();

WEBRTCP2PDLLSHARED_EXPORT void ReleaseWebrtcInitEnviBase(WebrtcInitEnviBase * value);
//////////////////////

class WEBRTCP2PDLLSHARED_EXPORT WebrtcWin32ThreadBase
{
public:
	virtual ~WebrtcWin32ThreadBase();
public:
	virtual rtc::Win32Thread * GetThread() = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT WebrtcWin32Thread :public WebrtcWin32ThreadBase
{
public:
	WebrtcWin32Thread();
	virtual ~WebrtcWin32Thread();
public:
	virtual rtc::Win32Thread *  GetThread();
private:
	rtc::Win32Thread *m_thread;
};

DllExtern_Fixed_Interface(WebrtcWin32ThreadBase)
EXTERN_C_END
#endif
