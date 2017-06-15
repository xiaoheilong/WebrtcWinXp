﻿#include "WebrtcConductor.h"
#include <utility>
///////
#include "webrtc/mediastreaminterface.h"
#include "webrtc/peerconnectioninterface.h"
#include "webrtc/base/scoped_ptr.h"

#include "webrtc/test/fakeconstraints.h"
#include "webrtc/base/common.h"
#include "webrtc/base/json.h"
#include "webrtc/base/logging.h"
#include "media/webrtc/webrtcvideocapturerfactory.h"
#include "webrtc/modules/video_capture/include/video_capture_factory.h"
#include "webrtc/datachannelinterface.h"
#include "webrtc/peerconnectioninterface.h"
#include "webrtc/proxy.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"
#include "webrtc/base/win32socketserver.h"
#include <iostream>
////

namespace WebrtcConductorTool
{
	const char kCandidateSdpMidName_Super[] = "sdpMid";
	const char kCandidateSdpMlineIndexName_Super[] = "sdpMLineIndex";
	const char kCandidateSdpName_Super[] = "candidate";

	// Names used for a SessionDescription JSON object.
	const char kSessionDescriptionTypeName_Super[] = "type";
	const char kSessionDescriptionSdpName_Super[] = "sdp";

	const bool DTLS_ON = true;
	const bool DTLS_OFF = false;

}
////////
WebrtcConductorDecorator::~WebrtcConductorDecorator()
{

}
//////////
using namespace WebrtcConductorTool;
static const  std::string emptyGoalbalString = "";
const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";
const char kStreamLabel[] = "stream_label";
const char kDataChannel[] = "datachannel_label";
//////////
namespace TransferNamespace
{
	class ServerInfoVector :public ContainerInfoShaowBase
	{
	public:
		ServerInfoVector();
		~ServerInfoVector();
	public:
		virtual void * GetInfoVector();
	protected:
		std::vector<ServerInfo> m_vector;
	};

	ServerInfoVector::ServerInfoVector()
	{
		m_vector.clear();
	}

	ServerInfoVector::~ServerInfoVector()
	{
		m_vector.clear();
	}

	void * ServerInfoVector::GetInfoVector()
	{
		return &m_vector;
	}

	inline ContainerInfoShaowBase * CreateServerInfoVector()
	{
		return new ServerInfoVector();
	}

	inline void DeleteServerInfoVector(ContainerInfoShaowBase * vector)
	{
		DeletePtr(vector);
	}
	//
	////////////
	class TurnserverInfoVector :public ContainerInfoShaowBase
	{
	public:
		explicit TurnserverInfoVector();
		virtual ~TurnserverInfoVector();
	public:
		virtual void* GetInfoVector();
		virtual bool Match(const webrtc::IceCandidateCollection * icecandidates);
	protected:
		std::vector<TurnserverInfo> m_vector;
	};

	TurnserverInfoVector::TurnserverInfoVector()
	{
		m_vector.clear();
	}

	TurnserverInfoVector::~TurnserverInfoVector()
	{
		m_vector.clear();
	}

	void* TurnserverInfoVector::GetInfoVector()
	{
		return &m_vector;
	}
	
	bool TurnserverInfoVector::Match(const webrtc::IceCandidateCollection * icecandidates)
	{
		bool  stunExit = false;
		bool  turnExit = false;
		if (icecandidates && icecandidates->count() > 0)
		{
			std::string stunType = "srflx";
			std::string turnType = "relay";
			for (size_t i = 0; i < icecandidates->count();i++)
			{
				const webrtc::IceCandidateInterface * node = icecandidates->at(i);
				if (node)
				{
					std::string nodeStr;
					if (node->ToString(&nodeStr))
					{
						if (m_vector.size() > 0)
						{
							if (nodeStr.find(stunType) != std::string::npos )
								stunExit =  true;
							if (nodeStr.find(turnType) != std::string::npos)
								turnExit = true;
						}
					}
				}
			}
		}
		return stunExit && turnExit;
	}

	inline ContainerInfoShaowBase * CreateTurnserverInfoVector()
	{
		return new TurnserverInfoVector();
	}

	inline void DeleteTurnserverInfoVector(ContainerInfoShaowBase * vector)
	{
		DeletePtr(vector);
	}
	//////////////
	class StringVector :public ContainerInfoShaowBase
	{
	public:
		explicit StringVector() { m_vector.clear(); }
		virtual ~StringVector() { m_vector.clear(); }
	public:
		virtual void* GetInfoVector() { return &m_vector; }
	protected:
		std::vector<std::string> m_vector;
	};


	inline ContainerInfoShaowBase * CreateStringVector()
	{
		return new StringVector();
	}

	inline void DeleteStringVector(ContainerInfoShaowBase * vector)
	{
		DeletePtr(vector);
	}
	/////////
	class DataBufferQueue :public ContainerInfoShaowBase
	{
	public:
		explicit DataBufferQueue();
		virtual ~DataBufferQueue();
	public:
		virtual void* GetInfoVector() { return &m_vector; }
	protected:
		std::queue<webrtc::DataBuffer> m_vector;
	};

	DataBufferQueue::DataBufferQueue()
	{

	}

	DataBufferQueue::~DataBufferQueue()
	{
		while (!m_vector.empty())
		{
			m_vector.pop();
		}
	}

	inline ContainerInfoShaowBase * CreateDataBufferQueue()
	{
		return new DataBufferQueue();
	}

	inline void DeleteDataBufferQueue(ContainerInfoShaowBase * queue)
	{
		DeletePtr(queue);
	}
	////////////
	class StringDeque :public ContainerInfoShaowBase
	{
	public:
		explicit StringDeque();
		virtual ~StringDeque();
	public:
		virtual void* GetInfoVector() { return &m_vector; }
	protected:
		std::deque<std::string*> m_vector;
	};

	StringDeque::StringDeque()
	{
		m_vector.clear();
	}

	StringDeque::~StringDeque()
	{
		m_vector.clear();
	}
	/////////
	class StreamActiveMap :public ContainerInfoShaowBase
	{
	public:
		explicit StreamActiveMap() { m_vector.clear(); }
		virtual ~StreamActiveMap() { m_vector.clear(); }
	public:
		virtual void* GetInfoVector() { return &m_vector; }
	protected:
		std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> > m_vector;
	};

	////////
}
/////////////////////////////innner class 


class WebrtcConductorDirect :public WebrtcConductor
{
public:
	WebrtcConductorDirect(WebrtcClientOriginal* peerConnectClient, ClientOutterInterfaceOriginal *outterUserClient);
	virtual ~WebrtcConductorDirect();
public:
	virtual void DealOwerSessionSDP(void* desc)override;
	virtual void DealOwerIceCandidate(const void* candidate)override;

	virtual void  SetOppositeSDPInformation(const std::string & sdp)override;
	virtual void  SetOppositeIceCandidate(const std::vector<std::string >&icecandidate)override;
	virtual const std::string&   GetOwerSDPInformation();
	const std::vector<std::string> * GetOwerIceCandidate();
	virtual void ConnectToPeer(void * info)override;
	virtual void DisconnectFromCurrentPeer()override;
	virtual void Close() override;
};


////////////////define the class by this header
///////////////convert the webrtc  class to the self class type
class WebrtcConductorBase;
class SessionConnectionObserverInterface :public webrtc::CreateSessionDescriptionObserver, public webrtc::PeerConnectionObserver
	, public ConductorExchangePosition, public ConductorExchangeVideo
{
public:
	virtual ~SessionConnectionObserverInterface() {}
public:
	virtual bool SessionConnectionIsValid() = 0;
	virtual webrtc::DataChannelInterface::DataState &GetMessageChannelState() = 0;
public:
	////////Get the parent pointer
	virtual WebrtcConductorBase* GetParent() = 0;
	virtual void SetParent(WebrtcConductorBase * parent) = 0;
	///////function which is used for webrtc connection
	virtual bool SendData(void *message, size_t size) = 0;
	/////////
	////////Create 、Init 、Release methons
	virtual bool InitializePeerConnection() = 0;
	virtual bool ReinitializePeerConnectionForLoopback() = 0;
	virtual bool CreatePeerConnection(bool dtls) = 0;
	virtual void DeletePeerConnection() = 0;
	virtual void AddStreams() = 0;
	//Add the turnserver information to the vector
	virtual void AddTurnServerInfo(const TurnserverInfo &info) = 0;
	virtual void AddSinalServerInfo(const ServerInfo &info) = 0;

	virtual void DealOwerSessionSDP(webrtc::SessionDescriptionInterface* desc) = 0;//deal the sdp info which create by self
	virtual void DealOwerIceCandidate(const webrtc::IceCandidateInterface* candidate) = 0;//deal the candidate info which create by self
	virtual void DealOppositeSessionSDP(const void* message) = 0;//deal the sdp info which receive from opposite
	virtual void DealOppositeIceCandidate(const void*  message1) = 0;//deal the candidate info which receive from opposite

	virtual void  SetOppositeSDPInformation(const std::string & sdp) = 0;
	virtual void  SetOppositeIceCandidate(const std::vector<std::string >&icecandidate) = 0;
	virtual const std::string&   GetOwerSDPInformation() = 0;
	virtual const std::vector<std::string> * GetOwerIceCandidate() = 0;

	virtual  std::string & SignalServerConnectionFailure() = 0;  //outter input to call the method
	virtual void CreateOffer() = 0;//start gathering local sdp and icecandidate information
	virtual bool DataStreamIsEmpty() = 0;
protected:
	// CreateSessionDescriptionObserver implementation.
	virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc) = 0;
	virtual void OnFailure(const std::string& error) = 0;
	// PeerConnectionObserver implementation.
	virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) = 0;
	virtual void OnAddStream(webrtc::MediaStreamInterface* stream) = 0;

	virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream) = 0;

	virtual void OnDataChannel(webrtc::DataChannelInterface* channel) = 0;

	virtual void OnRenegotiationNeeded() = 0;

	virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) = 0;

	virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) = 0;

	virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) = 0;

	virtual void OnIceConnectionReceivingChange(bool receiving) = 0;
	virtual void* OpenVideoCaptureDevice() = 0;
};
/////////////////
class SessionConnectionObserver :public SessionConnectionObserverInterface/*, rtc::MessageHandler*/
{
public:
	explicit SessionConnectionObserver(WebrtcConductorBase * parent);
	virtual ~SessionConnectionObserver();
public:
	virtual bool SessionConnectionIsValid();
	virtual webrtc::DataChannelInterface::DataState & GetMessageChannelState();
	////////Get the parent pointer
	virtual WebrtcConductorBase* GetParent();
	virtual void SetParent(WebrtcConductorBase * parent);
	/////
	virtual bool SendData(void *message, size_t size);
	////////Create 、Init 、Release methons
	virtual bool InitializePeerConnection();
	virtual bool ReinitializePeerConnectionForLoopback();
	virtual bool CreatePeerConnection(bool dtls);
	virtual void DeletePeerConnection();
	virtual void AddStreams();
	/////
	virtual void AddTurnServerInfo(const TurnserverInfo &info);
	virtual void AddSinalServerInfo(const ServerInfo &info);

	virtual void DealOwerSessionSDP(webrtc::SessionDescriptionInterface* desc);
	virtual void DealOwerIceCandidate(const webrtc::IceCandidateInterface* candidate);
	virtual void DealOppositeSessionSDP(const void *message1);
	virtual void DealOppositeIceCandidate(const void* message1);

	virtual void  SetOppositeSDPInformation(const std::string & sdp);
	virtual void  SetOppositeIceCandidate(const std::vector<std::string >&icecandidate);
	virtual const std::string&   GetOwerSDPInformation();
	virtual const std::vector<std::string> * GetOwerIceCandidate();

	virtual std::string & SignalServerConnectionFailure();
	virtual void CreateOffer();
	virtual bool DataStreamIsEmpty();
	virtual void* OpenVideoCaptureDevice();
private:
	virtual void StartLocalRenderer(void* local_video);
	virtual void StopLocalRenderer();
	virtual void StartRemoteRenderer(void* remote_video);
	virtual void StopRemoteRenderer();

	virtual bool IceCandidateInfoValid();
protected:
	// CreateSessionDescriptionObserver implementation.
	virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc);
	virtual void OnFailure(const std::string& error);
	// PeerConnectionObserver implementation.
	virtual void OnError();
	virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state);
	virtual void OnAddStream(webrtc::MediaStreamInterface* stream);

	virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream);

	virtual void OnDataChannel(webrtc::DataChannelInterface* channel);

	virtual void OnRenegotiationNeeded();

	virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state);

	virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state);

	virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);

	virtual void OnIceConnectionReceivingChange(bool receiving);
private:
	virtual int AddRef()  /*const*/;
	virtual int Release() /*const*/;
	ConnectState  RecieveGatheringComplete();
	ConnectState  OfferClientGatheringComplete(const webrtc::IceCandidateCollection * icecandidates);
	ConnectState  AnswerClientGatheringComplete(const webrtc::IceCandidateCollection * icecandidates);
public:
	bool m_loopback_; //message loopback ,send by self and recieve by self
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_peer_connection_;
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>m_peer_connection_factory_;
	ContainerInfoShaowBase * m_active_streams_;
	rtc::scoped_refptr<webrtc::DataChannelInterface> m_messageChannel;
protected:
	WebrtcConductorBase * m_parent;
	webrtc::DataChannelInterface::DataState m_channelState;
	STLStringShaow * m_owerSDPInfo;
	STLStringShaow * m_oppositeSDPInfo;
	ContainerInfoShaowBase *m_owericeCandidateInfo;//save the ower candidates array
	ContainerInfoShaowBase *m_oppositeiceCandidateInfo;//save the opposite candidates array
private:
	ContainerInfoShaowBase * m_sendTemporaryQueue;
	ContainerInfoShaowBase * m_turnserverVector;  //save the turnserver and signalserver's address information
	ContainerInfoShaowBase * m_signalServerVector;
	bool m_connectedToPeerFlag;
	bool m_getIceCandidates;
	bool m_candiatesGatheringSuccess;
	ConnectState m_connectedToTurnserverFlag;
};


//////////////SessionConnectionObserverInterface Proxy
//BEGIN_PROXY_MAP(SessionConnectionObserver)
//
//
//END_PROXY
//////////////

WEBRTCP2PDLLSHARED_EXPORT SessionConnectionObserverInterface* CreateSessionConnectionObserverBase(WebrtcConductorBase *parent)
{
	return new SessionConnectionObserver(parent);
}

WEBRTCP2PDLLSHARED_EXPORT void DeleteSessionConnectionObserverBase(SessionConnectionObserverInterface* value)
{
	DeletePtr(value);
}
////////////////////
#define CreateSTLString TransferNamespace::CreateSTLString 
#define CreateSTLStringEx TransferNamespace::CreateSTLStringEx
#define DeleteSTLString TransferNamespace::DeleteSTLString
///////////////////
class  DummySetSessionDescriptionObserverSuper : public webrtc::SetSessionDescriptionObserver
{
public:
	static DummySetSessionDescriptionObserverSuper* Create();
	virtual void OnSuccess();
	virtual void OnFailure(const std::string& error);
protected:
	DummySetSessionDescriptionObserverSuper();
	~DummySetSessionDescriptionObserverSuper();
};

DummySetSessionDescriptionObserverSuper::DummySetSessionDescriptionObserverSuper()
{

}

DummySetSessionDescriptionObserverSuper::~DummySetSessionDescriptionObserverSuper()
{

}

DummySetSessionDescriptionObserverSuper* DummySetSessionDescriptionObserverSuper::Create()
{
	return new rtc::RefCountedObject<DummySetSessionDescriptionObserverSuper>();
}

void DummySetSessionDescriptionObserverSuper::OnSuccess()
{
	LOG(INFO) << __FUNCTION__;
}

void DummySetSessionDescriptionObserverSuper::OnFailure(const std::string& error)
{
	LOG(INFO) << __FUNCTION__ << " " << error;
}
/////////
//ConductorBusiness::~ConductorBusiness()
//{
//
//}
////////////////
WebrtcConductorBase::~WebrtcConductorBase()
{

}
///////////////////
WebrtcConductor::WebrtcConductor(WebrtcClientOriginal* peerConnectClient, ClientOutterInterfaceOriginal *outterUserClient)
	:m_clientConnector(peerConnectClient),
	m_outterClient(outterUserClient)
{
	//init the file transfer threadmanager and datarepository
	/*m_owerSDPInfo = CreateSTLString();
	m_oppositeSDPInfo = CreateSTLString();
	m_owericeCandidateInfo = CreateContainer(TransferNamespace::StringVector);
	m_oppositeiceCandidateInfo = CreateContainer(TransferNamespace::StringVector);*/
	//m_sendTemporaryQueue = CreateContainer(TransferNamespace::DataBufferQueue);
	m_pending_messages_ = CreateContainer(TransferNamespace::StringDeque);
	/*m_active_streams_ = CreateContainer(TransferNamespace::StreamActiveMap);*/
	m_sessionConnectionObserver = CreateSessionConnectionObserverBase(this);
	m_messageClientObserver = CreateTransferDataBase();
	m_messageClientObserver->BindParent(m_outterClient);
	/////////
	//InitializePeerConnection();
	//////////
}

WebrtcConductor:: ~WebrtcConductor()
{
	assert(!Connection_Active());
	ReleaseWebrtcClientOriginal(m_clientConnector);
	DeleteContainer(m_pending_messages_);
	ReleaseTransferDataBase(m_messageClientObserver);
	DeleteSessionConnectionObserverBase(m_sessionConnectionObserver);
}

bool WebrtcConductor::Connection_Active() const
{
	if (m_sessionConnectionObserver)
		return m_sessionConnectionObserver->SessionConnectionIsValid();
	return false;
}

bool WebrtcConductor::IsLoginSuccese()const
{
	if (m_clientConnector)
	{
		WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
		if (clientOutter)
			return clientOutter->IsConnectSuccese();
	}
	return false;
}

void WebrtcConductor::Close()
{

}

bool WebrtcConductor::SendData(void *message, size_t size) //send normal data(include text、binarary etc)
{
	bool sendResults = false;
	if (m_sessionConnectionObserver)
	{
		sendResults = m_sessionConnectionObserver->SendData(message, size);
	}
	return sendResults;
}

void  WebrtcConductor::AcceptFile(void * param)
{
	FileDataPack * msg = reinterpret_cast<FileDataPack *>(param);
	if (msg)
	{
		if (msg->data)
		{
			//check the file data from sender which is or not  match current reciever 
			std::string senderResGuid = msg->owerInfo.sUserResGuid;
			P2PThreadBase * threads = m_fileThreadManager->GetThreadByGuid(senderResGuid);
			if (threads)
			{

				if (threads->GetTaskType() == RECIEVE_TASK)
				{
					std::string goalResPath = "";
					if (threads->GetSendResDes().IsResPathValid() && threads->GetSendResDes().resPath)
						goalResPath = threads->GetSendResDes().resPath->GetString();
					else
						goalResPath = msg->data->message;
					/////blow is  show this file task is match current p2p interface
					std::string resGuid = RecieveFile(senderResGuid, goalResPath);
					if (m_fileDataRepository)
					{
						m_fileDataRepository->SetResBlockSize(resGuid, msg->data->packetIndex);//set current repository total blocksize
						FileDataPack pack;
						pack.data->messageType = Can_Continue_Send;
						pack.owerInfo.SetRuserResGuid(resGuid.c_str());
						pack.owerInfo.SetSuserResGuid(msg->owerInfo.sUserResGuid);
						pack.data->SetContent(const_cast<char*>(goalResPath.c_str()));
						pack.message_id = File_Type;
						uint8_t buffer[3000];
						int serializeSize = pack.Serialize(buffer, 3000);
						SendData(buffer, serializeSize);
						return;
					}
				}
			}
		}
	}
	//or refuse this send task
	RefuseFile(param);
}

void  WebrtcConductor::RefuseFile(void * param)
{
	FileDataPack * msg = reinterpret_cast<FileDataPack *>(param);
	if (msg)
	{
		std::string senderResGuid = msg->owerInfo.sUserResGuid;
		m_fileThreadManager->UnAttach(senderResGuid);
		FileDataPack pack;
		if (!pack.data)
			return;
		pack.data->messageType = Not_Can_Continue_Send;
		pack.owerInfo.SetSuserResGuid(msg->owerInfo.sUserResGuid);
		pack.data->SetContent(msg->data->message);
		pack.message_id = File_Type;
		uint8_t buffer[3000];
		int serializeSize = pack.Serialize(buffer, 3000);
		SendData(buffer, serializeSize);
	}
}


bool WebrtcConductor::InterruptFileTask(void * param)
{
	//interpret the file transfer task,
	if (param)
	{
		char * guid = NULL;
		guid = static_cast<char*>(param);
		if (!guid)
			return false;
		std::string guidStd = guid;
		if (!m_fileThreadManager->SuspendChildThread(guidStd))
			return false;
		//send message to inform opposite client to interrupt It's ower task thread
		FileDataPack pack;
		pack.data->messageType = Interrupt_File_Transfer;
		pack.owerInfo.SetSuserResGuid(guid);
		pack.message_id = File_Type;
		uint8_t buffer[3000];
		int serializeSize = pack.Serialize(buffer, 3000);
		SendData(buffer, serializeSize);
		return true;

	}
	return false;
}

bool  WebrtcConductor::RestartFileTask(void * param)
{
	if (param)
	{
		char * guid = NULL;
		guid = static_cast<char*>(param);
		if (!guid)
			return false;
		std::string guidStd = guid;
		if (!m_fileThreadManager->ResumeChildThread(guidStd))
			return false;
		//inform opposite client to restart special file task
		FileDataPack pack;
		pack.data->messageType = Restart_File_Transfer;
		pack.owerInfo.SetSuserResGuid(guid);
		pack.message_id = File_Type;
		uint8_t buffer[3000];
		int serializeSize = pack.Serialize(buffer, 3000);
		SendData(buffer, serializeSize);
		return true;
	}
	return false;
}

bool  WebrtcConductor::CancleFileTask(void * param)
{
	if (param)
	{
		char * guid = NULL;
		guid = static_cast<char*>(param);
		if (!guid)
			return false;
		std::string guidStd = guid;
		m_fileThreadManager->UnAttach(guidStd);
		//inform opposite client to cancle the special file task
		FileDataPack pack;
		pack.data->messageType = Cancle_File_Transfer;
		pack.owerInfo.SetSuserResGuid(guid);
		pack.message_id = File_Type;
		uint8_t buffer[3000];
		int serializeSize = pack.Serialize(buffer, 3000);
		SendData(buffer, serializeSize);
		return true;
	}
	return false;
}

bool  WebrtcConductor::CreateRecieveFile(void * param, void * param1)
{
	if (m_fileThreadManager)
	{
		char* guid = static_cast<char*>(param);
		char* resGoalPath = static_cast<char*>(param1);
		if (!guid || !resGoalPath)
			return false;
		std::string guidStr = guid;
		std::string goalResPath = resGoalPath;
		if (!m_fileThreadManager->GetThreadByGuid(guidStr))
		{
			P2PThreadBase * thread = CreateP2PThreadBase(this);
			ThreadTaskDesFile senderFileDes(guidStr, goalResPath);
			thread->SetSenderDes(senderFileDes);
			if (!thread)
				return false;
			thread->SetTaskType(RECIEVE_TASK);
			m_fileThreadManager->Attach(guidStr, thread);
			return true;
		}
	}
	return false;
}

bool  WebrtcConductor::DeleteRecieveFile(void * param)
{
	if (m_fileThreadManager)
	{
		char* guid = static_cast<char*>(param);
		if (guid)
		{
			std::string guidStr = guid;
			m_fileThreadManager->UnAttach(guidStr);
			return true;
		}
	}
	return false;
}


std::string& WebrtcConductor::RecieveFile(const std::string &guid, const std::string &filepath)//according the filepath create a file resource
{
	if (m_fileDataRepository)
	{
		FileResDescription resinfo;
		resinfo.type = File_Type;
		//resinfo.filepath = filepath;
		if (!resinfo.filepath)
		{
			return const_cast<std::string&>(emptyGoalbalString);
		}
		resinfo.filepath->SetString(filepath);
		resinfo.guid->SetString(guid);
		return m_fileDataRepository->CreateDataRepository(resinfo, new P2PFileDataRepositoryReciever());
	}
	return const_cast<std::string&>(emptyGoalbalString);
}

bool WebrtcConductor::SendFile(std::string &filename)//the filename is respository guid now in 2017/3/1
{
	//new a dataline and take the filedata send to opposite client
	if (m_fileDataRepository)
	{
		DataRepositoryFile * res = reinterpret_cast<DataRepositoryFile * >(m_fileDataRepository->GetRespositoryByGUID(filename));
		if (!res)
			return false;
		//set the sender task information to the threadbase attribute
		ThreadTaskDesFile senderFileDes(filename, res->GetFilePath());
		P2PThreadBase * thread = CreateP2PThreadBase(this);
		thread->SetSenderDes(senderFileDes);
		if (!m_fileThreadManager || !thread)
			return false;
		m_fileThreadManager->Attach(filename, thread);
		thread->SetTaskType(SEND_TASK);
		if (!m_fileThreadManager->Begin(filename))
			return false;
	}
	return true;
}

std::string&  WebrtcConductor::CreateBarcodeFile(void * res)
{
	if (res)
	{
		DataRepositoryFile * res1 = reinterpret_cast<DataRepositoryFile *>(res);
		FileResDescription resinfo;
		resinfo.type = File_Type;
		if (!resinfo.filepath)
			return const_cast<std::string &>(emptyGoalbalString);
		resinfo.filepath->SetString(res1->GetFilePath());
		resinfo.guid->SetString(res1->GetGUID());
		return m_fileDataRepository->CreateDataRepository(resinfo, const_cast<DataRepositoryFile *>(res1));
	}
	return const_cast<std::string &>(emptyGoalbalString);
}

bool WebrtcConductor::DeleteBasecodeFile(void * guid)
{
	if (guid)
	{
		char * guidChar = static_cast<char *>(guid);
		std::string guidString = guidChar;
		return m_fileDataRepository->DeleteDataRepository(guidString);
	}
	return false;
}

bool WebrtcConductor::SaveFileData(std::string &guid, void * data)//return this save deal is succese
{
	if (m_fileDataRepository)
	{
		if (m_fileDataRepository->IsExitRes(guid))
		{
			//this resource is exist so recieve the data directly
			return m_fileDataRepository->Save(guid, data);
		}
	}
	return false;
}

ConductorBase* WebrtcConductor::GetConductResource() //get the resource class ,now the class only has one resource repository
{
	return m_fileDataRepository;
}

TransferDataBase *	 WebrtcConductor::GetMessageObserver()
{
	return m_messageClientObserver;
}

bool WebrtcConductor::InitializePeerConnection()
{
	if (m_sessionConnectionObserver)
		return m_sessionConnectionObserver->InitializePeerConnection();
	return false;
}

bool WebrtcConductor::ReinitializePeerConnectionForLoopback()
{
	if (m_sessionConnectionObserver)
		return m_sessionConnectionObserver->ReinitializePeerConnectionForLoopback();
	return false;
}

bool WebrtcConductor::CreatePeerConnection(bool dtls)
{
	if (m_sessionConnectionObserver)
		return m_sessionConnectionObserver->CreatePeerConnection(dtls);
	return false;
}

void WebrtcConductor::DeletePeerConnection()
{
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->DeletePeerConnection();
	if (m_clientConnector)
	{
		WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
		if (clientOutter)
			clientOutter->SetCurrentConnectPeerInfo(NULL);
	}
}

void WebrtcConductor::AddStreams()
{
	if (m_sessionConnectionObserver)
	{
		m_sessionConnectionObserver->AddStreams();
	}
}


void WebrtcConductor::OnSignalingChange(int new_state)
{

}

void  WebrtcConductor::OnAddStream(void* stream)
{
	/*if (!stream)
	return;
	LOG(INFO) << __FUNCTION__ << " " << stream->label();
	stream->AddRef();*/
	CallbackWebrtcClient(NEW_STREAM_ADDED, stream);//inform It's has new stream is adding
}

void WebrtcConductor::OnRemoveStream(void* stream)
{
	/*if (!stream)
	return;
	LOG(INFO) << __FUNCTION__ << " " << stream->label();
	stream->AddRef();*/
	CallbackWebrtcClient(STREAM_REMOVED, stream);
}

void WebrtcConductor::OnDataChannel(void* channel)
{
	if (!channel)
		return;
	webrtc::DataChannelObserver * ptr = reinterpret_cast<webrtc::DataChannelObserver *>(m_messageClientObserver->GetMessageObserver());
	if (ptr)
	{
		webrtc::DataChannelInterface* channel1 = static_cast<webrtc::DataChannelInterface*>(channel);
		if (!channel1)
			return;
		channel1->RegisterObserver(ptr);//注册数据观察者 
	}
}

void WebrtcConductor::OnRenegotiationNeeded()
{

}

void WebrtcConductor::OnIceConnectionChange(int type, int new_state)
{
	//this is indicated the connection which with opposite is complate
	switch (new_state)
	{
	case Connect_Success:
	{
		OnPeerConnected(&new_state);
	}
	break;
	case Connect_Failure:
	{
		OnPeerDisconnected(&new_state);
	}
	break;
	case Disconnect:
	{
		OnNeedReConnectionTurnserver(type, new_state);
	}
	default:
		break;
	}
}

void WebrtcConductor::OnIceGatheringChange(int type, int new_state)
{
	switch (new_state)
	{
	case ConnectState::Connect_Success:
	{
		OnPeerConnectedServer(NULL);//connect turnserver succesfully
	}
	break;
	case ConnectState::Connect_Failure:
	{
		OnPeerDisConnectedServer(NULL);//connect turnserver failure
	}
	break;
	case ConnectState::Disconnect:
	{
		OnNeedReConnectionTurnserver(type, new_state);
	}
	break;
	default:
		break;
	}

}

void WebrtcConductor::DealOwerIceCandidate(const void* candidate)
{

}

void WebrtcConductor::OnIceCandidate(const void* candidate)
{
	DealOwerIceCandidate(candidate);
}

void WebrtcConductor::OnIceConnectionReceivingChange(bool receiving)
{

}

void WebrtcConductor::OnSignedIn(void * param)
{
	LOG(INFO) << __FUNCTION__;
	////should inform out client incadicate the webrtc client is login succese
	if (m_outterClient)
	{
		m_outterClient->OnMessage(ON_LOGIN_SUCCESE, param, NULL);
	}
}

void WebrtcConductor::OnDisconnected()
{
	LOG(INFO) << __FUNCTION__;
	DeletePeerConnection();
}

void WebrtcConductor::OnPeerConnected(void *id)
{
	//not check the m_peer_id_'s is or not valid ,according the message  that recieve from server or opposite client ,check the Legitimacy
	//when opposite stub , then it login again, so ,need connect the client afresh  
	if (m_outterClient)
	{
		m_outterClient->OnMessage(ON_CONNECT_PEER_SUCCESE, id, NULL);
	}
}

void  WebrtcConductor::OnPeerDisconnected(void * id)
{
	LOG(INFO) << __FUNCTION__;
	if (m_outterClient)
	{
		m_outterClient->OnMessage(ON_CONNECT_PEER_FAILURE, id, const_cast<void*>(GetOppositeID()));
	}
	//DeletePeerConnection();//not should execute this method
}

void WebrtcConductor::OnPeerConnectedServer(void* id)
{
	if (m_outterClient)
	{
		m_outterClient->OnMessage(ON_PEER_CONNECT_TO_SERVER, id, NULL);
	}
}

void WebrtcConductor::OnPeerDisConnectedServer(void* id)
{
	/*if (!IsValid())
	return;*/
	LOG(INFO) << __FUNCTION__;
	if (m_outterClient)
	{
		m_outterClient->OnMessage(ON_PEER_DISCONNECT_TO_SERVER, id, const_cast<void*>(GetOppositeID()));
	}
	//DeletePeerConnection();
}

void WebrtcConductor::OnNeedReConnectionTurnserver(const int & type, const int & state)
{
	LOG(INFO) << __FUNCTION__;
	if (m_outterClient)
	{
		m_outterClient->PeerNeedRebuildTurnserverConnection(type, state);
	}
}

void WebrtcConductor::DealOppositeSessionSDP(const void* message)
{
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->DealOppositeSessionSDP(message);
}

void WebrtcConductor::DealOppositeIceCandidate(const void* message)
{
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->DealOppositeIceCandidate(message);
}

void WebrtcConductor::StartLocalRenderer(void* local_video)
{
	webrtc::VideoTrackInterface* video = static_cast<webrtc::VideoTrackInterface*>(local_video);
	if (m_outterClient && video)
		m_outterClient->StartLocalRenderer(video);
}

void WebrtcConductor::StopLocalRenderer()
{
	if (m_outterClient)
		m_outterClient->StopLocalRenderer();
}

void WebrtcConductor::StartRemoteRenderer(void * remote_video)
{
	webrtc::VideoTrackInterface* video = static_cast<webrtc::VideoTrackInterface*>(remote_video);
	if (m_outterClient && video)
		m_outterClient->StartRemoteRenderer(video);
}

void WebrtcConductor::StopRemoteRenderer()
{
	if (m_outterClient)
		m_outterClient->StopRemoteRenderer();
}

void WebrtcConductor::OnMessageFromPeer(void *id, const std::string& message)
{
	if (!IsValid())
		return;
	if (message.empty())
		return;
	if (!Connection_Active())//!m_peer_connection_.get()
	{
		if (!SetOppositeID(id))
		{
			LOG(LS_ERROR) << "Failed to initialize because the id is invalid!";
			return;
		}

		if (!InitializePeerConnection())
		{
			LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
			assert(m_clientConnector);
			WebrtcClientOriginalInner * clientInner = m_clientConnector->GetClientInner();
			if (clientInner)
				clientInner->SignOut();//this is a dog 
			return;
		}
	}
	else if (!IsCurrentOppositeID(id))
	{
		LOG(LS_ERROR) << "Received a message from unknown peer while already in a ""conversation with a different peer.";
		return;
	}

	Json::Reader reader;
	Json::Value jmessage;
	if (!reader.parse(message, jmessage))
	{
		LOG(WARNING) << "Received unknown message. " << message;
		return;
	}
	std::string type;
	std::string json_object;

	/*rtc::*/GetStringFromJsonObject(jmessage, kSessionDescriptionTypeName_Super, &type);
	if (!type.empty())
	{
		DealOppositeSessionSDP(&jmessage);
	}
	else
	{
		DealOppositeIceCandidate(&jmessage);
	}
	if (m_outterClient != NULL)
		m_outterClient->OnMessage(ON_CONNECT_PEER_SUCCESE, NULL, NULL);
}

void WebrtcConductor::OnMessageSent(int err)
{
	//CallbackWebrtcClient(SEND_MESSAGE_TO_PEER, NULL);
}

void WebrtcConductor::OnServerConnectionFailure()
{
	if (m_sessionConnectionObserver)
	{
		std::string signalAddressStr = m_sessionConnectionObserver->SignalServerConnectionFailure();
		m_outterClient->ErrorMessageBoxSlots(("Failed to connect to " + signalAddressStr).c_str());
	}
}

void WebrtcConductor::StartLogin(const std::string& server, int port)
{
	assert(m_clientConnector);
	WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
	WebrtcClientOriginalInner  * clientInner = m_clientConnector->GetClientInner();
	if (!clientOutter || !clientInner)
		if (clientOutter->IsConnectSuccese())
			return;
	char port1[10];
	memset(port1, 0, sizeof(port1));
	_itoa_s(port, port1, sizeof(int), 10);
	ServerInfo info(server, port1);
	AddSinalServerInfo(info);
	std::string str = "";
	clientInner->Connect(server, port, str);
}

void WebrtcConductor::DisconnectFromServer()
{
	if (!m_clientConnector)
		return;
	WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
	WebrtcClientOriginalInner * clientInner = m_clientConnector->GetClientInner();
	if (clientOutter)
	{
		if (clientOutter->IsConnectSuccese())
		{
			if (clientInner)
			{
				clientInner->SignOut();
			}
			else
				LOG(INFO) << "clientInner is NULL!";
		}
		else
			LOG(INFO) << "client is nit connect success!";
	}
	else
		LOG(INFO) << "clientOutter is NULL!";
}

void WebrtcConductor::ConnectToPeer(void * info)
{
	if (!IsValid())
		return;
	if (!m_outterClient)
	{
		LOG(INFO) << __FUNCTION__ << "WebrtcConductor's param m_outterClient is NULL!";
		return;
	}
	if (Connection_Active())
	{
		m_outterClient->ErrorMessageBoxSlots("We only support connecting to one peer at a time");
		return;
	}

	//if (InitializePeerConnection())
	//{
	//record the current peer id，and save it to the WebrtcClientOriginal class
	if (SetOppositeID(info))
		m_sessionConnectionObserver->CreateOffer();
	else
		m_outterClient->ErrorMessageBoxSlots("opposite client's ID is invalid!");
	//}
	/*else
	{
	if (!m_outterClient)
	return;
	m_outterClient->ErrorMessageBoxSlots("Failed to initialize PeerConnection");
	}*/
}

void WebrtcConductor::DisconnectFromCurrentPeer()
{
	LOG(INFO) << __FUNCTION__ << "Start!";
	if (!IsValid())
		return;
	if (Connection_Active())
	{
		WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
		if (!clientOutter)
			return;
		clientOutter->SendHangUp(const_cast<void*>(GetOppositeID()));
		DeletePeerConnection();
	}
	else
	{
		LOG(INFO) << __FUNCTION__ << " m_peerConnection is invalid!";
	}
	LOG(INFO) << __FUNCTION__ << "End!";
}

void WebrtcConductor::CallbackWebrtcClient(int msg_id, void* data)
{
	WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
	WebrtcClientOriginalInner * clientInner = m_clientConnector->GetClientInner();
	if (!clientOutter || !clientInner)
		return;
	switch (msg_id)
	{
	/*case SEND_MESSAGE_TO_PEER:
	{
		LOG(INFO) << "SEND_MESSAGE_TO_PEER";
		std::string* msg = reinterpret_cast<std::string*>(data);
		if (!m_pending_messages_)
			break;
		std::deque<std::string*>* deque = reinterpret_cast<std::deque<std::string*>*>(m_pending_messages_->GetInfoVector());
		if (!deque)
			break;
		if (msg)
		{
			deque->push_back(msg);
		}

		if (!deque->empty() && !clientInner->IsSendingMessage())
		{
			msg = deque->front();
			deque->pop_front();

			if (!clientOutter->SendToPeer(*msg))
			{
				LOG(LS_ERROR) << "SendToPeer failed";
				DisconnectFromServer();
			}
			delete msg;
		}

		if (!Connection_Active())
		{
			int id = -1;
			SetOppositeID(&id);
		}

		break;
	}*/
	case CREATE_FILE_REPOSITORY:
	{
		CreateBarcodeFile(data);
	}
	break;
	case DELETE_FILE_REPOSITORY:
	{
		//this is delete the barcodefile and find the barcode file
		DeleteBasecodeFile(data);
	}
	break;
	case SEND_FILE_CHANNEL:
	{
		//send file 
		std::string *filepath = reinterpret_cast<std::string *>(data);
		if (filepath)
			SendFile(*filepath);
	}
	break;

	case RECIEVE_FILE_CHANNEL:
	{
		FileDataPack * msg = reinterpret_cast<FileDataPack *>(data);
		if (msg)
		{
			std::string tempStr = msg->owerInfo.rUserResGuid;
			SaveFileData(tempStr, msg->data);
			TransferProgressPacket * progress = TransferProgress(tempStr);
			if (progress)
			{
				//set the ack packet to the file sender
				std::string rUserResGuid = msg->owerInfo.rUserResGuid;
				FileDataBase msg1;
				msg1.message_id = File_Type_Shake_Pack;
				msg1.owerInfo.SetRuserResGuid(msg->owerInfo.rUserResGuid);
				msg1.owerInfo.SetSuserResGuid(msg->owerInfo.sUserResGuid);
				msg1.owerInfo.filepath.rUserID = progress->finishTaskSize/*progress * 1000*/;
				msg1.owerInfo.filepath.sUserID = progress->totalTaskSize;
				uint8_t buffer[3000];
				int serializeSize = msg1.Serialize(buffer, 3000);
				SendData(buffer, serializeSize);
			}
		}
	}
	break;
	case CREATE_FILE_CHANNEL://get the deal process to outclient, outclient will call the specific method
	{
		AcceptFile(data);
	}
	break;
	case INTERRUPT_FILE_TRANSFER_TASK:
	{
		//the message is send by reciever.
		FileDataPack * msg = reinterpret_cast<FileDataPack *>(data);
		if (msg)
		{
			if (m_fileThreadManager)
			{
				std::string guid = msg->owerInfo.sUserResGuid;
				P2PThreadBase * thread = m_fileThreadManager->GetThreadByGuid(guid);
				if (thread)
				{
					//only sender thread could been interrupt
					if (thread->GetTaskType() == SEND_TASK)
					{
						std::cout << "interrup file task:" << guid.c_str() << std::endl;
						m_fileThreadManager->SuspendChildThread(guid);
					}
				}
			}
		}
	}
	break;
	case RESTART_FILE_TRANSFER_TASK:
	{
		FileDataPack * msg = reinterpret_cast<FileDataPack *>(data);
		if (msg)
		{
			if (m_fileThreadManager)
			{
				std::string guid = msg->owerInfo.sUserResGuid;
				P2PThreadBase * thread = m_fileThreadManager->GetThreadByGuid(guid);
				if (thread)
				{
					//only sender thread could been interrupt
					if (thread->GetTaskType() == SEND_TASK)
					{
						std::cout << "restart  file task:" << guid.c_str() << std::endl;
						m_fileThreadManager->ResumeChildThread(guid);
					}
				}
			}
		}
	}
	break;
	case CANCLE_FILE_TRANSFER_TASK:
	{
		FileDataPack * msg = reinterpret_cast<FileDataPack *>(data);
		if (msg)
		{
			if (m_fileThreadManager)
			{
				std::string guid = msg->owerInfo.sUserResGuid;
				m_fileThreadManager->UnAttach(guid);//release the thread in threadManager
				if (m_outterClient)
				{
					//m_outterClient->OnFileCancel(guid.c_str());
					m_outterClient->TransferFileComplete(guid.c_str(), FILE_TRANSFER_CANCLE);
				}
			}
		}
	}
	break;
	case CAN_SEND_FILE: //can transfer file data
	{
		FileDataPack * msg = reinterpret_cast<FileDataPack *>(data);
		if (msg)
		{
			//continue send the file data ,add the opposite res guid to the FileDataPack::guid,
			//so, the opposite can Distinguish the file data into the correctly file
			if (!msg->data)
				break;
			ThreadTaskDesFile fileInfo(msg->owerInfo.rUserResGuid, msg->data->message);
			if (!m_fileThreadManager)
				break;
			std::string sUserResGuid = msg->owerInfo.sUserResGuid;
			P2PThreadBase * thread = static_cast<P2PThreadBase*>(m_fileThreadManager->GetThreadByGuid(sUserResGuid));
			if (thread)
			{
				thread->SetRecieveResDes(fileInfo);
				m_fileThreadManager->ResumeChildThread(sUserResGuid);
			}
		}
	}
	break;
	case NOT_CAN_SEND_FILE:
	{
		FileDataPack * msg = reinterpret_cast<FileDataPack *>(data);
		if (msg)
		{
			if (!msg->data)
				break;
			ThreadTaskDesFile fileInfo(msg->owerInfo.rUserResGuid, msg->data->message);
			if (!m_fileThreadManager)
				break;
			std::string guid = msg->owerInfo.sUserResGuid;
			P2PThreadBase * thread = static_cast<P2PThreadBase*>(m_fileThreadManager->GetThreadByGuid(guid));
			if (thread)
			{
				thread->SetRecieveResDes(fileInfo);
				thread->ForceQuit(true);
				m_fileThreadManager->ResumeChildThread(guid);
			}
		}
	}
	break;
	case SHAKE_ACK_PACKET://resume the goal file send thread
	{
		FileDataBase * msg = reinterpret_cast<FileDataBase *>(data);
		if (msg)
		{
			if (!m_fileThreadManager)
				break;
			if (m_outterClient)
			{
				std::string guid = msg->owerInfo.sUserResGuid;
				TransferProgressPacket * progress = new TransferProgressPacket(guid, msg->owerInfo.filepath.sUserID, msg->owerInfo.filepath.rUserID);
				//double progress = msg->owerInfo.filepath.rUserID / 1000.000;
				m_outterClient->TransferProgressSlots(&guid, progress);
				DeletePtr(progress);
			}
		}
	}
	break;
	case END_SEND_FILE:
	{
		//combination the file data ,caclute the new file md5 value ,compare it to the sender's  goalfile md5 value
		//if both compare is true It prove both file is equal .thus the two file is not same, It should be transfer file data retry,
		//the process maybe that transfer the defect file packet
		FileDataPack * msg = reinterpret_cast<FileDataPack *>(data);
		if (!msg)
			break;
		if (!m_fileDataRepository)
			break;
		std::string rUserResGuid = msg->owerInfo.rUserResGuid;
		std::string sUserResGuid = msg->owerInfo.sUserResGuid;
		std::string fileMd5 = m_fileDataRepository->GetResourceMd5(rUserResGuid);

		FileDataPack pack;
		pack = *msg;
		pack.message_id = File_Type;
		////////////
		FileTransferState recieveState = FILE_TRANSFER_FAILURE;
		if (fileMd5.compare(msg->data->message) != 0)
		{
			//LOG(INFO)<<"Failure: "<< "sender's file md5:" << msg->data->message<<"Filepath:"<< " reciever's file md5:" << fileMd5;
			pack.data->messageType = Failure_Recieve_File;
		}
		else
		{
			//LOG(INFO) << "Succese:" << "sender's file md5:" << msg->data->message << " reciever's file md5 : " << fileMd5;
			pack.data->messageType = Succese_Recieve_File;
			recieveState = FILE_TRANSFER_SUCESS;
		}
		uint8_t buffer[3000];
		int serializeSize = pack.Serialize(buffer, 3000);
		SendData(buffer, serializeSize);
		///////delete the recieve model which in m_fileThreadManager
		m_fileThreadManager->UnAttach(sUserResGuid);
		if (m_outterClient != NULL)
			m_outterClient->TransferFileComplete(msg->owerInfo.sUserResGuid, recieveState);
		else
			LOG(INFO) << "m_outterClient is NULL!so recreate the p2p environment";
	}
	break;
	case SEND_FILE_SUCCESS:
	{
		FileDataPack * msg = reinterpret_cast<FileDataPack *>(data);
		if (msg && m_outterClient)
		{
			m_outterClient->TransferFileComplete(msg->owerInfo.sUserResGuid, FILE_TRANSFER_SUCESS);
		}
	}
	break;
	case SEND_FILE_FAILURE:
	{
		FileDataPack * msg = reinterpret_cast<FileDataPack *>(data);
		if (msg && m_outterClient)
		{
			m_outterClient->TransferFileComplete(msg->owerInfo.sUserResGuid, FILE_TRANSFER_FAILURE);
		}
	}
	break;
	case NEW_STREAM_ADDED:
	{
		webrtc::MediaStreamInterface* stream = reinterpret_cast<webrtc::MediaStreamInterface*>(data);
		webrtc::VideoTrackVector tracks = stream->GetVideoTracks();
		// Only render the first track.
		stream->Release();
	}
	break;
	case STREAM_REMOVED:
	{
		// Remote peer stopped sending a stream.
		webrtc::MediaStreamInterface* stream = reinterpret_cast<webrtc::MediaStreamInterface*>(data);
		stream->Release();
	}
	break;
	case SEND_DATA_CHANNEL://DATA_CAHNNEL deal
	{

	}
	break;
	default:
	{
		assert(false);
	}
	break;
	}
}

void WebrtcConductor::DealOwerSessionSDP(void* desc)
{

}

void WebrtcConductor::OnSuccess(void* desc)
{
	/*m_peer_connection_->SetLocalDescription(DummySetSessionDescriptionObserverSuper::Create(), desc);*/
	DealOwerSessionSDP(desc);
	///this is indicate local client is connect to turnserver successfully!,so notify thing to outter object
}

void WebrtcConductor::OnFailure(const std::string& error)
{
	LOG(LERROR) << error;
	//correspond to OnSuccese's deal
	OnPeerDisConnectedServer(NULL);
}

void  WebrtcConductor::AddTurnServerInfo(const TurnserverInfo &info)
{
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->AddTurnServerInfo(info);
}

void WebrtcConductor::AddSinalServerInfo(const ServerInfo &info)
{
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->AddSinalServerInfo(info);
}

WebrtcClientOriginal *  WebrtcConductor::GetWebrtcClient()
{
	return m_clientConnector;
}

ClientOutterInterfaceOriginal *  WebrtcConductor::GetOutterClient()
{
	return m_outterClient;
}

TransferProgressPacket * WebrtcConductor::TransferProgress(std::string &resGuid)
{
	if (m_outterClient)
	{
		TransferProgressPacket * progress = GetConductResource()->ResProgressDisk(resGuid);
		if (progress)
		{
			m_outterClient->TransferProgressSlots(&resGuid, progress);
			return progress;
		}
	}
	return NULL;
}

const void* WebrtcConductor::GetOppositeID()const
{
	if (!m_clientConnector)
		return NULL;
	WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
	if (clientOutter)
		return clientOutter->GetCurrentConnectPeerInfo();
	return NULL;
}

bool WebrtcConductor::SetOppositeID(void * info)
{
	if (!m_clientConnector)
		return false;
	WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
	if (clientOutter)
		return clientOutter->SetCurrentConnectPeerInfo(info);
	return NULL;
}

const void*  WebrtcConductor::GetOwerID()const
{
	if (!m_clientConnector)
		return NULL;
	WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
	if (clientOutter)
		return clientOutter->GetClientKey();
	return NULL;
}

bool  WebrtcConductor::IsCurrentOppositeID(void *id)const
{
	if (m_clientConnector)
	{
		WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
		if (clientOutter)
			return clientOutter->IsCurrentOppositeID(id);
	}
	return false;
}

bool  WebrtcConductor::IsValid()
{
	if (!m_clientConnector)
		return false;
	WebrtcClientOriginalOutter * clientOutter = m_clientConnector->GetClientOutter();
	if (clientOutter)
		return clientOutter->IsConnectSuccese();
	return false;
}

void  WebrtcConductor::CallMessage(const std::string& json_object)
{
	//this msg memory is not delete by now ,because the msg is save in the m_pending_messages_ unique
	//std::string* msg = new std::string(json_object);
	//CallbackWebrtcClient(static_cast<int>(SEND_MESSAGE_TO_PEER), reinterpret_cast<void *>(msg));
}

void WebrtcConductor::CreateP2PThreadManager(P2PThreadManagerBase * value)
{
	m_fileThreadManager = value;
}

void WebrtcConductor::ReleaseP2PThreadManager()
{
	ReleaseP2PThreadManagerBase(m_fileThreadManager);
}

void WebrtcConductor::CreateConductor(ConductorBase * value)
{
	m_fileDataRepository = value;
}

void WebrtcConductor::ReleaseConductor()
{
	ReleaseConductorBase(m_fileDataRepository);
}

void  WebrtcConductor::SetOppositeSDPInformation(const std::string & sdp)
{
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->SetOppositeSDPInformation(sdp);
}
void  WebrtcConductor::SetOppositeIceCandidate(const std::vector<std::string >&icecandidate)
{
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->SetOppositeIceCandidate(icecandidate);
}

const std::string&   WebrtcConductor::GetOwerSDPInformation()
{
	return emptyGoalbalString;
}

const std::vector<std::string> * WebrtcConductor::GetOwerIceCandidate()
{
	return NULL;
}


/////////////
WEBRTCP2PDLLSHARED_EXPORT WebrtcConductorBase* CreateWebrtcConductorBase(WebrtcClientOriginalOutter *value1, ClientOutterInterfaceOriginal* value2)
{
	WebrtcClientOriginalInner * value = CreateWebrtcClientOriginalInner();
	WebrtcClientOriginal *peerConnection = CreateWebrtcClientOriginal(value, value1);
	return new WebrtcConductorDirect(peerConnection, value2);
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseWebrtcConductorBase(WebrtcConductorBase * value)
{
	DeletePtr(value);
}
//////////////////////
///////////////WebrtcClientOutterIterator decorator class
class WEBRTCP2PDLLSHARED_EXPORT WebrtcConductorDecoratorExe :public WebrtcConductorDecorator
{
public:
	//typedef rtc::scoped_refptr<WebrtcConductor>  WebrtcConductor_Scoped;
	//typedef rtc::RefCountedObject<WebrtcConductor> WebrtcConductor_RefObject;
public:
	WebrtcConductorDecoratorExe() = delete;

	WebrtcConductorDecoratorExe(WebrtcConductorBase *conductor);

	virtual ~WebrtcConductorDecoratorExe();
public:
	//WebrtcClientOutterIterator's interface
	virtual void CallbackWebrtcClient(int msg_id, void* data);  //call the webrtc conductor 
																//WebrtcClientObserver's interface
	virtual void OnSignedIn(void * param);

	virtual void OnDisconnected();

	virtual void OnPeerConnected(void * id);

	virtual void OnPeerDisconnected(void* peer_id);

	virtual void OnPeerConnectedServer(void* id);

	virtual void OnPeerDisConnectedServer(void* id);

	virtual void OnMessageFromPeer(void* peer_id, const std::string& message);

	virtual void OnMessageSent(int err);

	virtual void OnServerConnectionFailure();
	//the way which outter interface want to know, they must  call the function 
	virtual bool Connection_Active();

	virtual bool LoginInSuccese();
	//////////////ConductorBusiness interface
	virtual void CreateP2PThreadManager(P2PThreadManagerBase * value);
	virtual void ReleaseP2PThreadManager();
	virtual void CreateConductor(ConductorBase * value);
	virtual void ReleaseConductor();
	virtual void AddTurnServerInfo(const TurnserverInfo &info)override;
	virtual void AddSinalServerInfo(const ServerInfo &info)override;

	virtual void  SetOppositeSDPInformation(const std::string & sdp);
	virtual void  SetOppositeIceCandidate(const std::vector<std::string >&icecandidate);
	virtual const std::string&   GetOwerSDPInformation();
	const std::vector<std::string> * GetOwerIceCandidate();

	virtual void  AcceptFile(void * param);
	virtual void  RefuseFile(void * param);
	virtual bool  InterruptFileTask(void * param);
	virtual bool  RestartFileTask(void * param);
	virtual bool  CancleFileTask(void * param);
	virtual bool  CreateRecieveFile(void * param, void * param1);
	virtual bool  DeleteRecieveFile(void * param);
	virtual bool InitializePeerConnection();

protected:
	WebrtcConductorBase *m_WebrtcConductor;
};
/////////////////////WebrtcConductorDecorator
//WebrtcConductorDecoratorExe::WebrtcConductorDecoratorExe() :m_WebrtcConductor(NULL)
//{
//	CreateP2PThreadManager(CreateP2PThreadManagerBase());
//	CreateConductor(new ConductorPeerConnection(CreatePeerConnectIterator()));
//}

WebrtcConductorDecoratorExe::WebrtcConductorDecoratorExe(WebrtcConductorBase *conductor)
{
	m_WebrtcConductor = conductor;
	CreateP2PThreadManager(CreateP2PThreadManagerBase());
	CreateConductor(new ConductorPeerConnection(CreatePeerConnectIterator()));
	if (m_WebrtcConductor)
	{
		if (m_WebrtcConductor->GetWebrtcClient())
			m_WebrtcConductor->GetWebrtcClient()->RegisterObserver(this);
		if (m_WebrtcConductor->GetOutterClient())
			m_WebrtcConductor->GetOutterClient()->RegisterObserver(this);
	}
}

WebrtcConductorDecoratorExe::~WebrtcConductorDecoratorExe()
{
	ReleaseP2PThreadManager();
	ReleaseConductor();
	if (m_WebrtcConductor)
		m_WebrtcConductor->Close();
	DeletePtr(m_WebrtcConductor);
	m_WebrtcConductor = NULL;
}


void WebrtcConductorDecoratorExe::CallbackWebrtcClient(int msg_id, void* data)
{
	if (!m_WebrtcConductor)
		return;
	WebrtcConductor *ptr = (WebrtcConductor *)(m_WebrtcConductor);
	if (!ptr)
		return;
	switch (msg_id)
	{
	case LOGIN_IN_SERVER:
	{
		PeerInfo * peerInfo = reinterpret_cast<PeerInfo *>(data);
		if (peerInfo)
			ptr->StartLogin(peerInfo->username->GetString(), peerInfo->id);
	}
	break;
	case LOGIN_OUT_SERVER:
	{
		ptr->DisconnectFromServer();
	}
	break;
	case CONNECT_PEER:
	{
		ptr->ConnectToPeer(data);
	}
	break;
	case DISCONNECT_PEER:
	{
		ptr->DisconnectFromCurrentPeer();
	}
	break;
	case SEND_DATA_CHANNEL:
	{
		MessageDeliverPacket * param = static_cast<MessageDeliverPacket *>(data);
		FileDataPack * ptr1 = static_cast<FileDataPack *>(param->data);
		if (param)
			ptr->SendData(param->data, param->datasize);
	}
	break;
	default:
	{
		ptr->CallbackWebrtcClient(msg_id, data);
	}
	break;
	}
}

void WebrtcConductorDecoratorExe::OnSignedIn(void * param)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->OnSignedIn(param);
}

void WebrtcConductorDecoratorExe::OnDisconnected()
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->OnDisconnected();
}

void WebrtcConductorDecoratorExe::OnPeerConnected(void * id)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->OnPeerConnected(id);
}

void WebrtcConductorDecoratorExe::OnPeerDisconnected(void* peer_id)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->OnPeerDisconnected(peer_id);
}

void WebrtcConductorDecoratorExe::OnPeerConnectedServer(void* id)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->OnPeerConnectedServer(id);
}

void WebrtcConductorDecoratorExe::OnPeerDisConnectedServer(void* id)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->OnPeerDisConnectedServer(id);
}

void WebrtcConductorDecoratorExe::OnMessageFromPeer(void* peer_id, const std::string& message)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->OnMessageFromPeer(peer_id, message);
}

void WebrtcConductorDecoratorExe::OnMessageSent(int err)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->OnMessageSent(err);
}

void WebrtcConductorDecoratorExe::OnServerConnectionFailure()
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->OnServerConnectionFailure();
}
//the way which outter interface want to know, they must  call the function 
bool WebrtcConductorDecoratorExe::Connection_Active()
{
	return m_WebrtcConductor && m_WebrtcConductor->Connection_Active();
}

bool WebrtcConductorDecoratorExe::LoginInSuccese()
{

	return m_WebrtcConductor && m_WebrtcConductor->IsLoginSuccese();
}

void WebrtcConductorDecoratorExe::CreateP2PThreadManager(P2PThreadManagerBase * value)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->CreateP2PThreadManager(value);
}

void WebrtcConductorDecoratorExe::ReleaseP2PThreadManager()
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->ReleaseP2PThreadManager();
}

void WebrtcConductorDecoratorExe::CreateConductor(ConductorBase * value)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->CreateConductor(value);
}

void WebrtcConductorDecoratorExe::ReleaseConductor()
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->ReleaseConductor();
}

void WebrtcConductorDecoratorExe::AddTurnServerInfo(const TurnserverInfo &info)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->AddTurnServerInfo(info);
}

void WebrtcConductorDecoratorExe::AddSinalServerInfo(const ServerInfo &info)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->AddSinalServerInfo(info);
}

void  WebrtcConductorDecoratorExe::SetOppositeSDPInformation(const std::string & sdp)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->SetOppositeSDPInformation(sdp);
}

void  WebrtcConductorDecoratorExe::SetOppositeIceCandidate(const std::vector<std::string >&icecandidate)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->SetOppositeIceCandidate(icecandidate);
}

const std::string&   WebrtcConductorDecoratorExe::GetOwerSDPInformation()
{
	if (m_WebrtcConductor)
		return m_WebrtcConductor->GetOwerSDPInformation();
	return emptyGoalbalString;
}

const std::vector<std::string> * WebrtcConductorDecoratorExe::GetOwerIceCandidate()
{
	if (m_WebrtcConductor)
		return m_WebrtcConductor->GetOwerIceCandidate();
	return NULL;
}

void  WebrtcConductorDecoratorExe::AcceptFile(void * param)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->AcceptFile(param);
}

void  WebrtcConductorDecoratorExe::RefuseFile(void * param)
{
	if (m_WebrtcConductor)
		m_WebrtcConductor->RefuseFile(param);
}

bool  WebrtcConductorDecoratorExe::InterruptFileTask(void * param)
{
	if (m_WebrtcConductor)
		return m_WebrtcConductor->InterruptFileTask(param);
	return false;
}

bool  WebrtcConductorDecoratorExe::RestartFileTask(void * param)
{
	if (m_WebrtcConductor)
		return m_WebrtcConductor->RestartFileTask(param);
	return false;
}

bool  WebrtcConductorDecoratorExe::CancleFileTask(void * param)
{
	if (m_WebrtcConductor)
		return m_WebrtcConductor->CancleFileTask(param);
	return false;
}

bool  WebrtcConductorDecoratorExe::CreateRecieveFile(void * param, void * param1)
{
	if (m_WebrtcConductor)
		return m_WebrtcConductor->CreateRecieveFile(param, param1);
	return false;
}

bool  WebrtcConductorDecoratorExe::DeleteRecieveFile(void * param)
{
	if (m_WebrtcConductor)
		return m_WebrtcConductor->DeleteRecieveFile(param);
	return false;
}

bool WebrtcConductorDecoratorExe::InitializePeerConnection()
{
	if (m_WebrtcConductor)
		return m_WebrtcConductor->InitializePeerConnection();
	return false;
}


WEBRTCP2PDLLSHARED_EXPORT WebrtcConductorDecorator * CreateWebrtcConductorDecorator(WebrtcClientOriginalOutter *value1, ClientOutterInterfaceOriginal* value2)
{
	WebrtcConductorBase * value = CreateWebrtcConductorBase(value1, value2);
	return new WebrtcConductorDecoratorExe(value);
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseWebrtcConductorDecorator(WebrtcConductorDecorator * value)
{
	DeletePtr(value);
}
//////////////////
WebrtcConductorDirect::WebrtcConductorDirect(WebrtcClientOriginal* peerConnectClient, ClientOutterInterfaceOriginal *outterUserClient)
	:WebrtcConductor(peerConnectClient, outterUserClient)
{

}

WebrtcConductorDirect::~WebrtcConductorDirect()
{

}

void WebrtcConductorDirect::DealOwerSessionSDP(void* desc)
{
	webrtc::SessionDescriptionInterface *desc1 = static_cast<webrtc::SessionDescriptionInterface*>(desc);
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->DealOwerSessionSDP(desc1);
}

void WebrtcConductorDirect::DealOwerIceCandidate(const void* candidate)
{
	const webrtc::IceCandidateInterface * candidate1 = static_cast<const webrtc::IceCandidateInterface *>(candidate);
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->DealOwerIceCandidate(candidate1);
}

void  WebrtcConductorDirect::SetOppositeSDPInformation(const std::string & sdp)
{
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->SetOppositeSDPInformation(sdp);
}

void  WebrtcConductorDirect::SetOppositeIceCandidate(const std::vector<std::string >&icecandidate)
{
	if (m_sessionConnectionObserver)
		m_sessionConnectionObserver->SetOppositeIceCandidate(icecandidate);
}

const std::string&   WebrtcConductorDirect::GetOwerSDPInformation()
{
	if (m_sessionConnectionObserver)
		return m_sessionConnectionObserver->GetOwerSDPInformation();
	return emptyGoalbalString;
}

const std::vector<std::string> * WebrtcConductorDirect::GetOwerIceCandidate()
{
	if (m_sessionConnectionObserver)
		return m_sessionConnectionObserver->GetOwerIceCandidate();
	return NULL;
}


void WebrtcConductorDirect::ConnectToPeer(void * info)
{
	if (!m_outterClient)
	{
		LOG(INFO) << __FUNCTION__ << "WebrtcConductor's param m_outterClient is NULL!";
		return;
	}
	if (m_sessionConnectionObserver != NULL)
		m_sessionConnectionObserver->CreateOffer();
}

void WebrtcConductorDirect::DisconnectFromCurrentPeer()
{
	if (Connection_Active())
	{
		DeletePeerConnection();
	}
	else
	{
		LOG(INFO) << __FUNCTION__ << " m_peerConnection is invalid!";
	}
}

void WebrtcConductorDirect::Close()
{
	DeletePeerConnection();
}

////////////////packet class
SessionConnectionObserver::SessionConnectionObserver(WebrtcConductorBase * parent)
{
	SetParent(parent);
	m_loopback_ = false;
	m_peer_connection_ = NULL;
	m_peer_connection_factory_ = NULL;
	m_active_streams_ = CreateContainer(TransferNamespace::StreamActiveMap);
	m_sendTemporaryQueue = CreateContainer(TransferNamespace::DataBufferQueue);
	m_messageChannel = NULL;
	m_channelState = webrtc::DataChannelInterface::DataState::kClosed;

	//////vector Initialize
	m_turnserverVector = CreateContainer(TransferNamespace::TurnserverInfoVector);
	//TurnserverInfo * turninfo = new TurnserverInfo("120.76.112.94", "3478", "helloword", "helloword", "");
	//AddTurnServerInfo(*turninfo);
	//TurnserverInfo * stuninfo = new TurnserverInfo("120.76.112.94", "3478", "helloword", "helloword", "");
	//AddTurnServerInfo(*stuninfo);
	m_signalServerVector = CreateContainer(TransferNamespace::ServerInfoVector);
	//DeletePtr(turninfo);
	//DeletePtr(stuninfo);

	m_owerSDPInfo = CreateSTLString();
	m_oppositeSDPInfo = CreateSTLString();
	m_owericeCandidateInfo = CreateContainer(TransferNamespace::StringVector);
	m_oppositeiceCandidateInfo = CreateContainer(TransferNamespace::StringVector);
	m_connectedToPeerFlag = false;
	m_connectedToTurnserverFlag = ConnectState::Connect_Zero;
	//m_sendDataThread = new rtc::Win32Thread();
	//m_sendDataThread->Start();
	m_getIceCandidates = false;
	m_candiatesGatheringSuccess = false;
}

SessionConnectionObserver::~SessionConnectionObserver()
{
	/*assert(m_peer_connection_.get() == NULL);
	m_parent = NULL;
	m_messageChannel = NULL;
	m_loopback_ = false;
	m_peer_connection_ = NULL;
	m_peer_connection_factory_ = NULL;*/
	DeletePeerConnection();
	//clear the packet of the m_sendTemporaryQueue
	DeleteContainer(m_active_streams_);
	DeleteContainer(m_sendTemporaryQueue);

	DeleteContainer(m_turnserverVector);
	DeleteContainer(m_signalServerVector);

	DeleteSTLString(m_owerSDPInfo);
	DeleteSTLString(m_oppositeSDPInfo);
	DeleteContainer(m_owericeCandidateInfo);
	DeleteContainer(m_oppositeiceCandidateInfo);
	m_connectedToPeerFlag = false;
	m_getIceCandidates = false;
	m_candiatesGatheringSuccess = false;
	m_connectedToTurnserverFlag = ConnectState::Connect_Zero;
	//DeletePtr(m_sendDataThread);
}

bool SessionConnectionObserver::SessionConnectionIsValid()
{
	if (m_peer_connection_.get() == NULL)
		return false;
	return true;
}

webrtc::DataChannelInterface::DataState & SessionConnectionObserver::GetMessageChannelState()
{
	if (m_messageChannel)
	{
		m_channelState = m_messageChannel->state();
	}
	return m_channelState;
}

void SessionConnectionObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
	if (m_peer_connection_)
		m_peer_connection_->SetLocalDescription(DummySetSessionDescriptionObserverSuper::Create(), desc);
	if (m_parent)
	{
		m_parent->OnSuccess(desc);
	}
}

void SessionConnectionObserver::OnFailure(const std::string& error)
{
	if (m_parent)
		m_parent->OnFailure(error);
}

void SessionConnectionObserver::OnError()
{

}

void SessionConnectionObserver::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
	if (m_parent)
		m_parent->OnSignalingChange(new_state);
}

void SessionConnectionObserver::OnAddStream(webrtc::MediaStreamInterface* stream)
{
	if (!stream)
		return;
	LOG(INFO) << __FUNCTION__ << " " << stream->label();
	stream->AddRef();
	if (m_parent)
		m_parent->OnAddStream(stream);
}

void SessionConnectionObserver::OnRemoveStream(webrtc::MediaStreamInterface* stream)
{
	if (!stream)
		return;
	LOG(INFO) << __FUNCTION__ << " " << stream->label();
	stream->AddRef();
	if (m_parent)
		m_parent->OnRemoveStream(stream);
}

void SessionConnectionObserver::OnDataChannel(webrtc::DataChannelInterface* channel)
{
	if (!channel)
		return;
	//channel->RegisterObserver(m_messageClientObserver);//注册数据观察者 
	m_messageChannel = NULL;
	m_messageChannel = channel;
	if (m_parent)
		m_parent->OnDataChannel(channel);
}

void SessionConnectionObserver::OnRenegotiationNeeded()
{
	if (m_parent)
		m_parent->OnRenegotiationNeeded();
}

void SessionConnectionObserver::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
	if (m_parent)
	{
		if (new_state == webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionConnected)
		{
			if (!m_connectedToPeerFlag)
			{
				m_connectedToPeerFlag = true;
				m_parent->OnIceConnectionChange(PeerClient, Connect_Success);
			}
		}
		else if (new_state == webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionFailed
			|| new_state == webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionClosed)
		{
			m_connectedToPeerFlag = false;
			m_parent->OnIceConnectionChange(PeerClient, Connect_Failure);
		}
		else if (new_state == webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionDisconnected)
		{
			m_connectedToPeerFlag = false;
			m_parent->OnIceConnectionChange(PeerClient, Disconnect);
		}
	}
}

ConnectState SessionConnectionObserver::OfferClientGatheringComplete(const webrtc::IceCandidateCollection * icecandidates)
{
	ConnectState state = ConnectState::Connect_Failure;
	TransferNamespace::TurnserverInfoVector* turnserverInfoVector = reinterpret_cast<TransferNamespace::TurnserverInfoVector*>(m_turnserverVector);
	if (turnserverInfoVector && turnserverInfoVector->Match(icecandidates))//information is correct
	{
		if (m_getIceCandidates)
			state = ConnectState::Connect_Success;
		else
			state = ConnectState::Disconnect;
	}
	else
		state = ConnectState::Connect_Failure;
	return state;
}

ConnectState SessionConnectionObserver::AnswerClientGatheringComplete(const webrtc::IceCandidateCollection * icecandidates)
{
	ConnectState state = ConnectState::Connect_Failure;
	if (icecandidates->count() > 0)
	{
		if (m_getIceCandidates)
			state = ConnectState::Connect_Success;
		else
			state = ConnectState::Disconnect;
	}
	else
		state = ConnectState::Connect_Failure;
	return state;
}

ConnectState SessionConnectionObserver::RecieveGatheringComplete()
{
	ConnectState connectTurnserverState = ConnectState::Connect_Failure;
	const webrtc::SessionDescriptionInterface *localDes = m_peer_connection_->local_description();
	if (localDes)
	{
		size_t candidateCounts = localDes->number_of_mediasections();
		std::string type = localDes->type();
		for (size_t i = 0; i < candidateCounts;i++) 
		{
			const webrtc::IceCandidateCollection * icecandidates = localDes->candidates(i);
			if (type.compare("offer") == 0)
			{
				connectTurnserverState = OfferClientGatheringComplete(icecandidates);
			}
			else
			{
				connectTurnserverState = AnswerClientGatheringComplete(icecandidates);
			}
			if (connectTurnserverState == ConnectState::Connect_Success)
				break;
		}
	}
	else
		connectTurnserverState = ConnectState::Connect_Failure;
	return connectTurnserverState;
}

void SessionConnectionObserver::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
	if (m_parent)
	{
		ConnectState state = ConnectState::Connect_Failure;
		webrtc::PeerConnectionInterface::IceGatheringState icegatheringState = m_peer_connection_->ice_gathering_state();
		switch (icegatheringState)
		{
		case webrtc::PeerConnectionInterface::IceGatheringState::kIceGatheringComplete:
		{
			state = RecieveGatheringComplete();
			if (m_connectedToTurnserverFlag != state)
			{
				if ((ConnectState::Connect_Failure == m_connectedToTurnserverFlag || ConnectState::Disconnect == m_connectedToTurnserverFlag)
					&& ConnectState::Connect_Success == state)
					break;
				m_connectedToTurnserverFlag = state;
				if (!m_getIceCandidates&& ConnectState::Connect_Success == m_connectedToTurnserverFlag)
					break;
				m_getIceCandidates = false;
				m_parent->OnIceGatheringChange(TurnServer, m_connectedToTurnserverFlag);
			}
		}
		break;
		default:
			break;
		}
			
	}
	LOG(INFO) << __FUNCTION__ << "FINISH GATHERING!";
}

void SessionConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
	m_getIceCandidates = true;
	if (!candidate)
		return;
	LOG(INFO) << __FUNCTION__ << " " << candidate->sdp_mline_index();
	LOG(INFO) << "IceCandidate's info:";
	//LOG(INFO) << candidate->candidate().ToString();

	if (m_loopback_)
	{
		if (m_peer_connection_ && !m_peer_connection_->AddIceCandidate(candidate))
		{
			LOG(WARNING) << "Failed to apply the received candidate";
		}
		return;
	}
	if (m_parent)
		m_parent->OnIceCandidate(candidate);
}

void SessionConnectionObserver::OnIceConnectionReceivingChange(bool receiving)
{
	if (m_parent)
		m_parent->OnIceConnectionReceivingChange(receiving);
}

bool SessionConnectionObserver::InitializePeerConnection()//m_parent trigger the function
{
	assert(m_peer_connection_factory_.get() == NULL);
	assert(m_peer_connection_.get() == NULL);

	m_peer_connection_factory_ = webrtc::CreatePeerConnectionFactory();
	if (!m_peer_connection_factory_.get())
	{
		DeletePeerConnection();
		return false;
	}

	if (!CreatePeerConnection(DTLS_ON))//
	{
		DeletePeerConnection();
	}
	AddStreams();
	return m_peer_connection_.get() != NULL;
}

bool SessionConnectionObserver::ReinitializePeerConnectionForLoopback()//m_parent trigger the function
{
	m_loopback_ = true;
	rtc::scoped_refptr<webrtc::StreamCollectionInterface> streams(m_peer_connection_->local_streams());
	m_peer_connection_ = NULL;
	if (CreatePeerConnection(DTLS_OFF))
	{
		for (size_t i = 0; i < streams->count(); ++i)
			m_peer_connection_->AddStream(streams->at(i),NULL);
		m_peer_connection_->CreateOffer(this, NULL);
	}
	return m_peer_connection_.get() != NULL;
}

bool SessionConnectionObserver::CreatePeerConnection(bool dtls)////m_parent trigger the function
{
	assert(m_peer_connection_factory_.get() != NULL);
	assert(m_peer_connection_.get() == NULL);
	if (!m_turnserverVector)
		return false;
	std::vector<TurnserverInfo>* turnserverInfoVector = reinterpret_cast<std::vector<TurnserverInfo>*>(m_turnserverVector->GetInfoVector());
	if (!turnserverInfoVector)
		return false;
	webrtc::PeerConnectionInterface::RTCConfiguration config;
	//loop init stunserver address information
	webrtc::PeerConnectionInterface::IceServer server1;
	std::string stunserver = "stun:";
	stunserver += (*turnserverInfoVector)[0].GetAddressPort();
	server1.uri = stunserver;
	//server1.username = (*turnserverInfoVector)[0].username->GetString();
	server1.username = (*turnserverInfoVector)[0].GetUserNameEx().c_str();
	//server1.password = (*turnserverInfoVector)[0].password->GetString();
	server1.password = (*turnserverInfoVector)[0].GetPassword().c_str();
	config.servers.push_back(server1);
	//turnserver information 
	webrtc::PeerConnectionInterface::IceServer server2;
	std::string stunserver1 = "turn:";
	stunserver1 += (*turnserverInfoVector)[0].GetAddressPort();
	server2.uri = stunserver1;
	//server2.username = (*turnserverInfoVector)[0].username->GetString();
	server2.username = (*turnserverInfoVector)[0].GetUserNameEx().c_str();
	//server2.password = (*turnserverInfoVector)[0].password->GetString();
	server2.password = (*turnserverInfoVector)[0].GetPassword().c_str();
	config.servers.push_back(server2);

	webrtc::FakeConstraints constraints;
	if (dtls)
	{
		constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "true");
	}
	else
	{
		constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "false");
	}
	//constraints.SetMandatoryIceRestart(false);
	constraints.SetMandatoryIceRestart(true);
	m_peer_connection_ = m_peer_connection_factory_->CreatePeerConnection(config, &constraints, NULL, NULL, this);
	return m_peer_connection_.get() != NULL;
}

void SessionConnectionObserver::DeletePeerConnection()
{
	m_messageChannel = NULL;
	if (m_peer_connection_)
	{
		m_peer_connection_ = NULL;
	}
	if (m_active_streams_)
	{
		std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>>* streamMap = \
			reinterpret_cast<std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> >*>(m_active_streams_->GetInfoVector());
		if (streamMap != NULL)
			streamMap->clear();
	}
	if (m_peer_connection_factory_)
	{
		m_peer_connection_factory_ = NULL;
	}
	m_loopback_ = false;
	m_connectedToPeerFlag = false;
}

void SessionConnectionObserver::AddStreams()
{
	if (!m_active_streams_)
		return;
	std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>>* streamMap = \
		reinterpret_cast<std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> >*>(m_active_streams_->GetInfoVector());
	if (!streamMap)
		return;
	if (streamMap->find(kStreamLabel) != streamMap->end())
		return;
	webrtc::DataChannelInit datainit;
	m_messageChannel = m_peer_connection_->CreateDataChannel(kDataChannel, &datainit);
	if (m_parent)
	{
		TransferDataBase * ptr = reinterpret_cast<TransferDataBase *>(m_parent->GetMessageObserver());
		if (ptr)
		{
			if (m_messageChannel)
				m_messageChannel->RegisterObserver(ptr->GetMessageObserver());//注册消息客户
		}
	}

	//rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
	//	m_peer_connection_factory_->CreateAudioTrack(
	//		kAudioLabel, m_peer_connection_factory_->CreateAudioSource(NULL)));

	//rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
	//	m_peer_connection_factory_->CreateVideoTrack(
	//		kVideoLabel,
	//		m_peer_connection_factory_->CreateVideoSource((cricket::VideoCapturer*)(OpenVideoCaptureDevice()),
	//			NULL)));
	////main_wnd_->StartLocalRenderer(video_track);

	//rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
	//	m_peer_connection_factory_->CreateLocalMediaStream(kStreamLabel);
	//stream->AddTrack(audio_track);
	//stream->AddTrack(video_track);
	//if (!m_peer_connection_->AddStream(stream)) {
	//	LOG(LS_ERROR) << "Adding stream to PeerConnection failed";
	//}
	/////add by  in 2016.10.20
	//////////
	//typedef std::pair<std::string,
	//	rtc::scoped_refptr<webrtc::MediaStreamInterface> >
	//	MediaStreamPair;
	//streamMap->insert(MediaStreamPair(stream->label(), stream));
}

WebrtcConductorBase* SessionConnectionObserver::GetParent()
{
	return m_parent;
}

void SessionConnectionObserver::SetParent(WebrtcConductorBase * parent)
{
	m_parent = parent;
}

bool SessionConnectionObserver::SendData(void *message, size_t size)
{
	if (!m_messageChannel)
		return false;
	bool sendResults = false;
	/*webrtc::DataChannelInterface::DataState state = m_messageChannel->state();*/
	if (GetMessageChannelState() != webrtc::DataChannelInterface::kOpen)
		return sendResults;
	const size_t datasize = size;
	uint8_t * data = new uint8_t[size];
	memset(data, 0, size);
	::memcpy(data, message, size);
	int retransmissionTimes = Re_Send_Times_WhenFailure;
	if (!m_sendTemporaryQueue)
		return false;

	//rtc::CopyOnWriteBuffer buffer(data, size);
	rtc::Buffer buffer(data, size);
	while (retransmissionTimes)
	{
		////control the webrtc send wnd, It's a dynamic process
		//if current send queued is run up to the condition, then push the packet to the temporary queue
		if (!m_messageChannel)
			break;
		double percent_send_queue = m_messageChannel->SendQueueSurplus();
		if (percent_send_queue < 1)
		{
			sendResults = m_messageChannel->Send(webrtc::DataBuffer(buffer, false)/*queue->front()*/);
			if (sendResults)
			{
				//queue->pop();//when send packet succese, popup the packet from queue 
				break;
			}
		}
		retransmissionTimes--;
		Sleep(100);//padding time
	}
	delete data;
	data = NULL;
	return sendResults;
}

void SessionConnectionObserver::AddTurnServerInfo(const TurnserverInfo &info)
{
	std::vector<TurnserverInfo>* turnserverInfoVector = reinterpret_cast<std::vector<TurnserverInfo>*>(m_turnserverVector->GetInfoVector());
	if (!turnserverInfoVector)//the turnserverInfoVector is NULL, so quit the program directly
		return;
	turnserverInfoVector->push_back(info);
}

void SessionConnectionObserver::AddSinalServerInfo(const ServerInfo &info)
{
	std::vector<ServerInfo>* serverInfoVector = reinterpret_cast<std::vector<ServerInfo>*>(m_signalServerVector->GetInfoVector());
	if (!serverInfoVector)//the turnserverInfoVector is NULL, so quit the program directly
		return;
	serverInfoVector->push_back(info);
}

void SessionConnectionObserver::DealOwerSessionSDP(webrtc::SessionDescriptionInterface* desc)
{
	if (!desc)
		return;
	std::string sdp;
	desc->ToString(&sdp);

	// For loopback test. To save some connecting delay.
	if (m_loopback_)
	{
		// Replace message type from "offer" to "answer"
		webrtc::SessionDescriptionInterface* session_description(webrtc::CreateSessionDescription("answer", sdp,
			nullptr));
		m_peer_connection_->SetRemoteDescription(DummySetSessionDescriptionObserverSuper::Create(), session_description);
		return;
	}
	Json::StyledWriter writer;
	Json::Value jmessage;
	jmessage[kSessionDescriptionTypeName_Super] = desc->type();
	jmessage[kSessionDescriptionSdpName_Super] = sdp;
	if (m_parent)
		m_parent->CallMessage(writer.write(jmessage));
	if (!m_owerSDPInfo)
		return;

	std::vector<std::string>* vector = reinterpret_cast<std::vector<std::string> *>(m_owericeCandidateInfo->GetInfoVector());
	if (vector)
		vector->clear();
	if (m_owerSDPInfo)
	{
		m_owerSDPInfo->clear();
		m_owerSDPInfo->SetString(writer.write(jmessage));
	}
}

void SessionConnectionObserver::DealOwerIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
	Json::StyledWriter writer;
	Json::Value jmessage;

	jmessage[kCandidateSdpMidName_Super] = candidate->sdp_mid();
	jmessage[kCandidateSdpMlineIndexName_Super] = candidate->sdp_mline_index();
	std::string sdp;
	if (!candidate->ToString(&sdp))
	{
		LOG(LS_ERROR) << "Failed to serialize candidate";
		return;
	}
	jmessage[kCandidateSdpName_Super] = sdp;
	if (m_parent)
		m_parent->CallMessage(writer.write(jmessage));
	std::vector<std::string> * vector = reinterpret_cast<std::vector<std::string>*>(m_owericeCandidateInfo->GetInfoVector());
	if (vector)
		vector->push_back(writer.write(jmessage));
}

void SessionConnectionObserver::DealOppositeSessionSDP(const void* message1)
{
	const Json::Value * message = static_cast<const Json::Value *>(message1);
	if (!message)
		return;
	std::string type;
	/*rtc::*/GetStringFromJsonObject(*message, kSessionDescriptionTypeName_Super, &type);
	if (type == "offer-loopback")
	{
		// This is a loopback call.
		// Recreate the peerconnection with DTLS disabled.
		if (!ReinitializePeerConnectionForLoopback())
		{
			LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
			DeletePeerConnection();
			/*m_clientConnector->SignOut();*/
		}
		return;
	}

	std::string sdp;
	if (!/*rtc::*/GetStringFromJsonObject(*message, kSessionDescriptionSdpName_Super, &sdp))
	{
		LOG(WARNING) << "Can't parse received session description message.";
		return;
	}
	webrtc::SdpParseError error;
	Json::StyledWriter writer;
	webrtc::SessionDescriptionInterface* session_description(webrtc::CreateSessionDescription(type, sdp, &error));
	if (!session_description)
	{
		LOG(WARNING) << "Can't parse received session description message. " << "SdpParseError was: " << error.description;
		return;
	}
	LOG(INFO) << " Received session description :" << *message;
	m_peer_connection_->SetRemoteDescription(DummySetSessionDescriptionObserverSuper::Create(), session_description);
	if (session_description->type() == webrtc::SessionDescriptionInterface::kOffer)
	{
		m_peer_connection_->CreateAnswer(this, NULL);
	}
}

void SessionConnectionObserver::DealOppositeIceCandidate(const void* message1)
{
	const Json::Value * message = static_cast<const Json::Value *>(message1);
	if (!message)
		return;
	std::string sdp_mid;
	int sdp_mlineindex = 0;
	std::string sdp;
	bool kCandidateSdpMidName_Super_bool = /*rtc::*/GetStringFromJsonObject(*message, kCandidateSdpMidName_Super, &sdp_mid);
	bool kCandidateSdpMlineIndexName_Super_bool = /*rtc::*/GetIntFromJsonObject(*message, kCandidateSdpMlineIndexName_Super,
		&sdp_mlineindex);
	bool kCandidateSdpName_Super_bool =/* rtc::*/GetStringFromJsonObject(*message, kCandidateSdpName_Super, &sdp);
	if (!kCandidateSdpMidName_Super_bool || !kCandidateSdpMlineIndexName_Super_bool ||
		!kCandidateSdpName_Super_bool)
	{
		LOG(WARNING) << "Can't parse received message.";
		return;
	}
	webrtc::SdpParseError error;
	rtc::scoped_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));
	if (!candidate.get())
	{
		LOG(WARNING) << "Can't parse received candidate message. " << "SdpParseError was: " << error.description;
		return;
	}
	if (!m_peer_connection_->AddIceCandidate(candidate.get()))
	{
		LOG(WARNING) << "Failed to apply the received candidate";
		return;
	}

	LOG(INFO) << " Received candidate :" << *message;
}

void SessionConnectionObserver::SetOppositeSDPInformation(const std::string & sdp)
{
	if (!m_peer_connection_.get())
	{
		if (!InitializePeerConnection())
		{
			LOG(LS_ERROR) << "WebrtcConductorDirect Failed to initialize our PeerConnection instance";
			return;
		}
	}

	Json::Reader reader;
	Json::Value jmessage;
	if (!reader.parse(sdp, jmessage))
	{
		LOG(WARNING) << "Received unknown message. " << sdp;
		return;
	}
	std::string type;
	std::string sdp_content;

	/*rtc::*/GetStringFromJsonObject(jmessage, kSessionDescriptionTypeName_Super, &type);
	if (type == "offer-loopback")
	{
		// This is a loopback call.
		// Recreate the peerconnection with DTLS disabled.
		if (!ReinitializePeerConnectionForLoopback())
		{
			LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
			DeletePeerConnection();
		}
		return;
	}

	if (!/*rtc::*/GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName_Super, &sdp_content))
	{
		LOG(WARNING) << "Can't parse received session description message.";
		return;
	}
	if (!m_oppositeSDPInfo)
		return;
	m_oppositeSDPInfo->SetString(sdp_content);
	//Create Answer sdp , then set the Answer sdp
	webrtc::SdpParseError error;
	webrtc::SessionDescriptionInterface* session_description(webrtc::CreateSessionDescription(type, m_oppositeSDPInfo->GetString(), &error));
	if (!session_description)
	{
		LOG(WARNING) << "Can't parse received session description message. " << "SdpParseError was: " << error.description;
		return;
	}
	m_peer_connection_->SetRemoteDescription(DummySetSessionDescriptionObserverSuper::Create(), session_description);
	if (session_description->type() == webrtc::SessionDescriptionInterface::kOffer)
	{
		m_peer_connection_->CreateAnswer(this, NULL);
	}
}

void  SessionConnectionObserver::SetOppositeIceCandidate(const std::vector<std::string >&icecandidate)
{
	if (!m_peer_connection_.get())
	{
		if (!InitializePeerConnection())
		{
			LOG(LS_ERROR) << "WebrtcConductorDirect Failed to initialize our PeerConnection instance";
			return;
		}
	}
	std::vector<std::string> * vector = reinterpret_cast<std::vector<std::string>*>(m_oppositeiceCandidateInfo->GetInfoVector());
	if (vector)
		vector->clear();
	(*vector) = icecandidate;
	/////////set the opposite candidates to the remoteIceCandidate
	if (!vector)
		return;
	for (size_t i = 0;i < vector->size();i++)
	{
		std::string candidate = (*vector)[i];
		Json::Reader reader;
		Json::Value jmessage;
		if (!reader.parse(candidate, jmessage))
		{
			LOG(WARNING) << "Received unknown message. " << candidate;
			return;
		}
		DealOppositeIceCandidate(&jmessage);
	}
}

const std::string&  SessionConnectionObserver::GetOwerSDPInformation()
{
	if (m_owerSDPInfo)
		return m_owerSDPInfo->GetString();
	else
		return emptyGoalbalString;
}

const std::vector<std::string> * SessionConnectionObserver::GetOwerIceCandidate()
{
	if (m_owericeCandidateInfo)
		return reinterpret_cast<std::vector<std::string>*>(m_owericeCandidateInfo->GetInfoVector());
	return NULL;
}

std::string & SessionConnectionObserver::SignalServerConnectionFailure()
{
	if (!m_signalServerVector)
		return const_cast<std::string &>(emptyGoalbalString);
	std::vector<ServerInfo>* serverInfoVector = reinterpret_cast<std::vector<ServerInfo>*>(m_signalServerVector->GetInfoVector());
	if (!serverInfoVector)//the turnserverInfoVector is NULL, so quit the program directly
		return const_cast<std::string &>(emptyGoalbalString);
	return (*serverInfoVector)[0].GetAddress();
}

void SessionConnectionObserver::CreateOffer()
{
	if (SessionConnectionIsValid())
		m_peer_connection_->CreateOffer(this, NULL);
}

bool SessionConnectionObserver::DataStreamIsEmpty()
{
	if (m_active_streams_ && m_active_streams_->GetInfoVector())
	{
		std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>>* streamMap = \
			reinterpret_cast<std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> >*>(m_active_streams_->GetInfoVector());
		return streamMap->empty();
	}
	return false;
}


void* SessionConnectionObserver::OpenVideoCaptureDevice()
{
	std::vector<std::string> device_names;
	{
		std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
			webrtc::VideoCaptureFactory::CreateDeviceInfo(0));
		if (!info)
		{
			return NULL;
		}
		int num_devices = info->NumberOfDevices();
		for (int i = 0; i < num_devices; ++i)
		{
			const uint32_t kSize = 256;
			char name[kSize] = { 0 };
			char id[kSize] = { 0 };
			if (info->GetDeviceName(i, name, kSize, id, kSize) != -1)
			{
				device_names.push_back(name);
			}
		}
	}

	cricket::WebRtcVideoDeviceCapturerFactory factory;
	cricket::VideoCapturer* capturer = nullptr;
	for (const auto& name : device_names)
	{
		capturer = factory.Create(cricket::Device(name, 0));
		if (capturer)
		{
			break;
		}
	}
	return capturer;
}

void SessionConnectionObserver::StartLocalRenderer(void* local_video)
{

}

void SessionConnectionObserver::StopLocalRenderer()
{

}

void SessionConnectionObserver::StartRemoteRenderer(void* remote_video)
{

}

void SessionConnectionObserver::StopRemoteRenderer()
{

}

bool SessionConnectionObserver::IceCandidateInfoValid()
{
	if (m_turnserverVector)
	{
		m_turnserverVector->GetInfoVector();
	}
	return false;
}

int SessionConnectionObserver::AddRef() /*const*/
{
	return 1;
}
int SessionConnectionObserver::Release() /*const*/
{
	return 1;
}

//void SessionConnectionObserver::OnMessage(rtc::Message* msg)
//{
	/*int retransmissionTimes = Re_Send_Times_WhenFailure;
	bool sendResults = false;
	std::queue<webrtc::DataBuffer> * queue = reinterpret_cast<std::queue<webrtc::DataBuffer>*>(m_sendTemporaryQueue->GetInfoVector());
	if (!queue)
		return;
	while (1)
	{
		if (!m_messageChannel)
			break;
		double percent_send_queue = m_messageChannel->SendQueueSurplus();
		if (percent_send_queue < 1)
		{
			sendResults = m_messageChannel->Send(queue->front());
			if (sendResults)
			{
				queue->pop();
				break;
			}
		}
		retransmissionTimes--;
		Sleep(1000);
	}*/
//}
///////////
ConductorAssemblyInit::~ConductorAssemblyInit()
{

}
///////////
ConductorExchangePosition::~ConductorExchangePosition()
{

}
//////////
ConductorFileOperatorBase::~ConductorFileOperatorBase()
{

}
////////////
ConductorBusiness::~ConductorBusiness()
{

}
