#include"SocketDecorator.h"
#include"./defaults.h"
#ifdef WIN32
#include "webrtc/base/win32socketserver.h"
#endif

class SocketDecorator:public SocketBase
{
public:
	explicit SocketDecorator(void * socket);
	virtual ~SocketDecorator();
public:
	virtual int Send(char * buffer, int len);
	virtual int Recv(char* buffer, int len) ;
	virtual void Close();
	virtual int GetState() ;
	virtual bool Connect(void* server) ;
	virtual bool IsCloseState();
private:
	rtc::AsyncSocket * m_socket;
};

SocketDecorator::SocketDecorator(void* socket)
{
	m_socket = static_cast<rtc::AsyncSocket *>(socket);
}

SocketDecorator::~SocketDecorator()
{
	
}

int SocketDecorator::Send(char * buffer, int len)
{
	if (!m_socket)
		return 0;
	return m_socket->Send(buffer, len);
}

int SocketDecorator::Recv(char* buffer, int len)
{
	if (!m_socket)
		return 0;
	return m_socket->Recv(buffer, len);
}

void SocketDecorator::Close()
{
	if (m_socket)
		m_socket->Close();
}

int SocketDecorator::GetState()
{
	if (!m_socket)
		return -1;
	return m_socket->GetState();
}

bool SocketDecorator::Connect(void* server)
{
	if (!m_socket)
		return false;
	rtc::SocketAddress * server1 = static_cast<rtc::SocketAddress *>(server);
	if (!server1)
		return false;
	return m_socket->Connect(*server1);
}

bool SocketDecorator::IsCloseState()
{
	if (!m_socket)
		return false;
	if (m_socket->GetState() != rtc::Socket::CS_CLOSED)
		return false;
	return true;
}
/////////////
WEBRTCP2PDLLSHARED_EXPORT SocketBase* CreateSocketBase(void* value)
{
	return new SocketDecorator(value);
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseSocketBase(SocketBase * socket)
{
	DeletePtr(socket);
}