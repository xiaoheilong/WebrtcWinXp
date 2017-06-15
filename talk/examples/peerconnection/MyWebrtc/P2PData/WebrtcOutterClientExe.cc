#include"WebrtcOutterClientExe.h"
//#include"main_wnd.h"
#include"WebrtcConductor.h"
#include "P2PDllInterface.h"
#include<iostream>
#include <stdlib.h>
///////////
ClientOutterInterfaceTest::ClientOutterInterfaceTest() : m_serverAddress(""), m_serverPort("")
{
	m_connectPeerVector.clear();
	m_outterWnd = NULL;
	m_tempStr.clear();
	m_serverAddress.clear();
	m_serverPort.clear();
	m_messageBoxSlots.connect(this, &ClientOutterInterfaceTest::MessageBox);
	m_transferProgressSlots.connect(this, &ClientOutterInterfaceTest::TransferProgress);
}

ClientOutterInterfaceTest::ClientOutterInterfaceTest(const std::string & serverAddress, const std::string & serverPort)
	: m_serverAddress(serverAddress), m_serverPort(serverPort)
{
	m_tempStr.clear();
	m_serverAddress.clear();
	m_serverPort.clear();
	m_connectPeerVector.clear();
	m_outterWnd = NULL;
}

ClientOutterInterfaceTest::~ClientOutterInterfaceTest()
{
	m_serverAddress.clear();
	m_serverPort.clear();
	m_connectPeerVector.clear();
	m_tempStr.clear();
}

//////declare some action that client want to run bool LoginIn(const std::string &address, const std::string &ip);//these action is relative sinalserver
bool ClientOutterInterfaceTest::LoginIn(const std::string &address, const int &port)
{
	if (m_callback)
	{
		PeerInfo info(port, address);
		ClientOutterInterfaceBase::SendMessageEx(LOGIN_IN_SERVER, &info);//login server
		return true;
	}
	return false;
}

bool ClientOutterInterfaceTest::LoginOut()
{
	if (m_callback)
	{
		ClientOutterInterfaceBase::SendMessageEx(LOGIN_OUT_SERVER, NULL);//login server
		return true;
	}
	return false;
}

bool ClientOutterInterfaceTest::ConnectToPeer(const PeerInfo & peerinfo)
{
	if (m_callback)
	{
		ClientOutterInterfaceBase::SendMessageEx(CONNECT_PEER, const_cast<int *>(&peerinfo.id));//login server
		return true;
	}
	return false;
}

bool ClientOutterInterfaceTest::DisConnectToPeer()
{
	if (m_callback)
	{
		ClientOutterInterfaceBase::SendMessageEx(DISCONNECT_PEER, NULL);//disconnect from signalserver
		return true;
	}
	return false;
}

void ClientOutterInterfaceTest::RecieveTextMessage(uint8_t * const message, int size)
{
	if (m_outterWnd)
	{
		char msg[3000];
		memset(msg, 0, 3000);
		RecieveText(msg, message, size);
		std::wstring str = MsToUc(msg);
		m_outterWnd->RecieveDataChannel(str.c_str());
	}
}


void ClientOutterInterfaceTest::RecieveVideoChannel(const cricket::VideoFrame& frame)
{

}

void ClientOutterInterfaceTest::RecieveAudioChannel(const void * data)
{

}

void ClientOutterInterfaceTest::AttachedWnd(OutterInterfaceBase * wnd)
{
	m_outterWnd = wnd;
}

void ClientOutterInterfaceTest::StartLocalRenderer(void* local_video)
{
	if (m_outterWnd)
		m_outterWnd->StartLocalRenderer(local_video);
}

void ClientOutterInterfaceTest::StopLocalRenderer()
{
	if (m_outterWnd)
		m_outterWnd->StopLocalRenderer();
}

void ClientOutterInterfaceTest::StartRemoteRenderer(void* remote_video)
{
	if (m_outterWnd)
		m_outterWnd->StartRemoteRenderer(remote_video);
}

void ClientOutterInterfaceTest::StopRemoteRenderer()
{
	if (m_outterWnd)
		m_outterWnd->StopRemoteRenderer();
}

void  ClientOutterInterfaceTest::TransferProgress(const void *resGuid, const void *progress)
{
	//std::string resGuid, double progress
	if (m_outterWnd)
	{
		const std::string *guid = reinterpret_cast<const std::string *>(resGuid);
		const TransferProgressPacket * progress1 = reinterpret_cast<const TransferProgressPacket *>(progress);
		//TransferProgressPacket* value =  new TransferProgressPacket(*(const_cast<std::string *>(guid)), progress1->totalTaskSize,progress1->finishTaskSize);
		m_outterWnd->TransferProgress(guid->c_str(), progress1->totalTaskSize, progress1->finishTaskSize);
	}
}

bool ClientOutterInterfaceTest::CreateBarcodeFile(void * res)
{
	return ClientOutterInterfaceBase::CreateBarcodeFile(res);
}

bool ClientOutterInterfaceTest::SendFile(void* param)
{
	return ClientOutterInterfaceBase::SendFile(param);
}

bool ClientOutterInterfaceTest::SendTextMessage(void *param, int paramsize)
{
	return ClientOutterInterfaceBase::SendTextMessage(param, paramsize);
}

//bool ClientOutterInterfaceTest::OnFileCancel(const char * param)
//{
//	if (m_outterWnd)
//	{
//		m_outterWnd->OnFileCancel(param);
//		return true;
//	}
//	return false;
//}

bool ClientOutterInterfaceTest::LoginInSuccese(void * prama)
{
	//call back more outter interface
	if (m_outterWnd)
	{
		return true;
	}
	return false;
}

bool ClientOutterInterfaceTest::LoginInFailure(void * prama)
{
	return true;
}


bool ClientOutterInterfaceTest::LoginOutSuccese(void *param)
{
	if (m_outterWnd)
	{
		return true;
	}
	return false;
}

bool ClientOutterInterfaceTest::LoginOutFialure(void * param)
{
	return true;
}

bool ClientOutterInterfaceTest::ConnectToPeerSuccese(void * prama)
{
	if (m_outterWnd)
	{
		return true;
	}
	return false;
}

bool ClientOutterInterfaceTest::ConnectToPeerFailure(void * prama)
{
	if (m_outterWnd)
	{
		return true;
	}
	return false;
}


bool ClientOutterInterfaceTest::PeerConnectToServer(void * param)
{
	if (m_outterWnd)
	{
		m_outterWnd->OnConnectTurnServer(Connect_Success);
		return true;
	}
	return false;
}

bool ClientOutterInterfaceTest::PeerDisConnectToServer(void *param)
{
	if (m_outterWnd)
	{
		m_outterWnd->OnConnectTurnServer(Connect_Failure);
		return true;
	}
	return false;
}

bool ClientOutterInterfaceTest::PeerNeedRebuildTurnserverConnection(const int & type, const int & state)
{
	if (m_outterWnd)
	{
		m_outterWnd->OnDisConnection(type, state);
	}
	return false;
}

bool ClientOutterInterfaceTest::SetOppositeSDPInfo(const std::string &sdp)
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		ptr->SetOppositeSDPInformation(sdp);
		return true;
	}
	return false;
}

bool ClientOutterInterfaceTest::SetOppositeIceCandidate(const std::vector<std::string >&icecandidate)
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		ptr->SetOppositeIceCandidate(icecandidate);
		return true;
	}
	return false;
}


const std::string&   ClientOutterInterfaceTest::GetOwerSDPInformation()
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		return ptr->GetOwerSDPInformation();
	}
	m_tempStr.clear();
	return m_tempStr;
}

const std::vector<std::string> * ClientOutterInterfaceTest::GetOwerIceCandidate()
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		return ptr->GetOwerIceCandidate();
	}
	return NULL;
}


void  ClientOutterInterfaceTest::AcceptFile(void * param)
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		ptr->AcceptFile(param);
	}
}

void  ClientOutterInterfaceTest::RefuseFile(void * param)
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		ptr->RefuseFile(param);
	}
}

bool  ClientOutterInterfaceTest::InterruptFileTask(void * param)
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		return ptr->InterruptFileTask(param);
	}
	return false;
}

bool  ClientOutterInterfaceTest::CreateRecieveFile(void * param, void * param1)
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		return ptr->CreateRecieveFile(param, param1);
	}
	return false;
}

bool  ClientOutterInterfaceTest::DeleteRecieveFile(void * param)
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		return ptr->DeleteRecieveFile(param);
	}
	return false;
}

bool  ClientOutterInterfaceTest::RestartFileTask(void * param)
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		return ptr->RestartFileTask(param);
	}
	return false;
}

bool  ClientOutterInterfaceTest::CancleFileTask(void * param)
{
	WebrtcConductorDecorator * ptr = static_cast<WebrtcConductorDecorator*>(m_callback);
	if (ptr)
	{
		return ptr->CancleFileTask(param);
	}
	return false;
}

void ClientOutterInterfaceTest::TransferProgressSlots(const void *resGuid, const void* progress)
{
	m_transferProgressSlots(resGuid, progress);
}

void  ClientOutterInterfaceTest::TransferFileComplete(const char * resGuid, const FileTransferState& state)
{
	if (m_outterWnd)
	{
		m_outterWnd->TransferFileComplete(resGuid, state);
	}
}

void ClientOutterInterfaceTest::ErrorMessageBoxSlots(const char* text)
{
	m_messageBoxSlots(text);
}

void ClientOutterInterfaceTest::MessageBox(const void * text)
{
	const char * str = reinterpret_cast<const char *>(text);
	DWORD flags = MB_OK | MB_ICONERROR;
	::MessageBoxA(NULL, str, "error!", flags);
}
////////////////
ClientOutterInterfaceSingle::ClientOutterInterfaceSingle()
{

}

ClientOutterInterfaceSingle::ClientOutterInterfaceSingle(const std::string & serverAddress, const std::string & serverPort)
	:ClientOutterInterfaceTest(serverAddress, serverPort)
{

}

ClientOutterInterfaceSingle::~ClientOutterInterfaceSingle()
{

}

bool ClientOutterInterfaceSingle::ConnectToPeerSuccese(void * prama)
{
	if (m_outterWnd)
	{
		m_outterWnd->OnConnectToPeer(Connect_Success);
		return true;
	}
	return false;
}

bool ClientOutterInterfaceSingle::ConnectToPeerFailure(void * prama)
{
	if (m_outterWnd)
	{
		m_outterWnd->OnConnectToPeer(Connect_Failure);
		return true;
	}
	return false;
}


void ClientOutterInterfaceSingle::StartLocalRenderer(void* local_video)
{
	ClientOutterInterfaceTest::StartLocalRenderer(local_video);
}

void ClientOutterInterfaceSingle::StopLocalRenderer()
{
	ClientOutterInterfaceTest::StopLocalRenderer();
}

void ClientOutterInterfaceSingle::StartRemoteRenderer(void* remote_video)
{
	ClientOutterInterfaceTest::StartRemoteRenderer(remote_video);
}

void ClientOutterInterfaceSingle::StopRemoteRenderer()
{
	ClientOutterInterfaceTest::StopRemoteRenderer();
}
//////////////////
WEBRTCP2PDLLSHARED_EXPORT  ClientOutterInterfaceOriginal * CreateClientOutterInterfaceKit()
{
	return new ClientOutterInterfaceSingle();
}

WEBRTCP2PDLLSHARED_EXPORT  void ReleaseClientOutterInterfaceKit(ClientOutterInterfaceOriginal *value)
{
	DeletePtr(value);
}


