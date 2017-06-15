#ifndef WEBRTC_CLIENT_HEADER_
#define WEBRTC_CLIENT_HEADER_
#pragma once
#include "./DllInterface.h"
#include"WebrtcClient.h"
EXTERN_C_BEGIN
class WebrtcClient : public WebrtcClientOriginalOutter
{
public:
	WebrtcClient(WebrtcClientObserver * call = NULL);

	virtual ~WebrtcClient();
public:
	virtual bool IsConnectSuccese() const;

	virtual void ClearClientKey();

	virtual bool IsValidClientKey(void * key)const;

	virtual bool IsCurrentOppositeID(void * id)const;

	virtual void*GetClientKey();

	virtual void SignOutHeaderMsg();////child class could override the fuction

	virtual void DoConnectHeaderMsg();////child class could override the fuction

	virtual void OnHangingGetConnect(void* socket1);//could been override

	virtual void Close();

	// Quick and dirty support for parsing HTTP header values.
	virtual bool GetHeaderValue(const std::string& data, size_t eoh, const char* header_pattern, size_t* value);

	virtual bool GetHeaderValue(const std::string& data, size_t eoh, const char* header_pattern, std::string* value);

	// Returns true if the whole response has been read.
	virtual bool ReadIntoBuffer(void* socket1, std::string* data, size_t* content_length);

	virtual void OnRead(void* socket1);

	virtual void OnMessageFromPeerOut(void* peer_id, const std::string& message);

	virtual bool SendToPeer(const std::string& message);
	

	virtual bool SendHangUp(void * peer_id);

	virtual void OnHangingGetRead(void* socket1);

	// Parses a single line entry in the form "<name>,<id>,<connected>"
	virtual bool ParseEntry(const std::string& entry, std::string* name, size_t* id, bool* connected);

	virtual int GetResponseStatus(const std::string& response);

	virtual bool ParseServerResponse(const std::string& response, size_t content_length, void* peer_id, size_t* eoh);

	virtual void* GetCurrentConnectPeerInfo();

	virtual bool SetCurrentConnectPeerInfo(void *info);

	virtual void* GetPeers();//std::map<int, std::string>

	virtual  void SetParent(const WebrtcClientOriginal * parent);
	virtual const WebrtcClientOriginal * GetParent();

	virtual WebrtcClientOriginalInner * GetInner();
	virtual void  SetInner(WebrtcClientOriginalInner * value);
private:
	int m_myID;//self client ID, this client use int type as client primary key
	int m_oppositePeerID;
	std::map<int, std::string>m_peers;//include all client that arealdy login server
	WebrtcClientOriginal * m_parent;
};


EXTERN_C_BEGIN
WEBRTCP2PDLLSHARED_EXPORT  WebrtcClientOriginalOutter * CreateWebrtcClient();
WEBRTCP2PDLLSHARED_EXPORT  void ReleaseWebrtcClient(WebrtcClientOriginalOutter *);
EXTERN_C_END

EXTERN_C_END
#endif
