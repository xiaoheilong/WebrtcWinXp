#ifndef P2PDLL_INTERFACE_HEADER
#define P2PDLL_INTERFACE_HEADER
#include "DllInterface.h"
#include "WebrtcConductor.h"
#include "TransferData.h"
#include "WebrtcClientExe.h"
#include "WebrtcInitInterface.h"
#include "WebrtcOutterClientExe.h"

EXTERN_C_BEGIN
///////////
int RecieveText(char*, uint8_t*, int);
///////////
WEBRTCP2PDLLSHARED_EXPORT class P2PEnvironmentBase
{
public:
	virtual ~P2PEnvironmentBase();
	virtual ClientOutterInterfaceOriginal * GetInner() = 0;
	virtual bool ServerIsValid() = 0;
	virtual bool PeerIsValid() = 0;
};

WEBRTCP2PDLLSHARED_EXPORT P2PEnvironmentBase* CreateP2PEnvironment(const char *, const int &, const char *,
	const char *, const char *);
WEBRTCP2PDLLSHARED_EXPORT void CloseP2PEnvironment(P2PEnvironmentBase *);
WEBRTCP2PDLLSHARED_EXPORT bool RegisterOutter(P2PEnvironmentBase *, OutterInterfaceBase*);
WEBRTCP2PDLLSHARED_EXPORT bool ServerIsValid(P2PEnvironmentBase *);
WEBRTCP2PDLLSHARED_EXPORT bool PeerIsValid(P2PEnvironmentBase *);
WEBRTCP2PDLLSHARED_EXPORT bool SendText(P2PEnvironmentBase *, wchar_t*,int);
WEBRTCP2PDLLSHARED_EXPORT void GetLocaltionSdpIcecandidate(P2PEnvironmentBase*, \
	char *);
WEBRTCP2PDLLSHARED_EXPORT void SetOppositeSdpIcecandidate(P2PEnvironmentBase * inner,char *);
WEBRTCP2PDLLSHARED_EXPORT bool CreateTurnServerConnection(P2PEnvironmentBase*);
WEBRTCP2PDLLSHARED_EXPORT bool CloseTurnServerConnection(P2PEnvironmentBase*);
WEBRTCP2PDLLSHARED_EXPORT bool SendFile(P2PEnvironmentBase* ,char *, char*);
WEBRTCP2PDLLSHARED_EXPORT void FileTaskProgress(void*, double & , double &);
WEBRTCP2PDLLSHARED_EXPORT bool InterruptFileTask(P2PEnvironmentBase *, char*);
WEBRTCP2PDLLSHARED_EXPORT bool RestartFileTask(P2PEnvironmentBase *, char*);
WEBRTCP2PDLLSHARED_EXPORT bool CancleFileTask(P2PEnvironmentBase *, char*);
WEBRTCP2PDLLSHARED_EXPORT bool CreateRecieveFile(P2PEnvironmentBase * ,char *, char *);
WEBRTCP2PDLLSHARED_EXPORT bool DeleteRecieveFile(P2PEnvironmentBase *, char *);
///media Audio interface 
WEBRTCP2PDLLSHARED_EXPORT bool  StartAudio(P2PEnvironmentBase*);
WEBRTCP2PDLLSHARED_EXPORT void  GetAudioData(P2PEnvironmentBase*, char*);
WEBRTCP2PDLLSHARED_EXPORT bool  PauseAudio(P2PEnvironmentBase*);
WEBRTCP2PDLLSHARED_EXPORT bool  EndAudio(P2PEnvironmentBase*);

///////////media Video interface
WEBRTCP2PDLLSHARED_EXPORT bool  StartVideo(P2PEnvironmentBase*);
WEBRTCP2PDLLSHARED_EXPORT void  GetVideoData(P2PEnvironmentBase*, char*);
WEBRTCP2PDLLSHARED_EXPORT bool  PauseVideo(P2PEnvironmentBase*);
WEBRTCP2PDLLSHARED_EXPORT bool  EndVideo(P2PEnvironmentBase*);
EXTERN_C_END
#endif
