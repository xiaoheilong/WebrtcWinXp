#include"P2PDllInterface.h"
#include"JsonFileIO.h"

#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"
#include "webrtc/base/logging.h"
#include<iostream>
#include<stdlib.h>
//#include <locale.h>
///////
const char* GetOwerSDP(ClientOutterInterfaceOriginal*);
void GetOwerCandidate(ClientOutterInterfaceOriginal*, char *[], int);
void ComebinateSdpIcecandidate(std::string &sdp, std::vector<std::string>* candidate, std::string & content);

bool AcceptFile(P2PEnvironmentBase*, const void*);
bool RefuseFile(P2PEnvironmentBase*, const void*);

bool CreateSendFile(P2PEnvironmentBase*, char *filepath, char * guidStr);//guidStr's size must upper 128
bool DeleteSendFile(P2PEnvironmentBase*, char *);
/////////
P2PEnvironmentBase::~P2PEnvironmentBase()
{
	LOG(INFO)<<__FUNCTION__<< "start!";
}
//////////
class P2PEnvironmentExe:public P2PEnvironmentBase
{
public:
	explicit P2PEnvironmentExe(const std::string &a, const std::string &p, const std::string & u,
		const std::string &pwd, const std::string &r);
	virtual ~P2PEnvironmentExe();
public:
	virtual bool Init(const std::string &a, const std::string &p, const std::string & u,
		const std::string &pwd, const std::string &r) ;
	virtual void Release();
	virtual ClientOutterInterfaceOriginal * GetInner();
	virtual bool  ServerIsValid();
	virtual bool  PeerIsValid();
private:
	//WebrtcInitEnviBase * m_EnviBase;
	ClientOutterInterfaceOriginal * m_OutterInterfaceKit;
	WebrtcClientOriginalOutter * m_OriginalOutter;
	WebrtcConductorDecorator * m_ConductorDecorator;
	TurnserverInfo * m_TurnserverInfo;
};

P2PEnvironmentExe::P2PEnvironmentExe(const std::string &a, const std::string &p, const std::string & u,
	const std::string &pwd, const std::string &r)
{
	//m_EnviBase = NULL;
	m_OutterInterfaceKit = NULL;
	m_OriginalOutter = NULL;
	m_ConductorDecorator = NULL;
	m_TurnserverInfo = NULL;
	Init(a,p,u,pwd, r);
}
P2PEnvironmentExe::~P2PEnvironmentExe()
{
	LOG(INFO) << __FUNCTION__ << "start!";
	Release();
}

bool P2PEnvironmentExe::Init(const std::string &a, const std::string &p, const std::string & u,
	const std::string &pwd, const std::string &r)
{
	//m_EnviBase = CreateWebrtcInitEnviBase();
	m_OutterInterfaceKit = CreateClientOutterInterfaceKit();
	m_OriginalOutter = CreateWebrtcClient();
	ClientOutterInterfaceOriginal * wnd1 = (ClientOutterInterfaceOriginal *)(m_OutterInterfaceKit);
	m_TurnserverInfo = CreateTurnserverInfo(a, p, u, pwd, r);
	AddTurnServerInfo(wnd1, *m_TurnserverInfo);
	m_ConductorDecorator = CreateWebrtcConductorDecorator(m_OriginalOutter, wnd1);
	m_ConductorDecorator->AddTurnServerInfo(*m_TurnserverInfo);
	m_ConductorDecorator->InitializePeerConnection();
	return true;
}

void  P2PEnvironmentExe::Release()
{
	//ReleaseWebrtcInitEnviBase(m_EnviBase);
	ReleaseWebrtcConductorDecorator(m_ConductorDecorator);
	ReleaseClientOutterInterfaceKit(m_OutterInterfaceKit);
	ReleaseWebrtcClient(m_OriginalOutter);
	ReleaseTurnserverInfo(m_TurnserverInfo);
}

ClientOutterInterfaceOriginal * P2PEnvironmentExe::GetInner()
{
	return m_OutterInterfaceKit;
}


bool  P2PEnvironmentExe::ServerIsValid()
{
	ClientOutterInterfaceOriginal * wnd1 = (ClientOutterInterfaceOriginal *)(m_OutterInterfaceKit);
	return ConnectServerIsValid(wnd1);
}


bool  P2PEnvironmentExe::PeerIsValid()
{
	ClientOutterInterfaceOriginal * wnd1 = (ClientOutterInterfaceOriginal *)(m_OutterInterfaceKit);
	return ConnectPeerIsValid(wnd1);
}
////////////////////////

WEBRTCP2PDLLSHARED_EXPORT P2PEnvironmentBase* CreateP2PEnvironment(const char *a, const int &p, const char *u,
	const char *pwd, const char * r)
{
	if (!a || p < 0 || p > 65535 || !u || !pwd || !r)
		return NULL;
	if (!IPStrIsValid(a))
		return NULL;
	std::string username = u;
	if (username.empty())
		return NULL;
	std::string password = pwd;
	if (password.empty())
		return NULL;
	//////
	std::string port = ConvertToString<int>(p);
	return  new P2PEnvironmentExe(a, port, u, pwd, r);
}

WEBRTCP2PDLLSHARED_EXPORT void CloseP2PEnvironment(P2PEnvironmentBase *value)
{
	DeletePtr(value);
}

WEBRTCP2PDLLSHARED_EXPORT bool RegisterOutter(P2PEnvironmentBase *envi, OutterInterfaceBase* outter)
{
	if (envi && outter)
	{
		ClientOutterInterfaceOriginal * inner = envi->GetInner();
		if(inner)
		inner->AttachedWnd(outter);
		return true;
	}
	return false;
}


WEBRTCP2PDLLSHARED_EXPORT bool ServerIsValid(P2PEnvironmentBase *value)
{
	if (value)
		value->ServerIsValid();
	return false;
}
WEBRTCP2PDLLSHARED_EXPORT bool PeerIsValid(P2PEnvironmentBase *value)
{
	if (value)
		value->PeerIsValid();
	return false;
}

WEBRTCP2PDLLSHARED_EXPORT bool SendText( P2PEnvironmentBase* callback, wchar_t*goaldata,int datasize)
{
	FileDataPack message;
	message.message_id = ResType::Text_Type;
	if (!message.data)
	{
		DeletePtr(goaldata);
		return false;
	}
	////////////////
	std::string messageT = UcToMs(goaldata);
	::memcpy(message.data->message, messageT.c_str(), messageT.size());
	message.data->messageSize = messageT.size();
	char buffer[3000];
	int serializeSize = message.Serialize(reinterpret_cast<uint8_t *>(buffer), 3000);
	if (callback)
	{
		ClientOutterInterfaceOriginal * inner = callback->GetInner();
		if (inner)
			return inner->SendTextMessage(buffer, serializeSize);
	}
	DeletePtr(goaldata);
	return false;
}


WEBRTCP2PDLLSHARED_EXPORT void GetLocaltionSdpIcecandidate(P2PEnvironmentBase*value,
	char *content)
{
	if (!value || !content)
		return;
	ClientOutterInterfaceOriginal * inner = value->GetInner();
	if (!inner)
		return;
	std::string sdp = GetOwerSDP(inner);
	std::string positionInfo = "";
	std::vector<std::string>* iceCandidateArray = const_cast<std::vector<std::string>*>(inner->GetOwerIceCandidate());
	ComebinateSdpIcecandidate(sdp, iceCandidateArray, positionInfo);
	memset(content, 0, strlen(content));
	memcpy(content, positionInfo.c_str(), positionInfo.size());
	content[positionInfo.size()] = '\0';
}

WEBRTCP2PDLLSHARED_EXPORT void SetOppositeSdpIcecandidate(P2PEnvironmentBase* inner, char * sdpCandidate1)
{
	if (!inner || !sdpCandidate1)
		return;
	ClientOutterInterfaceOriginal * core = inner->GetInner();
	if (!core)
		return;
	std::string sdpCandidate = sdpCandidate1;
	JsonFileIORead read(sdpCandidate, false);
	const Json::Value & json = read.GetJson();
	read.Read();
	std::string sdp = "";
	read.GetNode("sdp", sdp);
	std::vector<std::string >candidates;
	read.GetNode("icecandidate", candidates);
	core->SetOppositeSDPInfo(sdp);
	core->SetOppositeIceCandidate(candidates);
}

WEBRTCP2PDLLSHARED_EXPORT bool CreateTurnServerConnection( P2PEnvironmentBase* value)
{
	if (!value)
		return false;
	ClientOutterInterfaceOriginal * core = value->GetInner();
	if (!core)
		return false;
	PeerInfo info;
	return core->ConnectToPeer(info);
}

WEBRTCP2PDLLSHARED_EXPORT bool CloseTurnServerConnection(P2PEnvironmentBase* value)
{
	LOG(INFO) << "Close the turnserver connection start!";
	if (!value)
		return false;
	LOG(INFO) << "Close the turnserver connection start 2!";
	ClientOutterInterfaceOriginal * core = value->GetInner();
	bool result = core->DisConnectToPeer();
	if (result)
		LOG(INFO) << "Close the turnserver connection end success!";
	else
		LOG(INFO) << "Close the turnserver connection end failure!";
	return result;
}


WEBRTCP2PDLLSHARED_EXPORT bool SendFile(P2PEnvironmentBase* value, char *guid, char * filepath)
{
	if (value)
	{
		/////now create the file task records on the filethreadmanager,
		CreateSendFile(value, filepath, guid);
		////////
		ClientOutterInterfaceOriginal * core = value->GetInner();
		if (!core)
			return false;
		std::string str = guid;
		core->SendFile(&str);
		return true;
	}
	return false;
}


WEBRTCP2PDLLSHARED_EXPORT void FileTaskProgress(void* value1, double& totalSize, double & finishSize)
{
	TransferProgressPacket *value = reinterpret_cast<TransferProgressPacket*>(value1);
	if (value)
	{
		//process = value->progressValue * 100;
		totalSize = value->totalTaskSize;
		finishSize = value->finishTaskSize;
		DeletePtr(value);
	}
}

WEBRTCP2PDLLSHARED_EXPORT bool InterruptFileTask(P2PEnvironmentBase *p2pEnvi, char* taskGuid)
{
	if (p2pEnvi && taskGuid)
	{
		ClientOutterInterfaceOriginal * core = p2pEnvi->GetInner();
		if (!core)
			return false;
		return core->InterruptFileTask(taskGuid);
	}

	return false;
}

WEBRTCP2PDLLSHARED_EXPORT bool RestartFileTask(P2PEnvironmentBase * p2pEnvi, char* taskGuid)
{
	if (p2pEnvi && taskGuid)
	{
		ClientOutterInterfaceOriginal * core = p2pEnvi->GetInner();
		if (!core)
			return false;
		return core->RestartFileTask(taskGuid);
	}
	return false;
}

WEBRTCP2PDLLSHARED_EXPORT bool CancleFileTask(P2PEnvironmentBase *p2pEnvi, char* taskGuid)
{
	if (p2pEnvi && taskGuid)
	{
		ClientOutterInterfaceOriginal * core = p2pEnvi->GetInner();
		if (!core)
			return false;
		return core->CancleFileTask(taskGuid);
	}
	return false;
}

WEBRTCP2PDLLSHARED_EXPORT bool CreateRecieveFile(P2PEnvironmentBase * p2pEnvi, char *guid,char * resPath)
{
	if (p2pEnvi && guid && resPath)
	{
		ClientOutterInterfaceOriginal * core = p2pEnvi->GetInner();
		if (!core)
			return false;
		//create the  recive file task which flag is equal to guid
		core->CreateRecieveFile(guid, resPath);
		return true;
	}

	return false;
}

WEBRTCP2PDLLSHARED_EXPORT bool DeleteRecieveFile(P2PEnvironmentBase *p2pEnvi, char * guid)
{
	if (p2pEnvi && guid)
	{
		ClientOutterInterfaceOriginal * core = p2pEnvi->GetInner();
		if (!core)
			return false;
		std::string str = guid;
		///delete  the recieve file task which flag is equal to guid
		//core->InterruptFileTask(&str);
		return true;
	}

	return false;
}

WEBRTCP2PDLLSHARED_EXPORT bool  StartAudio(P2PEnvironmentBase* value)
{
	if (value)
	{

		return true;
	}
	return false;
}

WEBRTCP2PDLLSHARED_EXPORT void GetAudioData(P2PEnvironmentBase*value, char *buffer)
{
	if (value)
	{
		;
	}
}

WEBRTCP2PDLLSHARED_EXPORT bool  PauseAudio(P2PEnvironmentBase*value)
{
	if (value)
	{

		return true;
	}
	return false;
}

WEBRTCP2PDLLSHARED_EXPORT bool  EndAudio(P2PEnvironmentBase*value)
{
	if (value)
	{
		return true;
	}
	return false;
}


WEBRTCP2PDLLSHARED_EXPORT bool  StartVideo(P2PEnvironmentBase*value)
{
	if (value)
	{
		return true;
	}
	return false;
}

WEBRTCP2PDLLSHARED_EXPORT void GetVideoData(P2PEnvironmentBase*value, char*buffer)
{
	if (value)
	{
		;
	}
}

WEBRTCP2PDLLSHARED_EXPORT bool  PauseVideo(P2PEnvironmentBase*value)
{
	if (value)
	{
		return true;
	}
	return false;
}

WEBRTCP2PDLLSHARED_EXPORT bool  EndVideo(P2PEnvironmentBase*value)
{
	if (value)
	{

		return true;
	}
	return false;
}

////////////////////////////////////
const char* GetOwerSDP(ClientOutterInterfaceOriginal* value)
{
	if (!value)
		return NULL;
	return value->GetOwerSDPInformation().c_str();
}

void GetOwerCandidate(ClientOutterInterfaceOriginal*value, char *buffer[], int bufferSize)
{
	if (!value || buffer)
		return;
	std::vector<std::string>* iceCandidateArray = const_cast<std::vector<std::string>*>(value->GetOwerIceCandidate());
	if (!iceCandidateArray)
		return;
	int total = (int)(iceCandidateArray->size());
	if (bufferSize < total)
		return;
	for (int i = 0;i < total;i++)
	{
		if (!buffer[i])
			continue;
		int copySize = (*iceCandidateArray)[i].size();
		memcpy(buffer[i], (*iceCandidateArray)[i].c_str(), copySize);
		buffer[i][copySize] = '\0';
	}
}


void ComebinateSdpIcecandidate(std::string &sdp, std::vector<std::string>* candidate, \
	std::string & content)
{
	content.clear();
	JsonFileIODisposable write;
	write.AddNode("sdp", sdp);
	write.AddNode("icecandidate", *candidate);
	const Json::Value & json = write.GetJson();
	JsonStyledWriter sw;
	STLStringShaow * str = sw.Write(json);
	if (str)
		content = str->GetString();
}


bool AcceptFile(P2PEnvironmentBase*value, const void* filedata)
{
	if (value && filedata)
	{
		ClientOutterInterfaceOriginal * core = value->GetInner();
		if (!core)
			return false;
		core->AcceptFile(const_cast<void*>(filedata));
		return true;
	}
	return false;

}

bool RefuseFile(P2PEnvironmentBase* value, const void*filedata)
{
	if (value&& filedata)
	{
		ClientOutterInterfaceOriginal * core = value->GetInner();
		if (!core)
			return false;
		core->RefuseFile(const_cast<void*>(filedata));
		return true;
	}
	return false;
}

int  RecieveText(char * pack, uint8_t*  message, int size)
{
	if (!pack || !message || 0 == size)
		return 0 ;
	FileDataPack buffer;
	buffer.Deserialize(message, size);
	memcpy(pack, buffer.data->message, buffer.data->messageSize);
	pack[buffer.data->messageSize] = '\0';
	return buffer.data->messageSize;
}

///according the guid and the res filepath ,then create a records on  the filethreadmanager，then wait to send file by apply the resguid
bool CreateSendFile(P2PEnvironmentBase* value, char *filepath1, char * guidStr)
{
	if (value && filepath1&& guidStr && strlen(guidStr) < 128)
	{
		std::string filePath = filepath1;
		std::wstring  filepathWstr = MsToUc(filePath);
		ClientOutterInterfaceOriginal * core = value->GetInner();
		if (!core)
			return false;
		DataRepositoryBase * res = CreateP2PFileDataRepositorySender(filePath);
		res->SetGUID(guidStr);
		core->CreateBarcodeFile(res);
		if (!res->GetGUID().empty())
		{
			/*int copySize = res->GetGUID().size();
			memcpy(guidStr, res->GetGUID().c_str(), copySize);
			guidStr[copySize] = '\0';
			return true;*/
			return true;
		}
	}
	return false;
}

bool DeleteSendFile(P2PEnvironmentBase* value, char * guidStr)
{
	if (value && guidStr)
	{
		ClientOutterInterfaceOriginal * core = value->GetInner();
		if (!core)
			return false;
		return core->DeleteBarcodeFile(guidStr);
	}
	return false;
}

////////////////