#include"P2PThread.h"
#include"WebrtcConductor.h"
#include"defaults.h"
#include"TransferData.h"
#include"webrtc/base/logging.h"
#include "webrtc/base/messagehandler.h"
#include "webrtc/base/messagequeue.h"
#include "webrtc/base/thread.h"
#include"CHandleBase.h"
#include<algorithm>
#include<process.h>
//#include<io.h>
#include<iostream>
////////////////
using TransferNamespace::HTHREAD;
///////////////////////
class ThreadMapShadow;
class WEBRTCP2PDLLSHARED_EXPORT P2PThreadManagerFile :public P2PThreadManagerBase
{
public:
	P2PThreadManagerFile();
	virtual ~P2PThreadManagerFile();

	enum MessageType
	{
		RELEASE_THREAD_MESSAGE = 1,
	};
public:
	virtual bool Begin(std::string &guid);
	virtual bool End(std::string &guid);
	virtual void Attach(std::string &threadGUID, P2PThreadBase * thread);
	virtual void UnAttach(std::string &threadGUID);
	virtual void SetMaxThreadCount(int count);
	virtual void SetMinThreadCount(int count);
	virtual P2PThreadBase* GetThreadByGuid(std::string &guid);
	virtual bool SuspendChildThread(std::string &guid);
	virtual bool ResumeChildThread(std::string &guid);
private:
	virtual void ReleaseThread(std::string & guid);
protected:
	virtual void DynamicControlThreadCount();
protected:
	ThreadMapShadow *m_threadMap;
	int m_threadMaxCount;
	int m_threadMinCount;
};
/////////////////////
ThreadTaskDesBase::ThreadTaskDesBase()
{
	resGuid = TransferNamespace::CreateSTLString();
}

ThreadTaskDesBase::~ThreadTaskDesBase()
{
	TransferNamespace::DeleteSTLString(resGuid);
}


ThreadTaskDesBase::ThreadTaskDesBase(const TransferNamespace::STLStringShaow &guid)
{
	resGuid = TransferNamespace::CreateSTLString();
	if (resGuid)
		resGuid->SetString(guid.GetString());
}

ThreadTaskDesBase::ThreadTaskDesBase(const ThreadTaskDesBase & value)
{
	//resGuid = new TransferNamespace::STLStringShaow(*(value.resGuid));
	resGuid = TransferNamespace::CreateSTLString();
	if (resGuid && value.resGuid)
		resGuid->SetString(value.resGuid->GetString());
}

ThreadTaskDesBase::ThreadTaskDesBase(const std::string & value)
{
	resGuid = TransferNamespace::CreateSTLString();
	if (resGuid )
		resGuid->SetString(value);
}

ThreadTaskDesBase & ThreadTaskDesBase::operator=(const ThreadTaskDesBase &value)
{
	if (!resGuid)
	{
		resGuid = TransferNamespace::CreateSTLString();
		if (resGuid && value.resGuid)
			resGuid->SetString(value.resGuid->GetString());
		return (*this);
	}
	//*resGuid = *value.resGuid;
	if (resGuid && value.resGuid)
		resGuid->SetString(value.resGuid->GetString());
	return (*this);
}

bool ThreadTaskDesBase::IsGuidValid()const
{
	if (resGuid)
		return resGuid->IsValid();
	return false;
}
//////////////////////
ThreadTaskDesFile::ThreadTaskDesFile()
{
	resPath = TransferNamespace::CreateSTLString();//default resPath is empty
}

ThreadTaskDesFile::~ThreadTaskDesFile()
{
	TransferNamespace::DeleteSTLString(resPath);
}

ThreadTaskDesFile::ThreadTaskDesFile(const std::string &guid, const std::string &path) :ThreadTaskDesBase(guid)
{
	resPath = TransferNamespace::CreateSTLString();
	if (resPath)
		resPath->SetString(path);
}

ThreadTaskDesFile::ThreadTaskDesFile(const ThreadTaskDesFile & value) :ThreadTaskDesBase(value)
{
	/*resPath.clear();
	resPath = value.resPath;*/
	resPath = TransferNamespace::CreateSTLString();
	if (resPath &&value.resPath)
		resPath->SetString(value.resPath->GetString());
}

ThreadTaskDesFile& ThreadTaskDesFile::operator=(const ThreadTaskDesFile & value)
{
	ThreadTaskDesBase::operator=(value);
	resPath = TransferNamespace::CreateSTLString();
	if (resPath &&value.resPath)
		resPath->SetString(value.resPath->GetString());
	return (*this);
}

bool ThreadTaskDesFile::IsResPathValid()const
{
	if (resPath)
		return resPath->IsValid();
	return false;
}
/////////////
P2PThreadBase::~P2PThreadBase()
{

}
///////////////
P2PThreadManagerBase::~P2PThreadManagerBase()
{

}
//////////////
class ThreadMapShadow
{
public:
	ThreadMapShadow();
	virtual ~ThreadMapShadow();
public:
	virtual void clear();
	virtual std::map<std::string, P2PThreadBase *>::iterator begin();
	virtual std::map<std::string, P2PThreadBase *>::iterator end();
	P2PThreadBase * operator[](const std::string & key);
	virtual void Insert(const std::string & key, P2PThreadBase * thread);
	virtual void erase(const std::string &key);
private:
	std::map<std::string, P2PThreadBase *> m_threadMap;
};

ThreadMapShadow::ThreadMapShadow()
{
	m_threadMap.clear();
}

ThreadMapShadow::~ThreadMapShadow()
{
	std::map<std::string, P2PThreadBase *>::iterator beptr = m_threadMap.begin();
	while (beptr != m_threadMap.end())
	{
		if (beptr->second)
		{
			(beptr->second)->End();
			DeletePtr(beptr->second);
		}
		beptr++;
	}
	m_threadMap.clear();
}

void  ThreadMapShadow::clear()
{
	m_threadMap.clear();
}

std::map<std::string, P2PThreadBase *>::iterator ThreadMapShadow::begin()
{
	return m_threadMap.begin();
}

std::map<std::string, P2PThreadBase *>::iterator ThreadMapShadow::end()
{
	return m_threadMap.end();
}

P2PThreadBase * ThreadMapShadow::operator[](const std::string & key)
{
	std::map<std::string, P2PThreadBase *>::iterator ptr = m_threadMap.find(key);
	if(ptr!= m_threadMap.end())
		return m_threadMap[key];
	return NULL;
}

void ThreadMapShadow::Insert(const std::string & key, P2PThreadBase * thread)
{
	m_threadMap.insert(std::make_pair(key, thread));
}

void ThreadMapShadow::erase(const std::string &key)
{
	m_threadMap.erase(key);
}


////////////
P2PThreadManagerFile::P2PThreadManagerFile()
{
	m_threadMap = new ThreadMapShadow();
	m_threadMaxCount = MAX_INIT_THREAD_COUNT;
	m_threadMinCount = MIN_INIT_THREAD_COUNT;
}

P2PThreadManagerFile::~P2PThreadManagerFile()
{
	DeletePtr(m_threadMap);
}

bool P2PThreadManagerFile::Begin(std::string &guid)
{
	if (!m_threadMap)
		return false;
	P2PThreadBase* thread = GetThreadByGuid(guid);
	if (thread)
	{
		thread->Create();
		//thread->Begin();
		return true;
	}
	return false;
}

bool P2PThreadManagerFile::End(std::string &guid)
{
	if (!m_threadMap)
		return  false;
	P2PThreadBase* thread = GetThreadByGuid(guid);
	if (thread)
	{
		thread->End();
		return true;
	}
	return false;
}

void P2PThreadManagerFile::Attach(std::string &threadGUID, P2PThreadBase * thread)
{
	if (thread)
	{
		thread->Bind(this);
		if (!m_threadMap)
			return;
		m_threadMap->Insert(threadGUID, thread);
	}
}

void P2PThreadManagerFile::UnAttach(std::string &threadGUID)
{
	if (!m_threadMap)
		return;
	P2PThreadBase* thread = GetThreadByGuid(threadGUID);
	if (!thread)
		return;
	thread->ForceQuit(true);//set the transfering thread quit right now
	//ReleaseP2PThreadBase(thread);//release the threadBase memory  (*m_threadMap)[threadGUID]
	//(*m_threadMap).erase(threadGUID);
}

void P2PThreadManagerFile::ReleaseThread(std::string & guid)
{
	P2PThreadBase* thread = GetThreadByGuid(guid);
	if (!thread)
	{
		return;
	}
	ReleaseP2PThreadBase(thread);//release the threadBase memory  (*m_threadMap)[threadGUID]
	(*m_threadMap).erase(guid);
}

//void P2PThreadManagerFile::OnMessage(rtc::Message* msg)
//{
//	if (!msg)
//		return;
//	rtc::TypedMessageData<std::string> *guid = static_cast<rtc::TypedMessageData<std::string>*>(msg->pdata);
//	switch(msg->message_id)
//	{
//	case RELEASE_THREAD_MESSAGE:
//	{
//		if (!guid)
//			break;
//		std::string guid1 = guid->data();
//		P2PThreadBase* thread = GetThreadByGuid(guid1);
//		if (!thread)
//		{
//			DeletePtr(msg->pdata);
//			return;
//		}
//		ReleaseP2PThreadBase(thread);//release the threadBase memory  (*m_threadMap)[threadGUID]
//		(*m_threadMap).erase(guid1);
//		DeletePtr(msg->pdata);
//	}
//	break;
//	default:
//		break;
//	}
//}

void P2PThreadManagerFile::SetMaxThreadCount(int count)
{
	m_threadMaxCount = count;
}

void P2PThreadManagerFile::SetMinThreadCount(int count)
{
	m_threadMinCount = count;
}

P2PThreadBase* P2PThreadManagerFile::GetThreadByGuid(std::string &guid)
{
	return (*m_threadMap)[guid];
}
void P2PThreadManagerFile::DynamicControlThreadCount()
{
	
}

bool P2PThreadManagerFile::SuspendChildThread(std::string &guid)
{
	P2PThreadBase* thread = GetThreadByGuid(guid);
	if (thread)
	{
		return thread->Suspend();
	}
	return false;
}

bool P2PThreadManagerFile::ResumeChildThread(std::string &guid)
{
	P2PThreadBase* thread = GetThreadByGuid(guid);
	if (thread)
	{
		return thread->Resume();
	}
	return false;
}

/////////////////////////////////////
WEBRTCP2PDLLSHARED_EXPORT P2PThreadManagerBase* CreateP2PThreadManagerBase()
{
	return new P2PThreadManagerFile();
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseP2PThreadManagerBase(P2PThreadManagerBase * value)
{
	DeletePtr(value);
}
/////////////////////////////////////
P2PThreadFile::P2PThreadFile()
{
	m_threadHandle  = NULL;
	m_manager = NULL;
	m_senderResDes = new ThreadTaskDesFile();
	m_recieverResDes = new ThreadTaskDesFile();
	m_threadEvent = CreateCHandleBase(new CHandleBaseExe());
	m_forceQuitFlag = false;//default the flag's value is false
	m_taskType = SEND_TASK;
	m_skipFlag = false;
}

P2PThreadFile::P2PThreadFile( void * connection_file)
{
	m_threadHandle = NULL;
	m_manager = NULL;
	m_fileConductor = connection_file;
	//delete the resource description
	m_senderResDes = new ThreadTaskDesFile();
	m_recieverResDes = new ThreadTaskDesFile();
	m_threadEvent = CreateCHandleBase(new CHandleBaseExe());
	//m_threadEvent->CreateEvent(NULL, true, false, NULL);
	m_forceQuitFlag = false;
	//::InitializeCriticalSection(&m_threadEventLock);
	m_taskType = SEND_TASK;
	m_skipFlag = false;
}

P2PThreadFile::~P2PThreadFile()
{
	m_fileConductor = NULL;
	m_skipFlag = false;
	DeletePtr(m_senderResDes);
	DeletePtr(m_recieverResDes);
	m_forceQuitFlag = false;
	//::LeaveCriticalSection(&m_threadEventLock);
	m_taskType = SEND_TASK;
	ReleaseCHandleBase(m_threadEvent);
	m_threadEvent = NULL;
	UnBind();
}

bool P2PThreadFile::Begin()
{
	if (!m_threadHandle)
		return false;
	Resume();
	return true;
}

bool P2PThreadFile::End()
{
	if (m_threadHandle)
	{
		m_threadHandle = NULL; //should reset the handle to NULL
	}
	return true;
}

bool P2PThreadFile::Suspend()
{
	if (!m_threadEvent)
			return false;
	m_skipFlag = true;
	return true;
}



bool P2PThreadFile::Resume()
{
	if (!m_threadEvent)
		return false;
	m_skipFlag = false;
	m_threadEvent->ResetEvent();
	return true;
}


HTHREAD* P2PThreadFile::GetHandle()
{
	return m_threadHandle;
}

void P2PThreadFile::Bind(P2PThreadManagerBase * manager)
{
	m_manager = manager;
}

void P2PThreadFile::UnBind()
{
	m_manager = NULL;
}

void * P2PThreadFile::GetConductor()
{
	return m_fileConductor;
}

bool P2PThreadFile::Create()
{
	/*if (m_threadHandle)
	{
		CloseHandle(m_threadHandle);
	}*/
	//unsigned threadID = 0;
	//m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, &P2PThreadFile::Run, this, CREATE_SUSPENDED, &threadID);
	ThreadConvertBase<P2PThreadFile>  convertFun(&P2PThreadFile::Run);
	m_threadHandle = new std::thread(convertFun(), this);
	if (!m_threadHandle)
		return false;
	m_threadHandle->detach();
	return true;
}

void ReleaseRun(P2PThreadManagerBase * manager, std::string guid)
{
	if (manager)
		manager->ReleaseThread(guid);
}

bool P2PThreadFile::Release()
{
	///////send message to opposite client, indicate the current filetask is end!
	//end the send file data then inform oppositeclient to combination the data packet which recieveed ,to a file 
	WebrtcConductor * fileConnection = static_cast<WebrtcConductor *>(m_fileConductor);
	if (fileConnection)
	{
		std::string md5Str = fileConnection->GetConductResource()->GetResourceMd5(const_cast<std::string &>(m_senderResDes->resGuid->GetString()));
		FileDataPack packEnd;
		packEnd.data->messageType = End_Send_File;
		packEnd.data->packetIndex = 0;
		packEnd.message_id = File_Type;
		int md5Size = md5Str.size();
		memcpy(packEnd.data->message, md5Str.c_str(), md5Size);
		packEnd.data->message[md5Size] = '\0';
		packEnd.owerInfo.SetSuserResGuid(m_senderResDes->resGuid->GetString().c_str());//save the md5 value to the sUserResGuid district
		packEnd.owerInfo.SetRuserResGuid(m_recieverResDes->resGuid->GetString().c_str());
		uint8_t buffer[3000];
		memset(buffer, 0, sizeof(buffer));
		int serializeSize = packEnd.Serialize(buffer, 3000);
		fileConnection->SendData((void *)(buffer), serializeSize);
	}
	////////
	if (m_manager && m_senderResDes)
	{
		if (m_senderResDes->resGuid)
		{
			std::string guid = m_senderResDes->resGuid->GetString();
			std::thread  thread(ReleaseRun, m_manager, guid);
			//m_manager->ReleaseThread(guid);
			thread.detach();
			return true;
		}
		m_manager = NULL;
	}
	
	return false;
}

ThreadTaskType  P2PThreadFile::GetTaskType()
{
	return m_taskType;
}

void P2PThreadFile::SetTaskType(ThreadTaskType type)
{
	m_taskType = type;
}

void P2PThreadFile::SetSenderDes(const ThreadTaskDesFile & res)
{
	*m_senderResDes = res;
}

ThreadTaskDesFile & P2PThreadFile::GetSendResDes()
{
	return *m_senderResDes;
}

void P2PThreadFile::SetRecieveResDes(const ThreadTaskDesFile & res)
{
	*m_recieverResDes = res;
}

ThreadTaskDesFile & P2PThreadFile::GetRecieveResDes()
{
	return *m_recieverResDes;
}

void P2PThreadFile::ForceQuit(bool value)
{
	m_forceQuitFlag = value;
}

bool P2PThreadFile::IsForceQuit()
{
	return m_forceQuitFlag;
}


////hasn't deal the exception the thread return value, temporary not deal the item
unsigned int __stdcall P2PThreadFile::Run()
{
	//run the connection_file send file data
	P2PThreadFile * thread = this;
	LOG(INFO)<<__FUNCTION__ << "Enter the thread Run";
	if (thread)
	{
		//m_threadEvent->SetEvent();
		ThreadTaskDesFile sender = thread->GetSendResDes();
		LOG(INFO) << __FUNCTION__ << "first SetEvent";
		if (sender.IsGuidValid())//check the sender resource guid is't valid
		{
			WebrtcConductor * fileConnection = static_cast<WebrtcConductor *>(thread->GetConductor());
			if (fileConnection)
			{
				//send the file data transfer flag to opposite ,inform opposite client create file
				FileDataPack fileheader;
				fileheader.message_id = File_Type;
				if (!fileheader.data)
				{
					thread->Release();
					return 0;
				}
				fileheader.data->messageType = Create_File;
				fileheader.data->packetIndex = fileConnection->GetConductResource()->GetResBlockSize(const_cast<std::string &>(sender.resGuid->GetString()));//deliver the task total blocksize to reciever
				fileheader.owerInfo.SetSuserResGuid(sender.resGuid->GetString().c_str());
				fileheader.data->SetContent(const_cast<char*>(thread->m_senderResDes->resPath->GetString().c_str()));
				size_t length = sizeof(FileDataPack);
				uint8_t buffer[3000];
				int serializeSize = fileheader.Serialize(buffer, 3000);
				if (!fileConnection->SendData((void *)(buffer), serializeSize))
				{
					m_threadEvent->SetEvent();
				}
				//pause the pause, wait  opposite client's inform, then start the send data stream
				m_threadEvent->SetEvent();
				LOG(INFO) << __FUNCTION__ << "Second  SetEvent";
				//check the reciever resource guid's invalid
				ThreadTaskDesFile reciever = thread->GetRecieveResDes();
				if (!reciever.IsGuidValid())
				{
					thread->Release();
					return 0;
				}
				// behind wake up the thread, shoud caculate the goalfile md5 value 
				if (!fileConnection->GetConductResource())
				{
					thread->Release();
					return 0;
				}
				LOG(INFO) << __FUNCTION__ << "Start Send the Data!";
				while (!thread->IsForceQuit())
				{
					DataChannelTransferBase<> * secondPtr = reinterpret_cast<DataChannelTransferBase<> *>(fileConnection->GetConductResource()->ExtractData(const_cast<std::string &>(sender.resGuid->GetString())));
					if (secondPtr)
					{
						FileDataPack pack;
						pack.message_id = File_Type;
						WebrtcClientOriginalOutter * clientOutter = fileConnection->GetWebrtcClient()->GetClientOutter();
						if (!clientOutter)
						{
							thread->Release();
							return 0;
						}
						DWORD *sUserID = static_cast<DWORD *>(clientOutter->GetClientKey());
						if (sUserID)
							pack.owerInfo.filepath.SetSuserID(*sUserID);
						pack.owerInfo.SetRuserResGuid(thread->GetRecieveResDes().resGuid->GetString().c_str());//set reciever resGuid
						pack.owerInfo.SetSuserResGuid(thread->GetSendResDes().resGuid->GetString().c_str());//set sender  resGuid
						if (!pack.data)
							break;
						*(pack.data)= *secondPtr;
						pack.data->messageType = Send_File;
						memset(buffer, 0, sizeof(buffer));
						int serializeSize = pack.Serialize(buffer, 3000);
						if (!fileConnection->SendData(buffer, serializeSize))
						{
							//send file failure ,should end the thread task at once!
							//m_outterClient->TransferFileComplete(msg->owerInfo.sUserResGuid, FILE_TRANSFER_FAILURE);
							fileConnection->CallbackWebrtcClient(SEND_FILE_FAILURE, &pack);
							DeletePtr(secondPtr);
							break;
						}
						DeletePtr(secondPtr);
					}
					else
					{
						break;
					}
					if (m_skipFlag)//use these codes to support data transfer pause and restart
					{
						if (m_threadEvent)
							m_threadEvent->SetEvent();
					}
				}
			}
		}
	}
	thread->Release();
	return 0;
}

WEBRTCP2PDLLSHARED_EXPORT P2PThreadBase* CreateP2PThreadBase(void * value)
{
	return new P2PThreadFile(value);
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseP2PThreadBase(P2PThreadBase * value)
{
	DeletePtr(value);
}
