#ifndef P2P_THREAD_HEADER
#define P2P_THREAD_HEADER
#include"DllInterface.h" 
#include"CHandleBase.h"
#include<thread>
#include<vector>
#include<map>
#define MAX_INIT_THREAD_COUNT 10
#define MIN_INIT_THREAD_COUNT 1
//////////
EXTERN_C_BEGIN
namespace TransferNamespace 
{
	class STLStringShaow;
	typedef std::thread HTHREAD;
}

enum  ThreadTaskType
{
	SEND_TASK = 1,
	RECIEVE_TASK
};

struct WEBRTCP2PDLLSHARED_EXPORT ThreadTaskDesBase
{
	ThreadTaskDesBase();
	virtual ~ThreadTaskDesBase();
	ThreadTaskDesBase(const TransferNamespace::STLStringShaow &guid);

	ThreadTaskDesBase(const ThreadTaskDesBase & value);

	ThreadTaskDesBase(const std::string & value);

	ThreadTaskDesBase & operator=(const ThreadTaskDesBase &value);

	bool IsGuidValid()const;
	TransferNamespace::STLStringShaow * resGuid;
};

struct WEBRTCP2PDLLSHARED_EXPORT ThreadTaskDesFile :public ThreadTaskDesBase
{
	ThreadTaskDesFile();

	virtual ~ThreadTaskDesFile();

	ThreadTaskDesFile(const std::string &guid, const std::string& path);

	ThreadTaskDesFile(const ThreadTaskDesFile & value);

	ThreadTaskDesFile& operator=(const ThreadTaskDesFile & value);

	bool IsResPathValid()const;
	TransferNamespace::STLStringShaow * resPath;
};

/////////////////////////
class P2PThreadBase;
class WEBRTCP2PDLLSHARED_EXPORT P2PThreadManagerBase
{
public:
	virtual ~P2PThreadManagerBase();
public:
    virtual bool Begin(std::string &guid) = 0;
    virtual bool End(std::string &guid) = 0;
	virtual void Attach(std::string& threadGUID,P2PThreadBase * thread) = 0;
	virtual void UnAttach(std::string& threadGUID) = 0;
	virtual void SetMaxThreadCount(int count) = 0;
	virtual void SetMinThreadCount(int count) = 0;
	virtual P2PThreadBase* GetThreadByGuid(std::string& guid) = 0;
	virtual bool SuspendChildThread(std::string &guid) = 0;
	virtual bool ResumeChildThread(std::string &guid) = 0;

	virtual void ReleaseThread(std::string & guid) = 0;
protected:
	virtual void DynamicControlThreadCount() = 0;
};

DllExtern_Fixed_Interface(P2PThreadManagerBase)
//////////////////////////
class WEBRTCP2PDLLSHARED_EXPORT P2PThreadBase
{
public:
	virtual ~P2PThreadBase();
public:
    virtual bool Begin() = 0;
    virtual bool End() = 0;
    virtual bool Suspend() = 0;
	virtual bool Resume() = 0;
	virtual TransferNamespace::HTHREAD* GetHandle() = 0;
	virtual void Bind(P2PThreadManagerBase * manager) = 0;
	virtual void UnBind() = 0;
	virtual void SetSenderDes(const ThreadTaskDesFile & res) =  0;
	virtual ThreadTaskDesFile & GetSendResDes() = 0;
	virtual void SetRecieveResDes(const ThreadTaskDesFile & res) = 0;
	virtual ThreadTaskDesFile & GetRecieveResDes() = 0;
    virtual void ForceQuit(bool value) = 0;
	virtual bool IsForceQuit() = 0;
    virtual bool Create() = 0;
	virtual bool Release() = 0;
	virtual ThreadTaskType  GetTaskType() = 0;
	virtual void SetTaskType(ThreadTaskType type) = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT P2PThreadProgressBase
{
public:
    virtual float CurrentProgress() = 0;
};

class CHandleBase;
class CThreadBase;
class WEBRTCP2PDLLSHARED_EXPORT P2PThreadFile :public P2PThreadBase
{
public:
	P2PThreadFile();
	P2PThreadFile(void * connection_file);
	~P2PThreadFile();
public:
    virtual bool Begin();
    virtual bool End();
    virtual bool Suspend();
	virtual bool Resume();
	virtual TransferNamespace::HTHREAD* GetHandle();
	virtual void Bind(P2PThreadManagerBase * manager);
	virtual void UnBind();

	void * GetConductor();
    virtual bool Create();
	virtual bool Release();
	virtual ThreadTaskType  GetTaskType();
	virtual void SetTaskType(ThreadTaskType type);
	virtual void SetSenderDes(const ThreadTaskDesFile & res );
	virtual ThreadTaskDesFile & GetSendResDes();
	virtual void SetRecieveResDes(const ThreadTaskDesFile & res);
	virtual ThreadTaskDesFile & GetRecieveResDes();
	virtual void ForceQuit(bool value) ;
protected:
	virtual bool IsForceQuit();
protected:
	unsigned int __stdcall Run();
    void * m_fileConductor;
    ThreadTaskDesFile *m_senderResDes;
    ThreadTaskDesFile *m_recieverResDes;
	ThreadTaskType m_taskType;
	CHandleBase *m_threadEvent;
    bool m_forceQuitFlag;
	bool m_skipFlag;
    TransferNamespace::HTHREAD *m_threadHandle;
	P2PThreadManagerBase * m_manager;
};

DllExtern_Fixed_Interface_1(P2PThreadBase,void*)
EXTERN_C_END
#endif
