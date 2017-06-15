#ifndef WEBRTC_CLIENT_OUTTER_INTERFACE_HEADER
#define WEBRTC_CLIENT_OUTTER_INTERFACE_HEADER
#pragma once
#include <map>
#include <string>
#include<algorithm>

#include "TransferData.h"
#include "defaults.h"
#include "DllInterface.h"
///////////
namespace WebrtcClientOutterInterfaceNSP
{
	//the function is to delete the resource of container that include pointer struct
	//but the type T must be Sequence container, or it's can't work
	template<typename T>
	void DeleteContainerPointer(T  &container)
	{
        typename T::iterator beptr = container.begin();
		while (beptr != container.end())
		{
			if (*beptr)
			{
				DeletePtr(*beptr);
				*beptr = NULL;
			}
			beptr++;
		}
		container.clear();
	}

    struct WEBRTCP2PDLLSHARED_EXPORT MessageDeliverPacket//Because WebrtcClientOutterInterface deliver function param is by void *
	{//so , we should record the data's size, It use for socket send 
		MessageDeliverPacket(void *d, int s);
		~MessageDeliverPacket();
		void* data;
		int datasize;
	};
}

// A little helper class to make sure we always to proper locking and
// unlocking when working with VideoRenderer buffers.
template <typename T>
class AutoLock
{
public:
	explicit AutoLock(T* obj) : obj_(obj) { obj_->Lock(); }
	~AutoLock() { obj_->Unlock(); }
protected:
	T* obj_;
};
EXTERN_C_BEGIN
namespace TransferNamespace
{
	class STLStringShaow;
}
typedef /*typename*/ TransferNamespace::ContainerInfoShaowBase  ContainerInfoShaowBase;
/////////////declare some nesacery struct
class VideoRenderer;
namespace cricket
{
	class VideoFrame;
}
//this client class which apply function that connect signal server to webrtcClient
class WEBRTCP2PDLLSHARED_EXPORT WebrtcClientOutterIterator //this class is inhrit by WebrtcClient, Outter will use these interface to call webrtc fuction
{
public:
	virtual ~WebrtcClientOutterIterator();
public:
	virtual void CallbackWebrtcClient(int  msg_id,void* data) = 0;//webrtcClient deal the message which message index is msg_id, the message's pointer is data
	virtual void AddTurnServerInfo(const TurnserverInfo &info) = 0;
	virtual void AddSinalServerInfo(const ServerInfo &info) = 0;
};
////this is audio，hasn't define the AudioRender class 
class WEBRTCP2PDLLSHARED_EXPORT AudioRender
{

};

///////////////////////incadite state of the ClientOutterInterfaceOriginal class that use webrtc ,
enum ClientMessageID
{
	ON_LOGIN_SUCCESE = /*WM_USER +*/ 1,   //login in succese message id
	ON_LOGIN_FAILURE,					  //login in failure message id
	ON_LOGOUT_SUCCESE,					  //login out succese message id
	ON_LOGOUT_FAILURE,					  // LOGIN out failure message id
	ON_CONNECT_PEER_SUCCESE,			  // connect opposite client  succese message id
	ON_CONNECT_PEER_FAILURE,			  //connect  opposite client  failute message id
	ON_SEND_FILE_SUCCESE,				  //send opposite client some file succese message id
	ON_SEND_FILE_FAILURE,				  //send opposite client some file failure message id
	ON_OPPOSITE_CLIENT_CANCLE_FILE,		  //opposite client cancle current tasnfer
	ON_SEND_AUDIO_SUCCESE,				  //send opposite clent some audio data succese message id
	ON_SEND_AUDIO_FAILURE,				  // send opposite client some audio data failure message id
	ON_SEND_VIDEO_SUCCESE,				  // send opposite client some video data succese message id
	ON_SEND_VIDEO_FAILURE,				  //send opposite client some video data failure message id
	ON_SEND_TEXTMSG_SUCCESE,			  //send opposite client some message data succese
	ON_SEND_TEXTMSG_FAILURE,			  //send opposite client some message data failure
	ON_PEER_CONNECT_TO_SERVER,			  //some peer re land from server ,server will inform current client
	ON_PEER_DISCONNECT_TO_SERVER,		  //some peer re exit from server ,server will inform current client	
	ON_NEED_REBUIDCONNECT_TO_TURNSERVER,
};

enum FileTransferState
{
	FILE_TRANSFER_SUCESS = 0,
	FILE_TRANSFER_FAILURE,
	FILE_TRANSFER_CANCLE,
};

enum ConnectState
{
	Connect_Zero = 0,
	Connect_Success ,
	Connect_Failure,
	Disconnect,
	RepeatState,
};

enum DisConnectType
{
	TurnServer,
	PeerClient,
};

/////business base class
class WEBRTCP2PDLLSHARED_EXPORT BusinessBase
{
public:
	virtual ~BusinessBase() {};
public:
	//this class is be for get the business from business responsity,convinent to  dynamic deal all type message
	virtual bool OnMessage(int messageid, void * lparam = NULL, void *rparam = NULL) = 0;
};
////////////////////
struct PeerInfo;
class OutterInterfaceBase;
class WEBRTCP2PDLLSHARED_EXPORT ClientOutterInterfaceOriginal
{
public:
	virtual ~ClientOutterInterfaceOriginal();
public:
	virtual void RegisterObserver(void * callback) = 0;
	virtual void * GetCallback()const = 0;
	//SendMessage is deliver the message to webrtc model, inform webrtc class which inhrit WebrtcClientOutterIterator
	virtual void SendMessageEx(int msgid, void *data) = 0;
	virtual void RecieveTextMessage(uint8_t * const message, int size) = 0;
	virtual void RecieveDataChannel(uint8_t * const message, int size) = 0;
	virtual void RecieveVideoChannel(const cricket::VideoFrame& frame) = 0;
	virtual void RecieveAudioChannel(const void * data) = 0;//unclear the Audio's Original interface,to be extent
	////////////////////////////////////////								
	virtual bool OnMessage(int messageid, void * lparam = NULL, void *rparam = NULL) = 0;
	virtual void AddBusiness(BusinessBase * business) = 0;

	virtual void DeliverMessage(int messageid, void *lparam, void * rparm) = 0;

	virtual void DeleteBusiness(BusinessBase * business) = 0;
	///////////
	virtual bool ConnectServerIsValid()const = 0;

	virtual bool ConnectPeerIsValid()const = 0;
	/////////////////send  series function function
	virtual bool SendFile(void* param) = 0;//send to opposite a file, the param is the file's path

	virtual bool CreateBarcodeFile(void * res) = 0;
	virtual bool DeleteBarcodeFile(void * guid) = 0;
	virtual bool SendTextMessage(void *param, int paramsize) = 0; //send to opposite client a text param,
	////////////////////////////////////															//////////////the series interface is to deliver the connect state which with sinalserver and opposite client
	virtual bool LoginIn(const std::string &address, const int &port) = 0;//these action is relative sinalserver
	virtual bool LoginInSuccese(void * prama) = 0;
	virtual bool LoginInFailure(void * prama) = 0;

	virtual bool LoginOut() = 0;
	virtual bool LoginOutSuccese(void *param) = 0;
	virtual bool LoginOutFialure(void * param) = 0;

	virtual bool ConnectToPeer(const PeerInfo& peerinfo) = 0;
	virtual bool ConnectToPeerSuccese(void * prama) = 0;
	virtual bool ConnectToPeerFailure(void * prama) = 0;

	virtual bool DisConnectToPeer() = 0;

	virtual bool PeerConnectToServer(void * param) = 0;
	virtual bool PeerDisConnectToServer(void *param) = 0;
	virtual bool PeerNeedRebuildTurnserverConnection(const int & type, const int & state) = 0;
	/////////////////////////////////
	virtual bool SetOppositeSDPInfo(const std::string &sdp) = 0;
	virtual bool SetOppositeIceCandidate(const std::vector<std::string >&icecandidate) = 0;
	virtual const std::string&      GetOwerSDPInformation() = 0;
	virtual const std::vector<std::string> * GetOwerIceCandidate() = 0;
	///////////////////
	virtual void  AcceptFile(void * param) = 0;
	virtual void  RefuseFile(void * param) = 0;
	virtual bool  InterruptFileTask(void * param) = 0;
	virtual bool  CreateRecieveFile(void * param,void *param1) = 0;
	virtual bool  DeleteRecieveFile(void * param)  = 0;
	virtual bool  RestartFileTask(void * param) = 0;
	virtual bool  CancleFileTask(void * param)  = 0;
	//virtual bool  OnFileCancel(const char* param) = 0;
	virtual void  ErrorMessageBoxSlots(const char* text) = 0;
	virtual void  TransferProgressSlots(const void *resGuid, const void* progress) = 0;
	virtual void  TransferFileComplete(const char * resGuid,  const FileTransferState& state) = 0;
	virtual void  AddTurnServerInfo(const TurnserverInfo &info) = 0;
	virtual void  AddSinalServerInfo(const ServerInfo &info) = 0;
	virtual void  AttachedWnd(OutterInterfaceBase *wnd) = 0;
	////////media call interface
	virtual void StartLocalRenderer(void* local_video) = 0;
	virtual void StopLocalRenderer() = 0;
	virtual void StartRemoteRenderer(void* remote_video) = 0;
	virtual void StopRemoteRenderer() = 0;
};
/////////////////////////////
//It's the base class which apply some interface to convenient to client to call webrtc model
struct PeerInfo;
class WEBRTCP2PDLLSHARED_EXPORT ClientOutterInterfaceBase:public ClientOutterInterfaceOriginal
{
public:
	ClientOutterInterfaceBase();
protected:
	virtual ~ClientOutterInterfaceBase();
public:
	//for not define this class to a virtual base class , the callback is the void pointer
	//you should convert it to you want class type when you use it
	virtual void RegisterObserver(void * callback)final;

	virtual void * GetCallback()const final;
	//SendMessage is deliver the message to webrtc model, inform webrtc class which inhrit WebrtcClientOutterIterator
	virtual void SendMessageEx(int msgid, void *data)final;
	virtual void RecieveDataChannel(uint8_t * const message, int size) final;
	////////response the webrtc client's deal message pump ,return value is bool type ,It's incadite the function's isn't succese
	virtual bool OnMessage(int messageid, void * lparam = NULL, void *rparam = NULL) final;//now just support two void pointer to deliver the param
	////the function is use for dynamic add some new function model,just userful that model inhret BusinessBase class
	virtual void AddBusiness(BusinessBase * business)final;

	virtual void DeliverMessage(int messageid, void *lparam, void * rparm)final;

	virtual void DeleteBusiness(BusinessBase * business)final;
	///////////
	virtual bool ConnectServerIsValid()const final;

	virtual bool ConnectPeerIsValid()const final;
	/////////////////send  series function function
	virtual bool SendFile(void* param);//send to opposite a file, the param is the file's path

	virtual bool CreateBarcodeFile(void * res);
	virtual bool DeleteBarcodeFile(void * guid);
	virtual bool SendTextMessage(void *param, int paramsize) ; //send to opposite client a text param,
	virtual void AddTurnServerInfo(const TurnserverInfo &info) ;
	virtual void AddSinalServerInfo(const ServerInfo &info);
	virtual void AttachedWnd(OutterInterfaceBase *wnd) = 0;
protected:
	ContainerInfoShaowBase *m_businessVector;
	void * m_callback;
};

DllExtern_Dynamic_Interface(ClientOutterInterfaceOriginal)

EXTERN_C_BEGIN   \
WEBRTCP2PDLLSHARED_EXPORT void AddTurnServerInfo(ClientOutterInterfaceOriginal *, const TurnserverInfo&);

WEBRTCP2PDLLSHARED_EXPORT bool ConnectServerIsValid(ClientOutterInterfaceOriginal *);
WEBRTCP2PDLLSHARED_EXPORT bool ConnectPeerIsValid(ClientOutterInterfaceOriginal *);

EXTERN_C_END
///////declare the base class that maybe use the VideoRenderer or DataTransferBase
////////////these define some assist class
///////video transfer class
class WEBRTCP2PDLLSHARED_EXPORT VideoRenderBase
{
public:
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	//this function is use for trigger video frame deal
	virtual void OnFrame(const cricket::VideoFrame& frame) = 0;
	virtual const uint8_t* bmi() const = 0;
	virtual const uint8_t* image() const = 0;
protected:
	virtual void SetSize(int width, int height) = 0;
};
//////////////message transfer base class 
class DataTransferBase;
namespace webrtc
{
	class DataChannelObserver;
}

class WEBRTCP2PDLLSHARED_EXPORT TransferDataBase
{
public:
	virtual void PrintOutReceiveMessage() = 0;
	virtual void BindParent(ClientOutterInterfaceOriginal  *wnd) = 0;//绑定一个窗口句柄
	virtual webrtc::DataChannelObserver * GetMessageObserver() = 0;
};
DllExtern_Fixed_Interface(TransferDataBase)
///////////////////
struct WEBRTCP2PDLLSHARED_EXPORT PeerInfo
{
	int id;//peer's id
	STLStringShaow *username;//peer's name
	PeerInfo();

	PeerInfo(int i, const std::string & name = "");

	bool operator<(const PeerInfo& peer)const;

	void operator()(const PeerInfo& peer);
};

WEBRTCP2PDLLSHARED_EXPORT PeerInfo * CreatePeerInfo();
WEBRTCP2PDLLSHARED_EXPORT void ReleasePeerInfo(PeerInfo *);
EXTERN_C_END
/////////
#endif
