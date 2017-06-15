/*
*  Copyright 2012 The WebRTC Project Authors. All rights reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/
#include "WebrtcClient.h"
#include "TransferData.h"
#include "webrtc/base/stringutils.h"
#include "webrtc/base/stringencode.h"
#include "webrtc/base/signalthread.h"
#include "webrtc/base/common.h"
#include "webrtc/base/nethelpers.h"
#include "webrtc/base/logging.h"
#include "webrtc/base/sigslot.h"
#include "webrtc/base/physicalsocketserver.h"
#include "webrtc/base/common.h"
#ifdef WIN32
#include "webrtc/base/win32socketserver.h"
#endif
namespace WebrtcClientTool
{
	// Delay between server connection retries, in milliseconds
	const int kReconnectDelay = 2000;

    WEBRTCP2PDLLSHARED_EXPORT rtc::AsyncSocket* CreateClientSocket(int family)
	{
#ifdef WIN32
		rtc::Win32Socket* sock = new rtc::Win32Socket();
		sock->CreateT(family, SOCK_STREAM);
		return sock;
#elif defined(WEBRTC_POSIX)
		rtc::Thread* thread = rtc::Thread::Current();
		ASSERT(thread != NULL);
		return thread->socketserver()->CreateAsyncSocket(family, SOCK_STREAM);
#else
#error Platform not supported.
#endif
	}

    WEBRTCP2PDLLSHARED_EXPORT size_t SpringtFn(char*buffer, size_t size, char *format, ...)
	{
		va_list args;
		va_start(args, format);
		size_t len = rtc::vsprintfn(buffer, size, format, args);
		va_end(args);
		return len;
	}

}  // namespace

const uint16_t kDefaultServerPort = 8888;
/////////////
#define CreateSTLString TransferNamespace::CreateSTLString 
#define CreateSTLStringEx TransferNamespace::CreateSTLStringEx
#define DeleteSTLString TransferNamespace::DeleteSTLString
////////////////////
WebrtcClientOriginal::~WebrtcClientOriginal()
{

}
////////////////
WebrtcClientObserver::~WebrtcClientObserver()
{

}
//////////////
class  WebrtcClientBase :public WebrtcClientOriginalInner,public rtc::MessageHandler, public sigslot::has_slots<>
{
public:
	WebrtcClientBase();
	virtual ~WebrtcClientBase();
public:
	friend class WebrtcClientOriginalInner;
	virtual void Close();
	virtual void OnConnect(rtc::AsyncSocket* socket);

	virtual void OnResolveResult(rtc::AsyncResolverInterface* resolver);

	virtual void OnMessage(rtc::Message* msg);

	virtual bool SignOut();

	virtual void DoConnect();

	virtual bool ConnectControlSocket();

	virtual bool IsSendingMessage();

	virtual void OnClose(rtc::AsyncSocket* socket, int err);
	/////////////////
	virtual void InitSocketSignals();

	virtual void Connect(const std::string& server, int port, const std::string& client_name);
	virtual void* GetMessageHandler() final;

	virtual  void SetParent(const WebrtcClientOriginal * parent);
	virtual const WebrtcClientOriginal * GetParent();

	virtual WebrtcClientOriginalOutter * GetOutter();
	virtual void SetOutter(WebrtcClientOriginalOutter * value);
	//
public://BridgeForClientInnerOutter
	virtual void * GetServerSocketAddress();
	virtual void* GetAsyncResolver();
	virtual void* GetControlSocket();
	virtual void* GetHangingGetSocket();
	virtual STLStringShaow * GetConnectData();
	virtual STLStringShaow * GetControlData();
	virtual STLStringShaow * GetNotifycationData();
	virtual STLStringShaow * GetClientName();
	virtual State * const GetConnectState();
public:
	virtual void OnHangingGetConnect(rtc::AsyncSocket* socket);
	virtual void OnRead(rtc::AsyncSocket* socket) ;
	virtual void OnHangingGetRead(rtc::AsyncSocket* socket);
protected:
	rtc::SocketAddress *m_serverAddress;							//server_address_
	rtc::AsyncResolver* m_resolver;								//resolver_
	rtc::scoped_ptr<rtc::AsyncSocket> m_controlSocket;			//control_socket_
	rtc::scoped_ptr<rtc::AsyncSocket> m_hangingGet;				//hanging_get_
	STLStringShaow *m_connectData;									//m_connectData
	STLStringShaow *m_controlData;									//control_data_
	STLStringShaow *m_notifycationData;								//notification_data_
	STLStringShaow *m_clientName;									//client_name_
	State m_state;												//state_
	WebrtcClientOriginal * m_parent;
};
///////////////////
WEBRTCP2PDLLSHARED_EXPORT WebrtcClientOriginalInner * CreateWebrtcClientOriginalInner()
{
	return new WebrtcClientBase();
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseWebrtcClientOriginalInner(WebrtcClientOriginalInner * value)
{
	DeletePtr(value);
}
//////////////////////////
WebrtcClientBase::WebrtcClientBase()
{
	m_resolver = NULL;
	m_state = NOT_CONNECTED;
	m_parent = NULL;
	m_outter = NULL;
	m_connectData = CreateSTLString();
	m_controlData = CreateSTLString();
	m_notifycationData = CreateSTLString();
	m_clientName = CreateSTLString();
	m_serverAddress = new rtc::SocketAddress();
}

WebrtcClientBase::~WebrtcClientBase()
{
	DeleteSTLString(m_connectData);
	DeleteSTLString(m_controlData);
	DeleteSTLString(m_notifycationData);
	DeleteSTLString(m_clientName);
	DeletePtr(m_serverAddress);
	m_parent = NULL;
	m_state = NOT_CONNECTED;
}

void WebrtcClientBase::Close()
{
	if (m_controlSocket.get())
		m_controlSocket->Close();
	if (m_hangingGet.get())
		m_hangingGet->Close();
	m_connectData->clear();
	if (m_resolver != NULL)
	{
		m_resolver->Destroy(false);
		m_resolver = NULL;
	}
	if (m_outter)
	{
		m_outter->ClearClientKey();
		m_outter->SetInner(NULL);
	}
	else
		LOG(INFO) << "m_outter is NULL!";
	m_state = NOT_CONNECTED;
}

void WebrtcClientBase::OnConnect(rtc::AsyncSocket* socket)
{
	if (!m_connectData)
		return;
	ASSERT(m_connectData->IsValid());
	size_t sent = socket->Send(m_connectData->GetString().c_str(), m_connectData->GetString().length());
	ASSERT(sent == m_connectData->GetString().length());
	RTC_UNUSED(sent);
	m_connectData->clear();
}

void WebrtcClientBase::OnResolveResult(rtc::AsyncResolverInterface* resolver)
{
	if (m_resolver->GetError() != 0)
	{
		if (m_parent)
			m_parent->OnServerConnectionFailure();
		else
		{
			LOG(INFO) << "m_parent is NULL!";
			return;
		}
		m_resolver->Destroy(false);
		m_resolver = NULL;
		m_state = NOT_CONNECTED;
	}
	else
	{
		if (!m_serverAddress)
			return;
		*m_serverAddress = m_resolver->address();
		DoConnect();
	}
}

void WebrtcClientBase::OnMessage(rtc::Message* msg)
{
	//DoConnect();
}

bool WebrtcClientBase::SignOut()
{
	if (m_state == NOT_CONNECTED || m_state == SIGNING_OUT)
		return true;

	if (m_hangingGet->GetState() != rtc::Socket::CS_CLOSED)
		m_hangingGet->Close();
	if (m_controlSocket->GetState() == rtc::Socket::CS_CLOSED)
	{
		m_state = SIGNING_OUT;
		if (m_outter)
		{
			if (m_outter->IsConnectSuccese())//clientKey is valid 
			{
				m_outter->SignOutHeaderMsg();
				return ConnectControlSocket();
			}
			else
			{
				// Can occur if the app is closed before we finish connecting.
			}
		}
		else
		{
			LOG(INFO) << "m_outter is NULL!";
			return false;
		}
	}
	else
	{
		m_state = SIGNING_OUT_WAITING;
	}

	return true;
}

void WebrtcClientBase::DoConnect()
{
	if (!m_serverAddress)
		return;
	m_controlSocket.reset(CreateClientSocket(m_serverAddress->ipaddr().family()));
	m_hangingGet.reset(CreateClientSocket(m_serverAddress->ipaddr().family()));
	InitSocketSignals();
	if(m_outter)
		m_outter->DoConnectHeaderMsg();
	else
	{
		LOG(INFO) << "m_outter is NULL!";
		return;
	}
	bool ret = ConnectControlSocket();
	if (ret)
		m_state = SIGNING_IN;
	if (!ret)
	{
		m_parent->OnServerConnectionFailure();
	}
}

bool WebrtcClientBase::ConnectControlSocket()
{
	ASSERT(m_controlSocket->GetState() == rtc::Socket::CS_CLOSED);
	if (!m_serverAddress)
		return false;
	int err = m_controlSocket->Connect(*m_serverAddress);
	if (err == SOCKET_ERROR)
	{
		Close();
		return false;
	}
	return true;
}

bool WebrtcClientBase::IsSendingMessage()
{
	return m_state == CONNECTED &&m_controlSocket->GetState() != rtc::Socket::CS_CLOSED;
}

void WebrtcClientBase::OnClose(rtc::AsyncSocket* socket, int err)
{
	LOG(INFO) << __FUNCTION__;
	socket->Close();
#ifdef WIN32
	if (err != WSAECONNREFUSED)
	{
#else
	if (err != ECONNREFUSED)
	{
#endif
		if (socket == m_hangingGet.get())
		{
			if (m_state == CONNECTED)
			{
				m_hangingGet->Close();
				if (!m_serverAddress)
					return;
				m_hangingGet->Connect(*m_serverAddress);
			}
		}
		else
		{
			if (!m_parent)
			{
				LOG(INFO) << "m_parent is NULL!";
				return;
			}
			m_parent->OnMessageSent(err);
		}
	}
	else
	{
		if (socket == m_controlSocket.get())
		{
			LOG(WARNING) << "Connection refused; retrying in 2 seconds";
			rtc::Thread::Current()->PostDelayed(WebrtcClientTool::kReconnectDelay, this, 0);
		}
		else
		{
			Close();
			if (!m_parent)
			{
				LOG(WARNING) << "m_parent is NULL!";
				return;
			}
			m_parent->OnDisconnected();
		}
	}
}

void  WebrtcClientBase::InitSocketSignals()
{
	ASSERT(m_controlSocket.get() != NULL);
	ASSERT(m_hangingGet.get() != NULL);
	m_controlSocket->SignalCloseEvent.connect(this, &WebrtcClientBase::OnClose);//<clientKey, clientValue>
	m_hangingGet->SignalCloseEvent.connect(this, &WebrtcClientBase::OnClose);
	m_controlSocket->SignalConnectEvent.connect(this, &WebrtcClientBase::OnConnect);
	if (!m_outter)
	{
		LOG(INFO) << "m_outter is NULL!";
		return;
	}
	m_hangingGet->SignalConnectEvent.connect(this, &WebrtcClientBase::OnHangingGetConnect);
	m_controlSocket->SignalReadEvent.connect(this, &WebrtcClientBase::OnRead);
	m_hangingGet->SignalReadEvent.connect(this, &WebrtcClientBase::OnHangingGetRead);
}

void  WebrtcClientBase::Connect(const std::string& server, int port, const std::string& client_name)
{
	ASSERT(!server.empty());
	ASSERT(!client_name.empty());

	if (m_state != NOT_CONNECTED)
	{
		LOG(WARNING)
			<< "The client must not be connected before you can call Connect()";
		if (!m_parent)
		{
			LOG(INFO)<< "m_parent is NULL!";
			return;
		}
		m_parent->OnServerConnectionFailure();
		return;
	}

	if (server.empty() || client_name.empty())
	{
		if (!m_parent)
		{
			LOG(INFO) << "m_parent is NULL!";
			return;
		}
		m_parent->OnServerConnectionFailure();
		return;
	}

	if (port <= 0)
		port = kDefaultServerPort;
	if (!m_serverAddress)
		return;
	m_serverAddress->SetIP(server);
	m_serverAddress->SetPort(port);
	m_clientName->SetString(client_name);

	if (m_serverAddress->IsUnresolvedIP())// if the m_serverAddress.IsUnresolvedIP is not e valid IP
	{
		m_state = RESOLVING;
		m_resolver = new rtc::AsyncResolver();
		m_resolver->SignalDone.connect(this, &WebrtcClientBase::OnResolveResult);
		m_resolver->Start(*m_serverAddress);
	}
	else
	{
		DoConnect();
	}
}

void* WebrtcClientBase::GetMessageHandler()
{
	return this;
}

void WebrtcClientBase::SetParent(const WebrtcClientOriginal * parent)
{
	m_parent = const_cast<WebrtcClientOriginal *>(parent);
}

const WebrtcClientOriginal * WebrtcClientBase::GetParent()
{
	return m_parent;
}

WebrtcClientOriginalOutter * WebrtcClientBase::GetOutter()
{
	return m_outter;
}

void WebrtcClientBase::SetOutter(WebrtcClientOriginalOutter * value)
{
	m_outter = value;
}

void * WebrtcClientBase::GetServerSocketAddress()
{
	return m_serverAddress;
}

void* WebrtcClientBase::GetAsyncResolver()
{
	return m_resolver;
}

void* WebrtcClientBase::GetControlSocket()
{
	return &m_controlSocket;
}

void* WebrtcClientBase::GetHangingGetSocket()
{
	return &m_hangingGet;
}

STLStringShaow * WebrtcClientBase::GetConnectData()
{
	return m_connectData;
}

STLStringShaow * WebrtcClientBase::GetControlData()
{
	return m_controlData;
}

STLStringShaow * WebrtcClientBase::GetNotifycationData()
{
	return m_notifycationData;
}

STLStringShaow * WebrtcClientBase::GetClientName()
{
	return m_clientName;
}

State *  const WebrtcClientBase::GetConnectState()
{
	return &m_state;
}


void WebrtcClientBase::OnHangingGetConnect(rtc::AsyncSocket* socket)
{
	if (m_outter)
		m_outter->SignOutHeaderMsg();
}

void WebrtcClientBase::OnRead(rtc::AsyncSocket* socket)
{
	if (m_outter)
		m_outter->DoConnectHeaderMsg();
}

void WebrtcClientBase::OnHangingGetRead(rtc::AsyncSocket* socket)
{
	if (m_outter)
		m_outter->OnHangingGetRead(socket);
}
/////////////
class WEBRTCP2PDLLSHARED_EXPORT WebrtcClientOriginalExe :public WebrtcClientOriginal
{
public:
	explicit WebrtcClientOriginalExe(WebrtcClientOriginalInner * inner, WebrtcClientOriginalOutter * outter);
	WebrtcClientOriginalExe() = delete;
	virtual  ~WebrtcClientOriginalExe();
public://WebrtcClientObserver
	virtual void OnSignedIn(void * param);  // Called when we're logged on.
	virtual void OnDisconnected();
	virtual void OnPeerConnected(void *id);
	virtual void OnPeerDisconnected(void *peer_id);
	virtual void OnPeerConnectedServer(void* id);
	virtual void OnPeerDisConnectedServer(void* id);
	virtual void OnMessageFromPeer(void *peer_id, const std::string& message);
	virtual void OnMessageSent(int err);
	virtual void OnServerConnectionFailure();

	virtual WebrtcClientOriginalInner * const GetClientInner();
	virtual WebrtcClientOriginalOutter * const GetClientOutter();
	virtual void RegisterObserver(WebrtcClientObserver* const callback);
protected:
	WebrtcClientOriginalInner * m_clientInner;
	WebrtcClientOriginalOutter * m_clientOutter;
	WebrtcClientObserver * m_callback;
};
///////////////////
WebrtcClientOriginalExe::WebrtcClientOriginalExe(WebrtcClientOriginalInner * inner, WebrtcClientOriginalOutter * outter)
{
	m_clientInner = const_cast<WebrtcClientOriginalInner *>(inner);
	m_clientOutter = const_cast<WebrtcClientOriginalOutter*>(outter);
	if (m_clientInner)
		m_clientInner->SetOutter(m_clientOutter);
	if (m_clientOutter)
		m_clientOutter->SetInner(m_clientInner);
	m_callback = NULL;
}

WebrtcClientOriginalExe::~WebrtcClientOriginalExe()
{
	/*m_clientInner = NULL;*/
	DeletePtr(m_clientInner);
	m_clientOutter = NULL;
	m_callback = NULL;
}
///WebrtcClientOriginalOutter

void WebrtcClientOriginalExe::OnSignedIn(void * param)
{
    if(m_callback )
        m_callback->OnSignedIn(param);
}

void WebrtcClientOriginalExe::OnDisconnected()
{
    if(m_callback )
        m_callback->OnDisconnected();
}

void WebrtcClientOriginalExe::OnPeerConnected(void *id)
{
    if(m_callback )
        m_callback->OnPeerConnected(id);
}

void WebrtcClientOriginalExe::OnPeerDisconnected(void *peer_id)
{
    if(m_callback )
        m_callback->OnPeerDisconnected(peer_id);
}

void WebrtcClientOriginalExe::OnPeerConnectedServer(void* id)
{
    if(m_callback )
        m_callback->OnPeerConnectedServer(id);
}

void WebrtcClientOriginalExe::OnPeerDisConnectedServer(void* id)
{
    if(m_callback )
        m_callback->OnPeerDisConnectedServer(id);
}

void WebrtcClientOriginalExe::OnMessageFromPeer(void *peer_id, const std::string& message)
{
    if(m_callback )
        m_callback->OnMessageFromPeer(peer_id, message);
}

void WebrtcClientOriginalExe::OnMessageSent(int err)
{
    if(m_callback )
        m_callback->OnMessageSent(err);
}

void WebrtcClientOriginalExe::OnServerConnectionFailure()
{
    if(m_callback )
        m_callback->OnServerConnectionFailure();
}

WebrtcClientOriginalInner *  const WebrtcClientOriginalExe::GetClientInner()
{
	return m_clientInner;
}

WebrtcClientOriginalOutter *  const WebrtcClientOriginalExe::GetClientOutter()
{
	return m_clientOutter;
}

void WebrtcClientOriginalExe::RegisterObserver(WebrtcClientObserver* const callback)
{
	m_callback = callback;
}

WEBRTCP2PDLLSHARED_EXPORT WebrtcClientOriginal* CreateWebrtcClientOriginal(WebrtcClientOriginalInner *value1, WebrtcClientOriginalOutter *value2)
{
	return new WebrtcClientOriginalExe(value1, value2);
}

WEBRTCP2PDLLSHARED_EXPORT void  ReleaseWebrtcClientOriginal(WebrtcClientOriginal * value)
{
	DeletePtr(value);
}
//////////
WebrtcClientOriginalOutter::~WebrtcClientOriginalOutter()
{
    if(m_inner)
        m_inner->SetOutter(NULL);
}
////
WebrtcClientOriginalInner::~WebrtcClientOriginalInner() 
{
    if(m_outter)
        m_outter->SetInner(NULL);
}
