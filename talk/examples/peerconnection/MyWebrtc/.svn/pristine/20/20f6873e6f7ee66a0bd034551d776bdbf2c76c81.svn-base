#ifndef   WEBRTC_OUTTER_CLIENT_TEST
#define   WEBRTC_OUTTER_CLIENT_TEST
#pragma once
#include"WebrtcClientOutterInterface.h"
#include "webrtc\base\sigslot.h"
///////
namespace webrtc
{
	class VideoTrackInterface;
}

enum WindowMessages {
	UI_THREAD_CALLBACK = WM_APP + 1,
	UI_TASK_PROGRESS_CALLBACK,
};

enum UI {
	CONNECT_TO_SERVER,
	LIST_PEERS,
	STREAMING,
	DISCONNECT_TO_TURNSERVER
};


 class OutterInterfaceBase
{
public:
	////////media call interface
	virtual void StartLocalRenderer(void* local_video) = 0;
	virtual void StopLocalRenderer() = 0;
	virtual void StartRemoteRenderer(void* remote_video) = 0;
	virtual void StopRemoteRenderer() = 0;
	//////////////
	//virtual void TransferProgress(void* data) = 0;
	virtual void TransferProgress(const char* taskGuid, const double totalSize, const double finishSize) = 0;
	virtual void RecieveDataChannel(const wchar_t* message) = 0;
	virtual void OnConnectToPeer(const int &state) = 0;
	virtual void OnConnectTurnServer(const int & state) = 0;
	virtual void TransferFileComplete(const char * resGuid, const int& state) = 0;
	virtual void OnDisConnection(const int &type, const int & state) = 0;
	//virtual void OnFileCancel(const char * param) = 0 ;
};
EXTERN_C_BEGIN
////////////////

class ClientOutterInterfaceTest : virtual public ClientOutterInterfaceBase, public sigslot::has_slots<>
{
public:
	ClientOutterInterfaceTest();
	ClientOutterInterfaceTest(const std::string & serverAddress, const std::string & serverPort);
	~ClientOutterInterfaceTest();
public:
	/****************************inherit function********************************************************************/
	//////declare some action by self that client want to run
	virtual bool LoginIn(const std::string &address, const int &port);//these action is relative sinalserver
	virtual bool LoginInSuccese(void * prama);
	virtual bool LoginInFailure(void * prama);

	virtual bool LoginOut();
	virtual bool LoginOutSuccese(void *param);
	virtual bool LoginOutFialure(void * param);

	virtual bool ConnectToPeer(const PeerInfo & peerinfo);
	virtual bool ConnectToPeerSuccese(void * prama);
	virtual bool ConnectToPeerFailure(void * prama);

	virtual bool DisConnectToPeer();//this function hasn't param, the param is the m_currentConnectPeer

	virtual bool PeerConnectToServer(void * param);
	virtual bool PeerDisConnectToServer(void *param);
	virtual bool PeerNeedRebuildTurnserverConnection(const int & type, const int & state);
	//SendMessage is deliver the message to webrtc model, inform webrtc class which inhrit WebrtcClientOutterIterator
	virtual void RecieveTextMessage(uint8_t * const message, int size);
	virtual void RecieveVideoChannel(const cricket::VideoFrame& frame) override;
	virtual void RecieveAudioChannel(const void * data)override;

	virtual void AttachedWnd(OutterInterfaceBase *wnd);
	////////media call interface
	virtual void StartLocalRenderer(void* local_video);
	virtual void StopLocalRenderer() ;
	virtual void StartRemoteRenderer(void* remote_video);
	virtual void StopRemoteRenderer();


	virtual bool SetOppositeSDPInfo(const std::string &sdp);
	virtual bool SetOppositeIceCandidate(const std::vector<std::string >&icecandidate);
	virtual const std::string&   GetOwerSDPInformation();
	const std::vector<std::string> * GetOwerIceCandidate();

	virtual void  AcceptFile(void * param) ;
	virtual void  RefuseFile(void * param);
	virtual bool  InterruptFileTask(void * param);
	virtual bool  CreateRecieveFile(void * param,void* param1);
	virtual bool  DeleteRecieveFile(void * param);
	virtual bool  RestartFileTask(void * param);
	virtual bool  CancleFileTask(void * param);

	virtual void TransferProgressSlots(const void *resGuid, const void* progress);
	virtual void TransferFileComplete(const char * resGuid, const FileTransferState& state);
	virtual void ErrorMessageBoxSlots(const char* text)final;
	/////////////////////
	void MessageBox(const void * text);
	virtual void TransferProgress(const void *resGuid, const void *progress);

	virtual bool CreateBarcodeFile(void * res);
	virtual bool SendFile(void* param) ;
	virtual bool SendTextMessage(void *param, int paramsize);
	//virtual bool OnFileCancel(const char* param);
public:
	sigslot::signal1<const void *> m_messageBoxSlots;
	sigslot::signal2 < const void *, const void * >m_transferProgressSlots;
protected:
	std::string m_serverAddress;//want connect server's ip address
	std::string m_serverPort;//want connect server's port 
	std::string m_tempStr;
	std::vector<PeerInfo> m_connectPeerVector;
	OutterInterfaceBase*  m_outterWnd;
};



class  ClientOutterInterfaceSingle :public ClientOutterInterfaceTest
{
public:
	ClientOutterInterfaceSingle();
	ClientOutterInterfaceSingle(const std::string & serverAddress, const std::string & serverPort);
	virtual ~ClientOutterInterfaceSingle();
public:
	virtual bool ConnectToPeerSuccese(void * prama) override;
	virtual bool ConnectToPeerFailure(void * prama)override;
	////////media call interface
	virtual void StartLocalRenderer(void* local_video);
	virtual void StopLocalRenderer();
	virtual void StartRemoteRenderer(void* remote_video);
	virtual void StopRemoteRenderer();
};
EXTERN_C_BEGIN
WEBRTCP2PDLLSHARED_EXPORT  ClientOutterInterfaceOriginal * CreateClientOutterInterfaceKit();
WEBRTCP2PDLLSHARED_EXPORT  void ReleaseClientOutterInterfaceKit(ClientOutterInterfaceOriginal *);
EXTERN_C_END

EXTERN_C_END
#endif
