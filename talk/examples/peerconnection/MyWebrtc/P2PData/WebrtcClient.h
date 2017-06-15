/*
*  Copyright 2011 The WebRTC Project Authors. All rights reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef WEBRTC_SELF_CLIENT_HREADER__12T
#define WEBRTC_SELF_CLIENT_HREADER__12T
#pragma once

#include <map>
#include <string>
///
#include "defaults.h"
#include "DllInterface.h"
EXTERN_C_BEGIN

namespace WebrtcClientTool
{
    WEBRTCP2PDLLSHARED_EXPORT size_t SpringtFn(char*buffer, size_t size, char *format, ...);
}
namespace TransferNamespace
{
	class STLStringShaow;
}
///
using namespace WebrtcClientTool;
typedef TransferNamespace::STLStringShaow  STLStringShaow;
class  WEBRTCP2PDLLSHARED_EXPORT WebrtcClientObserver
{
public:
	virtual void OnSignedIn(void * param) = 0;  // Called when we're logged on.
	virtual void OnDisconnected() = 0;
	virtual void OnPeerConnected(void *id) = 0;
	virtual void OnPeerDisconnected(void *peer_id) = 0;
	virtual void OnPeerConnectedServer(void* id) = 0;
	virtual void OnPeerDisConnectedServer(void* id) = 0;
	virtual void OnMessageFromPeer(void *peer_id, const std::string& message) = 0;
	virtual void OnMessageSent(int err) = 0;
	virtual void OnServerConnectionFailure() = 0;

protected:
	virtual ~WebrtcClientObserver();
};
///client state 
enum State 
{
	NOT_CONNECTED,
	RESOLVING,
	SIGNING_IN,
	CONNECTED,
	SIGNING_OUT_WAITING,
	SIGNING_OUT,
};

class WebrtcClientBase;
class WebrtcClientOriginalInner;
class WEBRTCP2PDLLSHARED_EXPORT WebrtcClientOriginalOutter
{
public:
	virtual ~WebrtcClientOriginalOutter();
	virtual bool IsConnectSuccese() const = 0;//juge the connection isn't succese
	virtual void ClearClientKey() = 0;//clear  the  peer client primary key 
	virtual bool IsValidClientKey(void * key)const = 0;
	virtual bool IsCurrentOppositeID(void * id)const = 0; //juge the id pointer is same as the current opposite client
	virtual void * GetClientKey() = 0;// get the client's primary key
	virtual void DoConnectHeaderMsg() = 0;//send the login command message to server
	virtual void SignOutHeaderMsg() = 0;//send the logout command message to server
	virtual bool GetHeaderValue(const std::string& data, size_t eoh, const char* header_pattern, size_t* value) = 0;//get the message header value
	virtual bool GetHeaderValue(const std::string& data, size_t eoh, const char* header_pattern, std::string* value) = 0;
	virtual bool ReadIntoBuffer(void* socket, std::string* data, size_t* content_length) = 0;//read the server message 

	virtual void OnHangingGetConnect(void* socket) = 0;
	virtual void OnRead(void* socket) = 0;
	virtual void OnHangingGetRead(void* socket) = 0;//parse server response from server  peer_id is the size_t type pointer but Indicate it can be every type ,you can convert it to the type which you want

	virtual bool ParseServerResponse(const std::string& response, size_t content_length, void* peer_id, size_t* eoh) = 0;
	virtual void OnMessageFromPeerOut(void* peer_id, const std::string& message) = 0;//message from opposite client

	virtual bool SendToPeer(const std::string& message) = 0;
	virtual bool SendHangUp(void* peer_id) = 0;
	virtual void* GetCurrentConnectPeerInfo() = 0;//get current connectting peer client's information
	virtual bool SetCurrentConnectPeerInfo(void *info) = 0;
	virtual void * GetPeers() = 0;//std::map<int, std::string>
	virtual WebrtcClientOriginalInner * GetInner() = 0;
	virtual void  SetInner(WebrtcClientOriginalInner * value) = 0;
protected:
	WebrtcClientOriginalInner * m_inner;
};

class WEBRTCP2PDLLSHARED_EXPORT BridgeForClientInnerOutter
{
public:
	virtual void * GetServerSocketAddress() = 0;
	virtual void* GetAsyncResolver() = 0;
	virtual void* GetControlSocket() = 0;
	virtual void* GetHangingGetSocket() = 0;
	virtual STLStringShaow * GetConnectData() = 0;
	virtual STLStringShaow * GetControlData() = 0;
	virtual STLStringShaow * GetNotifycationData() = 0;
	virtual STLStringShaow * GetClientName() = 0;
	virtual State *const  GetConnectState() = 0;
};

class WebrtcClientOriginal;

class WEBRTCP2PDLLSHARED_EXPORT WebrtcClientOriginalInner:public BridgeForClientInnerOutter
{
public:
	virtual ~WebrtcClientOriginalInner();
	virtual void Close() = 0;

	virtual bool SignOut() = 0;

	virtual void DoConnect() = 0;

	virtual bool ConnectControlSocket() = 0;

	virtual bool IsSendingMessage() = 0;
	/////////////////
	virtual void InitSocketSignals() = 0;

	virtual void Connect(const std::string& server, int port, const std::string& client_name) = 0;

	virtual void* GetMessageHandler() = 0;

	virtual  void SetParent(const WebrtcClientOriginal * parent) = 0;
	virtual const WebrtcClientOriginal * GetParent() = 0;

	virtual WebrtcClientOriginalOutter * GetOutter() = 0;
	virtual void SetOutter(WebrtcClientOriginalOutter * value) = 0;//this is a dog!
protected:
	WebrtcClientOriginalOutter * m_outter;
};
DllExtern_Fixed_Interface(WebrtcClientOriginalInner)
////////////
class WEBRTCP2PDLLSHARED_EXPORT  WebrtcClientOriginal :public WebrtcClientObserver
{
public:
	virtual ~WebrtcClientOriginal();
	///WebrtcClientObserver
	virtual void OnSignedIn(void * param) = 0;  // Called when we're logged on.
	virtual void OnDisconnected() = 0;
	virtual void OnPeerConnected(void *id) = 0;
	virtual void OnPeerDisconnected(void *peer_id) = 0;
	virtual void OnPeerConnectedServer(void* id) = 0;
	virtual void OnPeerDisConnectedServer(void* id) = 0;
	virtual void OnMessageFromPeer(void *peer_id, const std::string& message) = 0;
	virtual void OnMessageSent(int err) = 0;
	virtual void OnServerConnectionFailure() = 0;

	virtual WebrtcClientOriginalInner * const GetClientInner() = 0;
	virtual WebrtcClientOriginalOutter * const GetClientOutter() = 0;
	virtual void RegisterObserver(WebrtcClientObserver* const  callback) = 0;
};
DllExtern_Fixed_Interface_2(WebrtcClientOriginal, WebrtcClientOriginalInner *, WebrtcClientOriginalOutter *);
/////////////////test window
EXTERN_C_END
#endif  // WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_PEER_CONNECTION_CLIENT_H_
