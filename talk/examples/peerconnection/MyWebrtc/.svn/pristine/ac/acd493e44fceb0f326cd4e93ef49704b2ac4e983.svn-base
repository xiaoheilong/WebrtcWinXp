#include"WebrtcClientExe.h"
#include"TransferData.h"
#include"SocketDecorator.h"
#include<assert.h>
////////////
const char kByeMessage[] = "BYE";
#define sprintfn WebrtcClientTool::SpringtFn
////////////
WebrtcClient::WebrtcClient(WebrtcClientObserver * call)
{
	m_myID = -1;
	m_oppositePeerID = -1;
	m_parent = NULL;
	m_inner = NULL;
}

WebrtcClient::~WebrtcClient()
{
	Close();
	m_parent = NULL;
}

bool WebrtcClient::IsConnectSuccese() const
{
	return IsValidClientKey((void*)(&m_myID));
}

void WebrtcClient::ClearClientKey()  //must been override  
{
	m_myID = -1; //reset the client primary key
}

bool WebrtcClient::IsValidClientKey(void * key)const
{
	int * key_ = static_cast<int *>(key);
	if (key_ && *key_ > 0)
		return true;
	return false;
}

bool WebrtcClient::IsCurrentOppositeID(void * id)const
{
	int *peerid = static_cast<int *>(id);
	if (peerid && m_oppositePeerID == *peerid)//when the two pointer point value  is same ,then return true
		return true;
	return false;
}

void * WebrtcClient::GetClientKey()
{
	return &m_myID;
}

void WebrtcClient::SignOutHeaderMsg() ////child class could override the fuction
{
	char buffer[1024];
	sprintfn(buffer, sizeof(buffer),
		"GET /sign_out?peer_id=%i HTTP/1.0\r\n\r\n", m_myID);
	if (!m_inner)
		return;
	//m_connectData->SetString(buffer);
	STLStringShaow *str = m_inner->GetConnectData();
	if (str)
		str->SetString(buffer);
}

void WebrtcClient::DoConnectHeaderMsg()////child class could override the fuction
{
	char buffer[1024];
	if (!m_inner)
		return;
	STLStringShaow * name = m_inner->GetClientName();
	STLStringShaow * connectData = m_inner->GetConnectData();
	if (!name || connectData)
		return;
	//sprintfn(buffer, sizeof(buffer),
	//	"GET /sign_in?%s HTTP/1.0\r\n\r\n", m_clientName->GetString().c_str());
	sprintfn(buffer, sizeof(buffer),
		"GET /sign_in?%s HTTP/1.0\r\n\r\n", name->GetString().c_str());
	/*m_connectData ->SetString(buffer);*/
	connectData->SetString(buffer);
}
/////@@callback
void WebrtcClient::OnHangingGetConnect(void* socket1)//could been override
{
	//rtc::AsyncSocket * socket = static_cast<rtc::AsyncSocket *>(socket1);
	SocketBase *socket = CreateSocketBase(socket1);
	if (!socket)
		return;
	char buffer[1024];
	sprintfn(buffer, sizeof(buffer),
		"GET /wait?peer_id=%i HTTP/1.0\r\n\r\n", m_myID);
	int len = static_cast<int>(strlen(buffer));
	int sent = socket->Send(buffer, len);
	assert(sent == len);
	//RTC_UNUSED2(sent, len);
	ReleaseSocketBase(socket);
}

void WebrtcClient::Close()
{
	m_myID = 0;
	m_oppositePeerID = 0;
	m_peers.clear();
	if (m_inner)
		m_inner->Close();
}

bool WebrtcClient::GetHeaderValue(const std::string& data, size_t eoh, const char* header_pattern, size_t* value)
{
	assert(value != NULL);
	size_t found = data.find(header_pattern);
	if (found != std::string::npos && found < eoh)
	{
		*value = atoi(&data[found + strlen(header_pattern)]);
		return true;
	}
	return false;
}

bool WebrtcClient::GetHeaderValue(const std::string& data, size_t eoh, const char* header_pattern, std::string* value)
{
	assert(value != NULL);
	size_t found = data.find(header_pattern);
	if (found != std::string::npos && found < eoh)
	{
		size_t begin = found + strlen(header_pattern);
		size_t end = data.find("\r\n", begin);
		if (end == std::string::npos)
			end = eoh;
		value->assign(data.substr(begin, end - begin));
		return true;
	}
	return false;
}


bool WebrtcClient::ReadIntoBuffer(void* socket1, std::string* data, size_t* content_length)
{
	//rtc::AsyncSocket* socket = static_cast<rtc::AsyncSocket *>(socket1);
	SocketBase *socket = CreateSocketBase(socket1);
	if (!socket)
	{
		return false;
	}
	char buffer[0xffff];
	do
	{
		int bytes = socket->Recv(buffer, sizeof(buffer));
		if (bytes <= 0)
			break;
		data->append(buffer, bytes);
	} while (true);

	bool ret = false;
	size_t i = data->find("\r\n\r\n");
	if (i != std::string::npos)
	{
		size_t sizevalue = data->size();
		//LOG(INFO) << "Headers received";
		if (GetHeaderValue(*data, i, "\r\nContent-Length: ", content_length))
		{
			size_t total_response_size = (i + 4) + *content_length;
			if (data->length() >= total_response_size)
			{
				ret = true;
				std::string should_close;
				const char kConnection[] = "\r\nConnection: ";
				if (GetHeaderValue(*data, i, kConnection, &should_close) && should_close.compare("close") == 0)
				{
					socket->Close();
					// Since we closed the socket, there was no notification delivered
					// to us.  Compensate by letting ourselves know.
					if (!m_inner)
					{
						ReleaseSocketBase(socket);
						return false;
					}
					//m_inner->OnClose(socket, 0);
				}
			}
			else
			{
				// We haven't received everything.  Just continue to accept data.
			}
		}
		else
		{
			//LOG(LS_ERROR) << "No content length field specified by the server.";
		}
	}

	ReleaseSocketBase(socket);
	return ret;
}
////@@callback
void WebrtcClient::OnRead(void* socket1)
{
	//rtc::AsyncSocket *socket = static_cast<rtc::AsyncSocket*>(socket1);
	SocketBase *socket = CreateSocketBase(socket1);
	if (!socket)
		return;
	if (!m_inner)
	{
		ReleaseSocketBase(socket);
		return;
	}
	State * connectionState = m_inner->GetConnectState();
	if (!connectionState)
	{
		ReleaseSocketBase(socket);
		return;
	}
	size_t content_length = 0;
	std::string tempStr = "";
	if (ReadIntoBuffer(socket,&tempStr, &content_length))
	{
		/*m_controlData->SetString(tempStr);*/
		STLStringShaow * str = m_inner->GetControlData();
		if (!str)
		{
			ReleaseSocketBase(socket);
			return;
		}
		str->SetString(tempStr);
		size_t peer_id = 0;
		size_t eoh = 0;
		bool ok = ParseServerResponse(str->GetString(), content_length, &peer_id, &eoh);
		if (ok)
		{
			if (!IsConnectSuccese())//explain the connection is invalid
			{
				// First response.  Let's store our server assigned ID.
				//assert(m_state == SIGNING_IN);
				assert(*connectionState== SIGNING_IN);
				m_myID = peer_id;
				assert(IsConnectSuccese());
				// The body of the response will be a list of already connected peers.
				if (content_length)
				{
					size_t pos = eoh + 4;
					while (pos < str->GetString().size())
					{
						size_t eol = str->GetString().find('\n', pos);
						if (eol == std::string::npos)
							break;
						size_t id = 0;
						std::string name = "";
						bool connected = false;
						if (ParseEntry(str->GetString().substr(pos, eol - pos), &name, &id,
							&connected) && id != m_myID)
						{
							//just save the client that alreadly login succese but not connect with them
							m_peers[id] = name;
							//LOG(INFO) << ConvertToString<int>(id) << name.c_str();
							//m_callback->OnPeerConnectedServer(static_cast<void *>(&m_peers));
						}
						pos = eol + 1;
					}
				}
				assert(IsConnectSuccese());
				//indicate the login in server is succeselly
				if (!m_parent)
				{
					ReleaseSocketBase(socket);
					return;
				}
				m_parent->OnSignedIn(&m_peers);
			}
			else if (*connectionState == SIGNING_OUT)/*m_state == SIGNING_OUT*/
			{
				Close();
				if (!m_parent)
				{
					ReleaseSocketBase(socket);
					return;
				}
				m_parent->OnDisconnected();
			}
			else if (*connectionState == SIGNING_OUT_WAITING)//m_state == SIGNING_OUT_WAITING
			{
				m_inner->SignOut();
			}
		}

		//m_controlData->clear();
		str->clear();

		if (*connectionState == SIGNING_IN)/*m_state == SIGNING_IN*/
		{
			SocketBase * hanging = CreateSocketBase(m_inner->GetHangingGetSocket());
			assert(hanging->IsCloseState());
			*connectionState = CONNECTED;
			if(!hanging)
			{
				ReleaseSocketBase(socket);
				return;
			}
			hanging->Connect(m_inner->GetServerSocketAddress());
			ReleaseSocketBase(hanging);
		}
	}
	ReleaseSocketBase(socket);
}

void WebrtcClient::OnMessageFromPeerOut(void* peer_id, const std::string& message)
{
	int messageL = message.length();
	int byeL = strlen(kByeMessage);
	int compareResult = message.compare(kByeMessage);
	if (!m_parent)
		return;
	if (messageL == byeL && compareResult == 0)
	{
		m_parent->OnPeerDisconnected(static_cast<void *>(&m_peers));
	}
	else
	{
		m_parent->OnMessageFromPeer(static_cast<void *>(peer_id), message);
	}
}

bool WebrtcClient::SendToPeer(const std::string& message)
{
	if (!m_inner)
		return false;
	if (*(m_inner->GetConnectState())!= CONNECTED)//m_state != CONNECTED
		return false;
	if (!m_parent)
		return false;
	assert(IsConnectSuccese());
	//rtc::scoped_ptr<rtc::AsyncSocket> * ptr = static_cast<rtc::scoped_ptr<rtc::AsyncSocket> * >(m_inner->GetControlSocket());
	SocketBase * ptr = CreateSocketBase(m_inner->GetControlSocket());
	if (!ptr)
		return false;
	assert(ptr->IsCloseState());
	if (!IsConnectSuccese() || !IsValidClientKey(&m_oppositePeerID))
	{
		ReleaseSocketBase(ptr);
		return false;
	}

	char headers[1024];
	sprintfn(headers, sizeof(headers),
		"POST /message?peer_id=%i&to=%i HTTP/1.0\r\n"
		"Content-Length: %i\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n",
		m_myID, m_oppositePeerID, message.length());
	m_inner->GetConnectData()->SetString(headers);
	m_inner->GetConnectData()->Add(message);
	bool result = m_inner->ConnectControlSocket();
	ReleaseSocketBase(ptr);
	return result;
}

bool WebrtcClient::SendHangUp(void * peer_id)
{
	int * id = static_cast<int *>(peer_id);
	if (!id && *id  < 0)
		return false;
	m_oppositePeerID = *id;
	//not juge the m_oppositeID's value isn't  0
	return SendToPeer(kByeMessage);
}


////@@callback
void WebrtcClient::OnHangingGetRead(void* socket1)
{
	//rtc::AsyncSocket * socket = static_cast<rtc::AsyncSocket *>(socket1);
	SocketBase* socket = CreateSocketBase(socket1);
	if (!socket)
		return;
	if (!m_inner)
	{
		ReleaseSocketBase(socket);
		return;
	}
	//LOG(INFO) << __FUNCTION__;
	size_t content_length = 0;
	std::string tempStr = "";
	if (ReadIntoBuffer(socket, &tempStr, &content_length))
	{
		STLStringShaow * notify = m_inner->GetNotifycationData();
		if (!notify)
		{
			ReleaseSocketBase(socket);
			return;
		}
		notify->SetString(tempStr);
		size_t peer_id = 0;
		size_t eoh = 0;
		bool ok = ParseServerResponse(notify->GetString(), content_length,
			(&peer_id), &eoh);
		if (ok)
		{
			// Store the position where the body begins.
			size_t pos = eoh + 4;
			if (m_myID == peer_id)
			{
				// A notification about a new member or a member that just  disconnected.
				size_t id = 0;
				std::string name;
				bool connected = false;
				if (ParseEntry(notify->GetString().substr(pos), &name, &id,
					&connected))
				{
					//LOG(INFO) << "ID:" << ConvertToString<int>(id) << "NAME:" << name.c_str();
					if (connected)
					{//////current peerclient disconnect ,but server inform it to login again
						m_peers[id] = name;
						if (!m_parent)
						{
							ReleaseSocketBase(socket);
							return;
						}
						m_parent->OnPeerConnectedServer(static_cast<void*>(&m_peers));
					}
					else
					{
						//delete the connect that connect belong last time loginIn
						//but opposite client is loginOut from server
						m_peers.erase(id);
						m_parent->OnPeerDisConnectedServer(static_cast<void*>(&m_peers));
					}
				}
			}
			else//indicate the message from another client that arealdy login in server
			{
				m_parent->OnMessageFromPeer(reinterpret_cast<void*>(&peer_id), notify->GetString().substr(pos));
			}
		}

		notify->clear();
	}

	//rtc::scoped_ptr<rtc::AsyncSocket> * hangingGet = static_cast<rtc::scoped_ptr<rtc::AsyncSocket>*>(m_inner->GetHangingGetSocket());
	SocketBase *hangingGet = CreateSocketBase(m_inner->GetHangingGetSocket());
	if (!hangingGet)
	{
		//ReleaseSocketBase(socket);
		return;
	}
	//if ((*hangingGet)->GetState() == rtc::Socket::CS_CLOSED &&*(m_inner->GetConnectState() )== CONNECTED)
	if(hangingGet->IsCloseState() && *(m_inner->GetConnectState()) == CONNECTED)
	{
		hangingGet->Connect(m_inner->GetServerSocketAddress());
	}
	ReleaseSocketBase(hangingGet);
	ReleaseSocketBase(socket);
}

bool WebrtcClient::ParseEntry(const std::string& entry, std::string* name, size_t* id, bool* connected)
{
	assert(name != NULL);
	assert(id != NULL);
	assert(connected != NULL);
	assert(!entry.empty());

	*connected = false;
	size_t separator = entry.find(',');
	if (separator != std::string::npos)
	{
		*id = atoi(&entry[separator + 1]);
		name->assign(entry.substr(0, separator));
		separator = entry.find(',', separator + 1);
		if (separator != std::string::npos)
		{
			*connected = atoi(&entry[separator + 1]) ? true : false;
		}
	}
	return !name->empty();
}

int WebrtcClient::GetResponseStatus(const std::string& response)
{
	int status = -1;
	size_t pos = response.find(' ');
	if (pos != std::string::npos)
		status = atoi(&response[pos + 1]);
	return status;
}

bool WebrtcClient::ParseServerResponse(const std::string& response, size_t content_length, void* peer_id, size_t* eoh)
{
	int status = GetResponseStatus(response.c_str());
	if (status != 200)
	{
		//LOG(LS_ERROR) << "Received error from server";
		Close();
		if (m_parent)
			m_parent->OnDisconnected();
		else
		{
			//LOG(INFO)<<"m_parent is NULL!";
			return false;
		}
	}

	*eoh = response.find("\r\n\r\n");
	assert(*eoh != std::string::npos);
	if (*eoh == std::string::npos)
		return false;

	size_t * ptr = (size_t *)(peer_id);
	if (ptr)
		*ptr = -1;

	// See comment in peer_channel.cc for why we use the Pragma header and not e.g. "X-Peer-Id".
	GetHeaderValue(response, *eoh, "\r\nPragma: ", (size_t*)(peer_id));
	return true;
}

void* WebrtcClient::GetCurrentConnectPeerInfo()
{
	return &m_oppositePeerID;
}

bool WebrtcClient::SetCurrentConnectPeerInfo(void *info)
{
	int * oppositePeer = static_cast<int *>(info);
	if (oppositePeer)
	{
		if (*oppositePeer > 0)
		{
			m_oppositePeerID = *oppositePeer;
			return true;
		}
	}
	m_oppositePeerID = -1;
	return false;
}

void* WebrtcClient::GetPeers()//std::map<int, std::string>
{
	return &m_peers;
}

void WebrtcClient::SetParent(const WebrtcClientOriginal * parent)
{
	m_parent = const_cast<WebrtcClientOriginal *>(parent);
}

const WebrtcClientOriginal * WebrtcClient::GetParent()
{
	return m_parent;
}

WebrtcClientOriginalInner * WebrtcClient::GetInner()
{
	return m_inner;
}

void  WebrtcClient::SetInner(WebrtcClientOriginalInner * value)
{
	m_inner = value;
}
/////////////
WEBRTCP2PDLLSHARED_EXPORT  WebrtcClientOriginalOutter * CreateWebrtcClient()
{
	return new WebrtcClient();
}

WEBRTCP2PDLLSHARED_EXPORT  void ReleaseWebrtcClient(WebrtcClientOriginalOutter * value)
{
	DeletePtr(value);
}