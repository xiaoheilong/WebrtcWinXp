#ifndef TRANSFER_DATA_HEADER
#define TRANSFER_DATA_HEADER
#include"CFileMd5.h"
#include"DllInterface.h"
#include"defaults.h"
#include<string.h>
typedef unsigned long  DWORD;
namespace TransferNamespace {
	class STLStringShaow
	{
	public:
		virtual ~STLStringShaow() {}
	public:
		virtual std::string const & GetString()const = 0;
		virtual void SetString(const std::string &string) = 0;
		virtual bool IsValid() = 0;
		virtual int compare(const STLStringShaow & str)const = 0;
		virtual void Add(const STLStringShaow & value) = 0;
		virtual void Add(const std::string & value) = 0;
		virtual void clear() = 0;
	};

	class ContainerInfoShaowBase
	{
	public:
		virtual ~ContainerInfoShaowBase() {}
	public:
		virtual void* GetInfoVector() = 0;
	};

	STLStringShaow*  CreateSTLString();
	STLStringShaow*  CreateSTLStringEx(const std::string & string);
	void DeleteSTLString(STLStringShaow * ptr);

#define CreateContainer(T) (new T())
#define DeleteContainer(T) DeletePtr(T)
}
typedef /*typename*/ TransferNamespace::STLStringShaow  STLStringShaow;
typedef /*typename*/ TransferNamespace::ContainerInfoShaowBase  ContainerInfoShaowBase;

template<class T>
class ThreadConvertBase
{
protected:
	typedef  unsigned int(__stdcall *Fun1)(void *);
	typedef  unsigned int(__stdcall T::*Fun2)();
	union
	{
		Fun1 ThreadFun;
		Fun2 MemberFun;
	}stdThreadFun;
public:
	ThreadConvertBase(Fun2 memberFun)
	{
		SetParamFun(memberFun);
	}

	Fun1 operator()()
	{
		return GetNoParamFun();
	}
private:
	Fun1  GetNoParamFun()
	{
		return stdThreadFun.ThreadFun;
	}

	void SetParamFun(Fun2 memberFun)
	{
		stdThreadFun.MemberFun = memberFun;
	}
};


struct  DataChannelMessageBase
{
    DWORD messageType;
};
template<int number = PER_BLOCK_SIZE>
struct DataChannelTransferBase :public DataChannelMessageBase
{
	DWORD packetIndex;//packet index
	int   messageSize = number;//???????
	char  message[number];//????
	void operator()(const DataChannelTransferBase<number> &value)
	{
		*this = value;
	}

	DataChannelTransferBase<number> & operator=(const DataChannelTransferBase<number> &value)
	{
		memset(message, 0, number);//clear all data to zore
		messageSize = value.messageSize;
		messageType = value.messageType;
		memcpy(message, value.message, messageSize); //not add '\0' charactor to the message,
													 ///because the messsage save data which could be binary data
		packetIndex = value.packetIndex;
		return (*this);
	}

	DataChannelTransferBase<number> * operator=(const DataChannelTransferBase<number> *value)
	{
		memset(message, 0, number);//clear all data to zore
		messageSize = value->messageSize;
		messageType = value->messageType;
		memcpy(message, value->message, messageSize); //not add '\0' charactor to the message,
													  ///because the messsage save data which could be binary data
		packetIndex = value->packetIndex;
		return (this);
	}

	void SetContent(char * str)
	{
		memset(message, 0, number);
		if (str)
		{
            std::string tempStr(str);
            messageSize = tempStr.size();
			memcpy(message, str, messageSize);
			message[messageSize] = '\0';
		}

	}

	bool operator<(const DataChannelTransferBase<number> & value)
	{
		return packetIndex < value.packetIndex;
	}

	bool operator>(const DataChannelTransferBase<number>& value)const
	{
		return packetIndex > value.packetIndex;
	}

	bool operator==(const DataChannelTransferBase<number> & value)const
	{
		return Equal(value);
	}

	bool Equal(const DataChannelTransferBase<number>& value)
	{
		return(packetIndex == value.packetIndex && messageType == value.messageType && messageSize == value.messageSize
			&& memcmp(message, value.message, messageSize) == 0);
	}
};
//sort function object of DataChannelTransferBase<>
template<int number = PER_BLOCK_SIZE>
class Sort_DataChannelTransferBase
{
public:
	bool operator()(const DataChannelTransferBase<number> *value1, const DataChannelTransferBase<number> *value2)
	{
		if (!value1)
		{
			if (value2)
				return false;
		}
		else
		{
			if (value2)
				return value1->packetIndex < value2->packetIndex;
			else
				return true;
		}
		return true;
	}
};

//
EXTERN_C_BEGIN
//define message id 
enum MessageID
{
	Create_File = 1,
	Send_File  = 0,
	Resume_Opposite_Send,
	Recieve_File,
	Can_Continue_Send,
	Not_Can_Continue_Send,
	RFileRecieveState_To_Sender,
	RFileSendState_To_Reciever,
	End_Send_File,
	End_Recieve_File,
	Succese_Recieve_File,
	Failure_Recieve_File,
	Interrupt_File_Transfer,
	Restart_File_Transfer,
	Cancle_File_Transfer
};



//////////
struct WEBRTCP2PDLLSHARED_EXPORT FilePathParam
{
	DWORD sUserID;
	DWORD rUserID;
	void Set(const DWORD &suserid, const DWORD &ruserid/*, const char *f*/);

	void Set(const FilePathParam &value);

	FilePathParam & operator=(const FilePathParam &value);

	void SetSuserID(DWORD id);

	void SetRuserID(DWORD id);
};

struct WEBRTCP2PDLLSHARED_EXPORT FileResourceParam
{
	FilePathParam filepath;
	char sUserResGuid[40]; //sender resource's guid
	char rUserResGuid[40]; //recieve resource's guid
	void Set(const FilePathParam &value, const char * sGuid, const char * rGuid);

	void SetSuserResGuid(const char *str);

	void SetRuserResGuid(const char * str);

	FileResourceParam & operator=(const FileResourceParam & value);
};
////////// "header(2bit) +" type(1 byte) + length(4 byte) + value "+ tail" : header and tail not neccesary!
enum PacketType
{
	TYPE_ZORE = 0,
	FILEDATABASE ,//FileDataBase's series message
};

class SerializeDataPacket
{
public://size is the out or in buffer size
	virtual int  Serialize(uint8_t* out, int size) = 0;//out is the buffer which will save the SerializeDataPacket object serialize data
	virtual int Deserialize(uint8_t * in, int size) = 0;//in is the buffer which will parseed the data that to assignment to the SerializeDataPacket object
};
///
struct WEBRTCP2PDLLSHARED_EXPORT FileDataBase:public SerializeDataPacket
{
	void Set(const FileDataBase & pack);

	FileDataBase();
	FileDataBase & operator=(const FileDataBase & value);

	FileDataBase* operator=(const FileDataBase * value);
	virtual int Serialize(uint8_t* out, int size);
	virtual int Deserialize(uint8_t * in, int size);
	DWORD message_id;//the message id 
	FileResourceParam owerInfo;// the message's client information
};
///file data transfer packet It's include truely file data and the data's owers info
struct WEBRTCP2PDLLSHARED_EXPORT FileDataPack:public FileDataBase
{ 
	void Set(const FileDataPack & pack);
	FileDataPack();
	FileDataPack(const FileDataPack & pack);
	~FileDataPack();
	FileDataPack& operator=(const FileDataPack & value);
	FileDataPack* operator=(const FileDataPack * value);
	
	virtual int Serialize(uint8_t* out, int size);
	virtual int Deserialize(uint8_t * in, int size);

	DataChannelTransferBase<> *  data;
};

//class FileDataPackOutter
//{
//public:
//	virtual ~FileDataPackOutter() {}
//	virtual char * GetData() = 0;
//	virtual void SetData(const std::string &) = 0;
//	virtual int MessageType() = 0;
//	virtual void Deserialize(uint8_t * in, int size) = 0;
//};

//WEBRTCP2PDLLSHARED_EXPORT FileDataPackOutter * CreateFileDataPackOutter();
//WEBRTCP2PDLLSHARED_EXPORT void ReleaseFileDataPackOutter(FileDataPackOutter *);
////////////////
enum RespositorySwitchState
{
	Close_State = 0,
	Open_State =1
};

enum ResType
{
	Audio_Type = 1,
	Video_Type,
	File_Type,
	File_Type_Shake_Pack,
	Text_Type,
	Empty_Type
};

struct WEBRTCP2PDLLSHARED_EXPORT ResDescriptionBase
{
	ResDescriptionBase();
	virtual ~ResDescriptionBase();
	ResDescriptionBase(const ResDescriptionBase & value);
	bool operator <(const ResDescriptionBase &value)const;

	bool operator ==(const ResDescriptionBase &value)const;

	bool operator >(const ResDescriptionBase &value)const;

	ResDescriptionBase & operator=(const ResDescriptionBase & value);
	ResType type;
	STLStringShaow *guid;
};

struct  MediaResDescription :public ResDescriptionBase
{
	std::string mediaName;
};

struct FileResDescription:public ResDescriptionBase
{
	FileResDescription();
	virtual ~FileResDescription();
	FileResDescription(const ResDescriptionBase & value);
	bool operator <(const FileResDescription &value)const;

	bool operator ==(const FileResDescription &value)const;

	bool operator >(const FileResDescription &value)const;

	FileResDescription & operator=(const FileResDescription & value);
	STLStringShaow * filepath;
};


struct WEBRTCP2PDLLSHARED_EXPORT TransferProgressPacket
{
	TransferProgressPacket(const std::string & id, double totalSize, double finishSize);
	virtual ~TransferProgressPacket();
	TransferProgressPacket(const TransferProgressPacket & value);
	TransferProgressPacket();
	void Clear();
	double totalTaskSize;
	double finishTaskSize;
	TransferProgressPacket & operator=(const TransferProgressPacket & value);
	TransferProgressPacket * operator=(const TransferProgressPacket * value);
	STLStringShaow * guid;
};

//////////////turnserver address information
struct WEBRTCP2PDLLSHARED_EXPORT ServerInfo
{
	ServerInfo();
	ServerInfo(const ServerInfo & value);
	ServerInfo(const std::string &a, const std::string &p, const std::string &u = "");
	~ServerInfo();
	ServerInfo& operator=(const ServerInfo & value);
	bool operator<(const ServerInfo & value)const;
	bool operator==(const ServerInfo &value)const;
	virtual void SetAddress(const std::string &a);
	virtual void SetPort(const std::string &p);
	virtual void SetUserName(const std::string &u);
	virtual std::string& GetAddress();
	virtual std::string& GetPort();
	virtual std::string& GetAddressPort();
	virtual std::string& GetUserNameEx();
private:
	STLStringShaow *address;
	STLStringShaow *port;
	STLStringShaow *username;
	STLStringShaow *combination;
};


struct WEBRTCP2PDLLSHARED_EXPORT TurnserverInfo :virtual public ServerInfo
{
	TurnserverInfo();
	TurnserverInfo(const std::string &a, const std::string &p, const std::string & u,
		const std::string &pwd, const std::string &r);
	TurnserverInfo(const TurnserverInfo &value);
	TurnserverInfo & operator=(const TurnserverInfo & value);
	~TurnserverInfo();
	bool operator<(const TurnserverInfo & value)const;
	bool operator==(const TurnserverInfo & value)const;
	virtual void SetPassword(const std::string &p);
	virtual void SetReaml(const std::string &r);
	virtual std::string& GetPassword();
	virtual std::string& GetRealm();
private:
	STLStringShaow * password;
	STLStringShaow * realm;
};

EXTERN_C_BEGIN
WEBRTCP2PDLLSHARED_EXPORT TurnserverInfo * CreateTurnserverInfo(const std::string &, const std::string &, const std::string &,
	const std::string &, const std::string &);
WEBRTCP2PDLLSHARED_EXPORT void ReleaseTurnserverInfo(TurnserverInfo * value);
EXTERN_C_END
//const std::string videoDeviceName = "video_device";
//const std::string audioDeviceName = "audio_device";
//const std::string fileDeviceName =  "file_device";
/*************************
this class is the base class that a webrtc's connection,It's the interface which to transfer data
the data is include:Video?Audio? normal data etc.
Otherwise, the base class is useed to  link the DataRepositoryBase
*******************/
class PeerConnectIterator;
class DataRepositoryBase;
struct TransferProgressPacket;
class WEBRTCP2PDLLSHARED_EXPORT ConductorBase
{
public:
	virtual ~ConductorBase();
public:
	virtual void *ExtractData(std::string &resGuid) = 0;//extract data from dataRepository which GUID is resGuid
	virtual double GetResBlockSize(std::string &resGuid) = 0;
	virtual void SetResBlockSize(std::string &resGuid, double count) = 0;
	virtual TransferProgressPacket* ResProgressDisk(std::string &resGuid) = 0;
	virtual	bool Save(std::string &resGuid, void * value) = 0;//save data to dataRepository which GUID is resGuid
	virtual std::string& CreateDataRepository(ResDescriptionBase &resdes,DataRepositoryBase * data) = 0;//Create a Video Repository,deliver the param which has import information 
	virtual bool  DeleteDataRepository(std::string & guid) = 0;
	virtual void AttachDataLine(PeerConnectIterator * dataline) = 0;
	virtual void UnAttachDataLine() = 0;
	virtual bool IsExitRes(std::string& guid) = 0;
	virtual std::string& GetResourceMd5(std::string& resGuid) = 0; //according the guid get resource md5 value
	virtual DataRepositoryBase * GetRespositoryByGUID(std::string& guid) = 0;
};


class WEBRTCP2PDLLSHARED_EXPORT ConductorPeerConnection :public ConductorBase
{
public:
	~ConductorPeerConnection();
	ConductorPeerConnection(PeerConnectIterator * iterator);
	ConductorPeerConnection() = delete;
public:
	virtual void *ExtractData(std::string &resGuid);
	virtual double GetResBlockSize(std::string &resGuid);//
	virtual void SetResBlockSize(std::string &resGuid, double count);
	virtual TransferProgressPacket* ResProgressDisk(std::string &resGuid);
	virtual	bool Save(std::string &resGuid, void * value) ;
	virtual std::string& CreateDataRepository(ResDescriptionBase &resdes, DataRepositoryBase * data);
	virtual bool  DeleteDataRepository(std::string & guid);
	virtual void AttachDataLine(PeerConnectIterator * dataline) ;
	virtual void UnAttachDataLine() ;
	virtual bool IsExitRes(std::string &guid);
	virtual std::string &GetResourceMd5(std::string& resGuid);
	virtual DataRepositoryBase * GetRespositoryByGUID(std::string &guid);
protected:
	//first string mean the device path, second std::string is data's res GUID string
	ContainerInfoShaowBase * m_resVector;
	PeerConnectIterator * m_resourceIterator;
	STLStringShaow * m_md5;
	STLStringShaow * m_tempStr;
};

DllExtern_Dynamic_Interface(ConductorBase)
//////////////////////////////
class DataRepositoryBase;
class WEBRTCP2PDLLSHARED_EXPORT PeerConnectIterator
{
public:
	virtual ~PeerConnectIterator();
public:
	virtual bool IsConnectPeer() = 0;  //this Function is to juge that connection weather connection succuse!
	virtual bool SetDataSwitch(DataRepositoryBase * res, bool value) = 0;
	virtual bool SetDataSwitch(std::string& guid,bool value) = 0;
	virtual void FitOnDataRespository(DataRepositoryBase * res) = 0;//install the DataRespository
	virtual void DischargeDataRespository(std::string &guid) = 0;//uninstall the DataRespository
	virtual void AttachConductor(ConductorBase * conductor) = 0;
	virtual void UnAttachConductor() = 0;
	virtual bool Push(std::string& resGuid, void * data) = 0;//default push the data to top of the datarepository
	virtual void* Pop(std::string& resGuid) = 0; //default pop the top of the datarepository
	virtual double GetResBlockSize(std::string& resGuid) = 0;
	virtual void SetResBlockSize(std::string& resGuid, double count) = 0;
	virtual TransferProgressPacket* ResProgressDisk(std::string& resGuid) = 0;
	virtual std::string& CreateDataRepository(ResDescriptionBase &resdes, DataRepositoryBase * data) = 0;
	virtual bool  DeleteDataRepository(std::string & guid) = 0;
	virtual std::string& GetResourceMd5(std::string& resGuid) = 0;
	virtual DataRepositoryBase * GetRespositoryByGUID(std::string& guid) = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT PeerConnectNormalDataIterator :public PeerConnectIterator
{
public:
	explicit PeerConnectNormalDataIterator();
	virtual ~PeerConnectNormalDataIterator();
public:
	virtual bool IsConnectPeer();  //this Function is to juge that connection weather connection succuse!
	virtual bool SetDataSwitch(DataRepositoryBase * res, bool value) ;
	virtual bool SetDataSwitch(std::string& guid, bool value);
	virtual void FitOnDataRespository(DataRepositoryBase * res) ;//install the DataRespository
	virtual void DischargeDataRespository(std::string &res) ;//uninstall the DataRespository
	virtual void AttachConductor(ConductorBase * conductor) ;
	virtual void UnAttachConductor();
	virtual bool Push(std::string& resGuid, void * data) ;//default push the data to top of the datarepository
	virtual void* Pop(std::string& resGuid) ; //default pop the top of the datarepository
	virtual double GetResBlockSize(std::string& resGuid);
	virtual void SetResBlockSize(std::string& resGuid, double count);
	virtual TransferProgressPacket* ResProgressDisk(std::string& resGuid);
	//
	virtual std::string& CreateDataRepository(ResDescriptionBase &resdes, DataRepositoryBase * data) ;
	virtual bool  DeleteDataRepository(std::string & guid);
	virtual std::string& GetResourceMd5(std::string &resGuid);
	virtual DataRepositoryBase * GetRespositoryByGUID(std::string& guid);
protected:
	ContainerInfoShaowBase *m_dataBox;//the data box first param is GUID string ,second param is the guid code's ower datarepository
	ConductorBase * m_hostConnection = NULL;
	STLStringShaow *m_tempStr;
};

DllExtern_Fixed_Interface(PeerConnectIterator)
////////
class WEBRTCP2PDLLSHARED_EXPORT DataRepositoryBase
{
public:
	virtual ~DataRepositoryBase() {}
public:
	virtual void CreateGUUIDCross() = 0;
	virtual void Attach(PeerConnectIterator * connection) = 0;
	virtual void UnAttach(PeerConnectIterator * connection) = 0;
	virtual void PushBack(void *data) = 0;
	virtual void* PopUp() = 0;
	virtual double GetResBlockSize() = 0;
	virtual void SetResBlockSize(double count) = 0;
	virtual TransferProgressPacket* ResProgressDisk() = 0;
	virtual void Clear() = 0; // clear the Responsitory , one time
	virtual std::string& Create(ResDescriptionBase * res) = 0;
	//resource MD5 value
	virtual void StartCaculateMd5() = 0;
	virtual unsigned int  __stdcall  CaculateMd5() = 0;//this faction need child class to  inherit and override write it
	virtual std::string& GetResourceMd5() = 0;
	//
	virtual int GetSwitchState() = 0;
	virtual void SetSwitchState(RespositorySwitchState state) = 0;
	virtual std::string& GetGUID()  = 0;
	virtual void SetGUID(const std::string &guid) = 0;
	virtual PeerConnectIterator * GetOwerConnection() = 0;
	virtual bool Equal(DataRepositoryBase *value) = 0;
protected:
	virtual void ClearCaculateHandle() = 0;
};


class WEBRTCP2PDLLSHARED_EXPORT DataRepositoryBaseExe:public DataRepositoryBase
{
public:
	explicit DataRepositoryBaseExe();
	virtual ~DataRepositoryBaseExe();
public:
	virtual void CreateGUUIDCross();
	virtual void Attach(PeerConnectIterator * connection) = 0;
	virtual void UnAttach(PeerConnectIterator * connection) = 0;
	virtual void PushBack(void *data) = 0;
	virtual void* PopUp() = 0;
	virtual double GetResBlockSize() = 0;
	virtual void SetResBlockSize(double count) = 0;
	virtual TransferProgressPacket* ResProgressDisk() = 0;
	virtual void Clear() = 0; // clear the Responsitory , one time
	virtual std::string& Create(ResDescriptionBase * res) = 0;
	//resource MD5 value
	virtual void StartCaculateMd5();
	virtual unsigned int  __stdcall  CaculateMd5()  = 0;//this faction need child class to  inherit and override write it
	virtual std::string& GetResourceMd5();
	//
	virtual int GetSwitchState();
	virtual void SetSwitchState(RespositorySwitchState state);
	virtual std::string& GetGUID();
	virtual void SetGUID(const std::string &guid);
	virtual PeerConnectIterator * GetOwerConnection();
public:
	virtual bool Equal(DataRepositoryBase *value);
protected:
	void ClearCaculateHandle();
protected:
	PeerConnectIterator * m_ower;
	//GUID m_resID;//the repository ID
	STLStringShaow * m_resGuid;
	STLStringShaow * m_resMd5;
	RespositorySwitchState m_switchState;
	void *m_caculateMd5Thread;//caculate md5 thread's handle
};

//Audio repository
class WEBRTCP2PDLLSHARED_EXPORT DataRepositoryAudio :public DataRepositoryBaseExe
{

};

///////Video repository
class WEBRTCP2PDLLSHARED_EXPORT DataRepositoryVideo :public DataRepositoryBaseExe
{

};


///////normal Data repository
class WEBRTCP2PDLLSHARED_EXPORT DataRepositoryFile :public DataRepositoryBaseExe
{
public:
	DataRepositoryFile();
	DataRepositoryFile(const std::string  &filepath);
	virtual ~DataRepositoryFile();
public:
	virtual void Attach(PeerConnectIterator * connection) override;
	virtual void UnAttach(PeerConnectIterator * connection) override;
	virtual void PushBack(void *data)override;
	virtual void* PopUp()override;
	virtual double GetResBlockSize() = 0;
	virtual void  SetResBlockSize(double count) = 0;
	virtual TransferProgressPacket* ResProgressDisk() = 0;
	virtual void SetFilePath(std::string& path) = 0;
	virtual void Clear()override; // clear the Responsitory , one time
    //
	virtual unsigned int  __stdcall  CaculateMd5() = 0;
	//
	virtual std::string& Create(ResDescriptionBase * res) = 0;
	virtual std::string& GetFilePath() = 0;
protected:
	STLStringShaow *m_filepath;
	STLStringShaow *m_fileMD5;//The md5 value of the file 
};


class WEBRTCP2PDLLSHARED_EXPORT P2PFileDataRepositorySender :public DataRepositoryFile
{
public:
	P2PFileDataRepositorySender(std::string & filepath);
	~P2PFileDataRepositorySender();
public:
	virtual void* PopUp()override;
	virtual void Clear()override;
public:
	virtual std::string& Create(ResDescriptionBase * res) override;
	virtual unsigned int  __stdcall  CaculateMd5()override;
	virtual TransferProgressPacket* ResProgressDisk();
private:
	virtual void SetFilePath(std::string& path)override;
	virtual std::string& GetFilePath()override;
	virtual double GetResBlockSize();
	virtual void SetResBlockSize(double count);
private:
	ContainerInfoShaowBase * m_readFile;
	int m_sendPacketIndex;
	/*double m_fileTotalSize;
	double m_currentSendSize;*/
	TransferProgressPacket * m_progressInfo;
};

EXTERN_C_BEGIN
WEBRTCP2PDLLSHARED_EXPORT DataRepositoryBase* CreateP2PFileDataRepositorySender(std::string &);
WEBRTCP2PDLLSHARED_EXPORT void ReleaseP2PFileDataRepositorySender(DataRepositoryBase * value);
EXTERN_C_END

class WEBRTCP2PDLLSHARED_EXPORT P2PFileDataRepositoryReciever :public DataRepositoryFile
{
public:
	P2PFileDataRepositoryReciever();
	~P2PFileDataRepositoryReciever();
public:
	virtual void PushBack(void *data)override;
	virtual void Clear()override;
public:
	virtual std::string& Create(ResDescriptionBase * res) override;
private:
	virtual void CreateFileEx(std::string& filepath);
	virtual void SetFilePath(std::string& path)override;
	virtual std::string& GetFilePath()override;
	virtual unsigned int  __stdcall  CaculateMd5()override;
	virtual double GetResBlockSize();
	virtual TransferProgressPacket* ResProgressDisk();
	virtual void CombinationDataToFile();
	virtual bool WriteDataToFile(const DataChannelTransferBase<> * data);
	virtual void SetResBlockSize(double count);
private:
	/*double m_totalPackCount;
	double m_recievePackCount;*/
	TransferProgressPacket * m_progressInfo;
	ContainerInfoShaowBase * m_writeStream;
	double m_oldFinishPercent;
};

EXTERN_C_BEGIN
WEBRTCP2PDLLSHARED_EXPORT DataRepositoryBase* CreateP2PFileDataRepositoryReciever(std::string &);
WEBRTCP2PDLLSHARED_EXPORT void ReleaseP2PFileDataRepositoryReciever(DataRepositoryBase * value);
EXTERN_C_END
/////////////file name deal class 
class WEBRTCP2PDLLSHARED_EXPORT FileDealBase
{
public:
	virtual bool NewFile(std::string &path) = 0;
	virtual bool ReleaseFile(std::string &path) = 0;
	virtual std::string& RenameFile(std::string& path) = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT FileDealBaseIterator
{
public:
	FileDealBaseIterator(FileDealBase * deal);

	virtual ~FileDealBaseIterator();
public:
	virtual bool NewFile(std::string &path) = 0;
	virtual bool ReleaseFile(std::string& path) = 0;
	virtual std::string& RenameFile(std::string& path) = 0;
private:
	FileDealBase * m_fileDeal;
};

class WEBRTCP2PDLLSHARED_EXPORT FileDealFunction :public FileDealBase
{
public:
	FileDealFunction(std::string &path);
	FileDealFunction();
	virtual  ~FileDealFunction();
public:
	virtual bool NewFile(std::string& path);
	virtual bool ReleaseFile(std::string& path);
	virtual std::string& RenameFile(std::string& path);
	virtual bool IsExitFile( std::string& path);
private:
	STLStringShaow *m_filepath;
	STLStringShaow *m_tempStr;
};
///////////
class ConvertStringBase
{
public:
	virtual void *c_str() = 0;
};

class UnicodeToUtf8:public ConvertStringBase
{
public:
	UnicodeToUtf8(const wchar_t * str);
	virtual ~UnicodeToUtf8();
	virtual void * c_str();
private:
	char* m_string;
};

class UTF82WCS:public ConvertStringBase
{
public:
	UTF82WCS(const char * str);
	virtual ~UTF82WCS();
	virtual void * c_str();
private:
	wchar_t* m_string;
};
//////related the turnserver attribute set 
EXTERN_C_END
/////////
#endif
