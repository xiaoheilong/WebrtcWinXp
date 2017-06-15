#include "webrtc/mediastreaminterface.h"

#include "talk/media/base/mediachannel.h"
#include "talk/media/base/videocommon.h"
#include "talk/media/base/videoframe.h"
#include "talk/media/base/videorenderer.h"
#include "webrtc/base/win32.h"

#include "webrtc/datachannelinterface.h"
#include"WebrtcClientOutterInterface.h"
#include "WebrtcConductor.h"
#include "TransferData.h"
#include "defaults.h"
#include<iostream>
///
namespace WebrtcClientOutterInterfaceNSP
{
	MessageDeliverPacket::MessageDeliverPacket(void *d, int s)
	{
		data = d;
		datasize = s;
	}

	MessageDeliverPacket::~MessageDeliverPacket()
	{
		data = NULL;
		datasize = 0;
	}
}

namespace TransferNamespace
{
	class BusinessBaseVector :public ContainerInfoShaowBase
	{
	public:
		explicit BusinessBaseVector();
		virtual ~BusinessBaseVector();
	public:
		virtual void * GetInfoVector();
	protected:
		std::vector<BusinessBase *> m_vector;
	};

	BusinessBaseVector::BusinessBaseVector()
	{
		m_vector.clear();
	}

	BusinessBaseVector::~BusinessBaseVector()
	{
		WebrtcClientOutterInterfaceNSP::DeleteContainerPointer<std::vector<BusinessBase *>>(m_vector);
	}

	void* BusinessBaseVector::GetInfoVector()
	{
		return &m_vector;
	}

	ContainerInfoShaowBase * CreateBusinessVector()
	{
		return new BusinessBaseVector();
	}

	void DeleteBusinessVector(ContainerInfoShaowBase * value)
	{
		DeletePtr(value);
	}
}

#define CreateSTLString TransferNamespace::CreateSTLString 
#define CreateSTLStringEx TransferNamespace::CreateSTLStringEx
#define DeleteSTLString TransferNamespace::DeleteSTLString
////some typdef 
typedef WebrtcClientOutterInterfaceNSP::MessageDeliverPacket MessageDeliverPacket;
////////////////
WebrtcClientOutterIterator::~WebrtcClientOutterIterator()
{

}
///////////////
class WEBRTCP2PDLLSHARED_EXPORT VideoRenderer : public webrtc::VideoRendererInterface/*rtc::VideoSinkInterface<cricket::VideoFrame>*/
{
public:
	VideoRenderer(ClientOutterInterfaceOriginal *parent, int width, int height, webrtc::VideoTrackInterface* track_to_render);
	virtual ~VideoRenderer();

	void Lock();

	void Unlock();

	// VideoSinkInterface implementation
	//void OnFrame(const cricket::VideoFrame& frame) override;
	virtual void RenderFrame(const cricket::VideoFrame* frame);

	const uint8_t* bmi() const;
	const uint8_t* image() const;
protected:
	void SetSize(int width, int height);
	enum
	{
		SET_SIZE,
		RENDER_FRAME,
	};

	ClientOutterInterfaceOriginal *m_parent;//outer handle that will show this image data in they device
	rtc::scoped_ptr<uint8_t[]> image_;
	CRITICAL_SECTION buffer_lock_;
	rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
};
////////////////
VideoRenderer::VideoRenderer(ClientOutterInterfaceOriginal *parent, int width, int height,webrtc::VideoTrackInterface* track_to_render)
: m_parent(parent), rendered_track_(track_to_render) 
{
	::InitializeCriticalSection(&buffer_lock_);

	//rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
	rendered_track_->AddRenderer(this);
}

VideoRenderer::~VideoRenderer() 
{
	//rendered_track_->RemoveSink(this);
	rendered_track_->RemoveRenderer(this);
	::DeleteCriticalSection(&buffer_lock_);
}

void VideoRenderer::Lock()
{
	::EnterCriticalSection(&buffer_lock_);
}

void VideoRenderer::Unlock()
{
	::LeaveCriticalSection(&buffer_lock_);
}

const uint8_t* VideoRenderer::bmi() const
{ 
	return  NULL/*bmi_*/;
}

const uint8_t* VideoRenderer::image() const
{
	return image_.get();
}

void VideoRenderer::SetSize(int width, int height)
{
	
}

//void VideoRenderer::OnFrame(const cricket::VideoFrame& video_frame) 
//{
//	AutoLock<VideoRenderer> lock(this);
//
//	const cricket::VideoFrame* frame =
//		video_frame.GetCopyWithRotationApplied();
//
//	SetSize(frame->width(), frame->height());
//
//	assert(image_.get() != NULL);
//	/*if (m_parent)
//		m_parent->ImageDataInvalidate();*/
//	//this place is parent how to use those frame image
//}

void VideoRenderer::RenderFrame(const cricket::VideoFrame* frame)
{
	AutoLock<VideoRenderer> lock(this);
	if (!frame)
		return;
	ASSERT(image_.get() != NULL);
	/*frame->ConvertToRgbBuffer(cricket::FOURCC_ARGB,
		image_.get(),
		bmi_.bmiHeader.biSizeImage,
		bmi_.bmiHeader.biWidth *
		bmi_.bmiHeader.biBitCount / 8);*/

	assert(image_.get() != NULL);
	/*if (m_parent)
	m_parent->ImageDataInvalidate();*/
	//this place is parent how to use those frame image
}
///////////////////
class  DataTransferBase :public TransferDataBase, public webrtc::DataChannelObserver
{
public:
	DataTransferBase(ClientOutterInterfaceOriginal * parent);
	DataTransferBase();
	~DataTransferBase();
public:
	virtual void PrintOutReceiveMessage();
	virtual void BindParent(ClientOutterInterfaceOriginal  *wnd);//绑定一个窗口句柄
	virtual webrtc::DataChannelObserver* GetMessageObserver();
protected:
	virtual void OnStateChange()override;
	// The data channel's buffered_amount has changed.
	//virtual void OnBufferedAmountChange(uint64_t previous_amount)override;
	//the data recieve event, It's autoly, but need bind to a webrtc connect
	virtual void OnMessage(const webrtc::DataBuffer &message) override;
private:
	ClientOutterInterfaceOriginal *m_parent;//所在窗口的句柄
	char m_receiveMessage[500];
	size_t m_state_change_count;
};

WEBRTCP2PDLLSHARED_EXPORT TransferDataBase * CreateTransferDataBase()
{
	return new DataTransferBase();
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseTransferDataBase(TransferDataBase * value)
{
	DeletePtr(value);
}
///////////////////////
DataTransferBase::DataTransferBase(ClientOutterInterfaceOriginal * parent):m_parent(parent)
{
	memset(&m_receiveMessage, 0, sizeof(m_receiveMessage));
	m_state_change_count = 0;
}

DataTransferBase::DataTransferBase() :m_parent(NULL)
{
	memset(&m_receiveMessage, 0, sizeof(m_receiveMessage));
	m_state_change_count = 0;
}

DataTransferBase::~DataTransferBase()
{
	memset(&m_receiveMessage, 0, sizeof(m_receiveMessage));
	m_state_change_count = 0;
	//m_parent = NULL;
}


void DataTransferBase::PrintOutReceiveMessage()
{
	//printf("you receive message:%s", m_receiveMessage);
}

void DataTransferBase::BindParent(ClientOutterInterfaceOriginal  *wnd)
{
	m_parent = wnd;
}

webrtc::DataChannelObserver* DataTransferBase::GetMessageObserver()
{
	return this;
}

void DataTransferBase::OnStateChange()
{
	m_state_change_count++;
}

//void DataTransferBase::OnBufferedAmountChange(uint64_t previous_amount)
//{
//
//}

void DataTransferBase::OnMessage(const webrtc::DataBuffer &message)
{
	if (m_parent)
	{
		const char * messageStr = message.data.data();
		if (!messageStr)
			return;
		m_parent->RecieveDataChannel((uint8_t * )messageStr, message.data.length());
	}
}

////////////////
ClientOutterInterfaceOriginal::~ClientOutterInterfaceOriginal()
{

}
///////////////////
ClientOutterInterfaceBase::ClientOutterInterfaceBase()
{
	m_businessVector = CreateContainer(TransferNamespace::BusinessBaseVector);
	std::vector<BusinessBase *> * vector = reinterpret_cast<std::vector<BusinessBase *>*>(m_businessVector->GetInfoVector());
	if (vector)
		WebrtcClientOutterInterfaceNSP::DeleteContainerPointer<std::vector<BusinessBase *>>(*vector);
	m_callback = NULL;
}

ClientOutterInterfaceBase::~ClientOutterInterfaceBase()
{		
	DeleteContainer(m_businessVector);
	m_callback = NULL;
}

void ClientOutterInterfaceBase::RegisterObserver(void * callback)
{
	m_callback = callback;
}

void * ClientOutterInterfaceBase::GetCallback()const
{
	return m_callback;
}


void ClientOutterInterfaceBase::AddBusiness(BusinessBase * business)
{
	std::vector<BusinessBase *> * vector = reinterpret_cast<std::vector<BusinessBase *>*>(m_businessVector->GetInfoVector());
	if (vector)
		vector->push_back(business);
}

void  ClientOutterInterfaceBase::DeliverMessage(int messageid, void *lparam, void * rparm)
{
	std::vector<BusinessBase *> * vector = reinterpret_cast<std::vector<BusinessBase *>*>(m_businessVector->GetInfoVector());
	if (vector)
	{
		if (vector->empty())
			return;
		std::vector<BusinessBase *>::iterator beptr = vector->begin();
		while (beptr != vector->end())
		{
			if (*beptr)
			{
				(*beptr)->OnMessage(messageid, lparam, rparm);
			}
			beptr++;
		}
	}
}

void ClientOutterInterfaceBase::DeleteBusiness(BusinessBase * business)
{
	std::vector<BusinessBase *> * vector = reinterpret_cast<std::vector<BusinessBase *>*>(m_businessVector->GetInfoVector());
	if (vector)
	{
		std::vector<BusinessBase *>::iterator beptr = std::find_if(vector->begin(), vector->end(),
			[&](BusinessBase * business1)->bool \
		{
			if (business == business)
				return true;
			else
				return false;
		});
	}

}

bool ClientOutterInterfaceBase::ConnectPeerIsValid()const
{
	if (m_callback)
	{
		WebrtcConductorDecorator * conductor = static_cast<WebrtcConductorDecorator *>(m_callback);
		return conductor && conductor->Connection_Active();
	}
	return false;
}

void ClientOutterInterfaceBase::SendMessageEx(int msgid, void *data)
{
	if (m_callback)
	{
		WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
		if (ptr)
		{
			ptr->CallbackWebrtcClient(msgid, data);
		}
	}
}

bool ClientOutterInterfaceBase::ConnectServerIsValid()const
{
	if (m_callback)
	{
		WebrtcConductorDecorator * conductor = static_cast<WebrtcConductorDecorator *>(m_callback);
		return conductor && conductor->LoginInSuccese();
	}
	return false;
}

bool ClientOutterInterfaceBase::SendTextMessage(void *param, int paramsize)//send to opposite client a text param,
{//the param is the content of the text messag , paramsize is it's size
	MessageDeliverPacket data(param, paramsize);
	FileDataPack * ptr = static_cast<FileDataPack *>(param);
	if (m_callback)
	{
		SendMessageEx(SEND_DATA_CHANNEL, &data);//disconnect from signalserver
		return true;
	}
	return false;
}

void ClientOutterInterfaceBase::AddTurnServerInfo(const TurnserverInfo &info)
{
	if (m_callback)
	{
		WebrtcConductorDecorator * conductor = static_cast<WebrtcConductorDecorator *>(m_callback);
        if(conductor != NULL)
            conductor->AddTurnServerInfo(info);
	}
}

void ClientOutterInterfaceBase::AddSinalServerInfo(const ServerInfo &info)
{
	if (m_callback)
	{
		WebrtcConductorDecorator * conductor = static_cast<WebrtcConductorDecorator *>(m_callback);
        if(conductor != NULL)
            conductor->AddSinalServerInfo(info);
	}
}

bool ClientOutterInterfaceBase::SendFile(void* param)//send to opposite a file, the param is the file's path
{
	if (m_callback)
	{
		SendMessageEx(SEND_FILE_CHANNEL, param);
		return true;
	}
	return false;
}

bool ClientOutterInterfaceBase::CreateBarcodeFile(void * res)
{
	if (m_callback)
	{
		SendMessageEx(CREATE_FILE_REPOSITORY, res);
		return true;
	}
	return false;
}

bool ClientOutterInterfaceBase::DeleteBarcodeFile(void * guid)
{
	if (m_callback)
	{
		SendMessageEx(DELETE_FILE_REPOSITORY, guid);
		return true;
	}
	return false;
}

void ClientOutterInterfaceBase::RecieveDataChannel(uint8_t * const message, int size)
{
	PacketType type;
	memcpy(&type, message, sizeof(PacketType));
	if (type == PacketType::FILEDATABASE)
	{
		FileDataBase  tempMsg;//
		tempMsg.Deserialize(message,size);
		switch (tempMsg.message_id)
		{
		case Text_Type:
		{
			RecieveTextMessage(message, size);
		}
		break;
		case File_Type:
		{
			FileDataPack * msg = new FileDataPack();
			msg->Deserialize(message, size);
			if (!msg->data)
				break;
			switch (msg->data->messageType)
			{
			case Create_File:
			{
				//set the savefilepath to the msg .deliver the param to the UIThreadCallback
				SendMessageEx(static_cast<int>(CREATE_FILE_CHANNEL), reinterpret_cast<void*>(msg));
			}
			break;
			case Send_File:
			{
				SendMessageEx(static_cast<int>(RECIEVE_FILE_CHANNEL), reinterpret_cast<void*>(msg));
			}
			break;
			case Can_Continue_Send:
			{
				SendMessageEx(static_cast<int>(CAN_SEND_FILE), reinterpret_cast<void*>(msg));
			}
			break;
			/*case Not_Can_Continue_Send:
			{
				SendMessageEx(static_cast<int>(NOT_CAN_SEND_FILE), reinterpret_cast<void*>(msg));
			}
			break;*/
			case Failure_Recieve_File:
			{
				LOG(INFO) << "Failure_Recieve_File";
				SendMessageEx(static_cast<int>(SEND_FILE_FAILURE), reinterpret_cast<void*>(msg));
			}
			break;
			case Succese_Recieve_File:
			{
				LOG(INFO) << "Succese_Recieve_File";
				SendMessageEx(static_cast<int>(SEND_FILE_SUCCESS), reinterpret_cast<void*>(msg));
			}
			break;
			case Interrupt_File_Transfer:
			{
				SendMessageEx(static_cast<int>(INTERRUPT_FILE_TRANSFER_TASK), reinterpret_cast<void*>(msg));
			}
			break;
			case Restart_File_Transfer:
			{
				SendMessageEx(static_cast<int>(RESTART_FILE_TRANSFER_TASK), reinterpret_cast<void*>(msg));
			}
			break;
			case Cancle_File_Transfer:
			{
				SendMessageEx(static_cast<int>(CANCLE_FILE_TRANSFER_TASK), reinterpret_cast<void*>(msg));
			}
			break;
			case End_Send_File:
			{
				SendMessageEx(static_cast<int>(END_SEND_FILE), reinterpret_cast<void*>(msg));
			}
			break;
			default:
				break;
			}
			DeletePtr(msg);
		}
		break;
		case File_Type_Shake_Pack://check the replay of the file reciever recieve the file packet ,so deal with the packet to occur the event
		{
			SendMessageEx(static_cast<int>(SHAKE_ACK_PACKET), reinterpret_cast<void *>(&tempMsg));
		}
		break;
		default:
		{

		}
		break;
		}
	}
}

bool ClientOutterInterfaceBase::OnMessage(int messageid, void * lparam, void *rparam)
{
	switch (messageid)
	{
	case ON_LOGIN_SUCCESE://get the message from webrtc client,and excute self deal function
	{
		LoginInSuccese(lparam);
	}
	break;
	case ON_LOGIN_FAILURE:
	{
		LoginInSuccese(lparam);
	}
	break;
	case ON_LOGOUT_SUCCESE:
	{
		LoginOutSuccese(lparam);
	}
	break;
	case ON_LOGOUT_FAILURE:
	{
		LoginOutFialure(lparam);
	}
	break;
	case ON_CONNECT_PEER_SUCCESE:
	{
		ConnectToPeerSuccese(lparam);
	}
	break;
	case ON_CONNECT_PEER_FAILURE:
	{
		ConnectToPeerFailure(lparam);
	}
	break;
	case ON_PEER_CONNECT_TO_SERVER:
	{
		PeerConnectToServer(lparam);
	}
	break;
	case ON_PEER_DISCONNECT_TO_SERVER:
	{
		PeerDisConnectToServer(lparam);
	}
	break;
	case ON_NEED_REBUIDCONNECT_TO_TURNSERVER:
	{
		//PeerNeedRebuildTurnserverConnection(NULL);
	}
	break;
	//case ON_OPPOSITE_CLIENT_CANCLE_FILE:
	//{
	//	OnFileCancle(lparam);
	//}
	break;
	default:
	{
		//not define message deal ,then deliver the message to m_businessVector
		DeliverMessage(messageid, lparam, rparam);
	}
	break;
	}
	return false;
}
////////////////
WEBRTCP2PDLLSHARED_EXPORT ClientOutterInterfaceOriginal* CreateClientOutterInterfaceOriginal(ClientOutterInterfaceOriginal*value)
{
	return value;
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseClientOutterInterfaceOriginal(ClientOutterInterfaceOriginal * value)
{
	DeletePtr(value);
}

WEBRTCP2PDLLSHARED_EXPORT void AddTurnServerInfo(ClientOutterInterfaceOriginal * inter , const TurnserverInfo &server)
{
	if (inter)
		inter->AddTurnServerInfo(server);
}

WEBRTCP2PDLLSHARED_EXPORT bool ConnectServerIsValid(ClientOutterInterfaceOriginal *value)
{
	if (value)
		return value->ConnectServerIsValid();
	return false;
}

WEBRTCP2PDLLSHARED_EXPORT bool ConnectPeerIsValid(ClientOutterInterfaceOriginal * value)
{
	if (value)
		return value->ConnectPeerIsValid();
	return false;
}

/////////////
PeerInfo::PeerInfo()
{
	id = -1;
	username = CreateSTLString();
}

PeerInfo::PeerInfo(int i, const std::string & name)
{
	id = i;
	username = CreateSTLStringEx(name);
}

bool PeerInfo:: operator<(const PeerInfo& peer)const
{
	return id < peer.id;
}

void  PeerInfo:: operator()(const PeerInfo& peer)
{
	id = peer.id;
	if (username && peer.username)
		username->SetString(peer.username->GetString());
	else
		username->SetString("");
}
///////////
WEBRTCP2PDLLSHARED_EXPORT PeerInfo * CreatePeerInfo(const LRESULT &id)
{
	return new PeerInfo(id);
}
WEBRTCP2PDLLSHARED_EXPORT void ReleasePeerInfo(PeerInfo * value)
{
	DeletePtr(value);
}
