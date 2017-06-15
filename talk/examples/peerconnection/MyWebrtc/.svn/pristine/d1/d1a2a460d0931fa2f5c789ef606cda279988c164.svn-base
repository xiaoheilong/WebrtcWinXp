#include "webrtc/mediastreaminterface.h"
#include "webrtc/peerconnectioninterface.h"
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/logging.h"
#include"TransferData.h"
#include"defaults.h"
#include"CFileMd5.h"
//#include"DllInterface.h"
#include<iostream>
using namespace std;
#include <io.h>
#include<assert.h>
#include <functional>
#include<algorithm>
#include<process.h>
#include<thread>
#include<fstream>
#include<tchar.h>
#define TRANSFER_ACK_SIZE_WINDOW  0.001
#ifdef WIN32
	#include <objbase.h>
#elif
	#include <uuid/uuid.h>
	typedef struct _GUID
	{
		unsigned long Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char Data4[8];
	} GUID, UUID;
#endif
#include<map>
namespace XGUID
{
	GUID CreateGuid()
	{
		GUID guid;
		#ifdef WIN32
			CoCreateGuid(&guid);
		#else
			uuid_generate(reinterpret_cast<unsigned char *>(&guid));
		#endif
		return guid;
	}

	std::string GuidToString(const GUID &guid)
	{
		char buf[64] = { 0 };
		#ifdef __GNUC__
				snprintf(
		#else // MSVC
				_snprintf_s(
		#endif
			buf,
			sizeof(buf),
			"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);
		return std::string(buf);
	}

	std::string CreateGuidString()
	{
		return GuidToString(CreateGuid());
	}

}//namespace
////////////////
////////////////
namespace TransferNamespace {
	///////////////////////////
	class STLStringShaowExe :public STLStringShaow
	{
	public:
		STLStringShaowExe();
		STLStringShaowExe(const std::string & value);
		STLStringShaowExe(const STLStringShaowExe & value);
		STLStringShaowExe & operator=(const STLStringShaowExe &value);
		virtual ~STLStringShaowExe();
	public:
		virtual std::string const &  GetString()const ;
		virtual void SetString(const std::string &string);
		virtual bool IsValid() ;
		virtual int compare(const STLStringShaow & str)const;
		virtual void Add(const STLStringShaow & value);
		virtual void Add(const std::string & value);
		virtual void clear();
	private:
		std::string m_string;
	};

	STLStringShaowExe::STLStringShaowExe()
	{
		m_string = "";
	}

	STLStringShaowExe::STLStringShaowExe(const std::string &value)
	{
		m_string = value.c_str();
	}

	STLStringShaowExe::STLStringShaowExe(const STLStringShaowExe & value)
	{
		m_string.clear();
		m_string = value.m_string;
	}

	STLStringShaowExe & STLStringShaowExe::operator=(const STLStringShaowExe &value)
	{
		m_string.clear();
		m_string = value.m_string;
		return (*this);
	}

	STLStringShaowExe::~STLStringShaowExe()
	{
		m_string.clear();
	}

	std::string const &  STLStringShaowExe::GetString()const
	{
		return m_string;
	}

	void STLStringShaowExe::SetString(const std::string &string)
	{
		m_string = string;
	}

	bool STLStringShaowExe::IsValid()
	{
		return !m_string.empty();
	}

	int STLStringShaowExe::compare(const STLStringShaow & str)const
	{
		return m_string.compare(str.GetString());
	}

	void STLStringShaowExe::Add(const STLStringShaow & value)
	{
		m_string += value.GetString();
	}

	void STLStringShaowExe::Add(const std::string & value)
	{
		m_string += value;
	}

	void STLStringShaowExe::clear()
	{
		m_string.clear();
	}

	STLStringShaow*  CreateSTLString()
	{
		return new STLStringShaowExe();
	}

	STLStringShaow*  CreateSTLStringEx(const std::string & string)
	{
		return new STLStringShaowExe(string);
	}

	void DeleteSTLString(STLStringShaow * ptr)
	{
		DeletePtr(ptr);
	}
	///////////////
	class ResDescriptionBaseVector:public ContainerInfoShaowBase
	{
	public:
		explicit ResDescriptionBaseVector();
		virtual ~ResDescriptionBaseVector();
	public:
		virtual void* GetInfoVector();
	protected:
		std::vector<ResDescriptionBase> m_vector;
	};

	ResDescriptionBaseVector::ResDescriptionBaseVector()
	{
		m_vector.clear();
	}

	ResDescriptionBaseVector::~ResDescriptionBaseVector()
	{
		m_vector.clear();
	}

	void* ResDescriptionBaseVector::GetInfoVector()
	{
		return &m_vector;
	}

	inline ContainerInfoShaowBase * CreateResDescriptionBaseVector()
	{
		return new ResDescriptionBaseVector();
	}

	inline void DeleteResDescriptionBaseVector(ContainerInfoShaowBase * value)
	{
		DeletePtr(value);
	}
	////////////
	class MapDataRepositoryBase :public ContainerInfoShaowBase
	{
	public:
		explicit MapDataRepositoryBase();
		virtual ~MapDataRepositoryBase();
	public:
		virtual void* GetInfoVector();
	private:
		std::map<std::string, DataRepositoryBase*>m_dataMap;
	};

	MapDataRepositoryBase::MapDataRepositoryBase()
	{
		m_dataMap.clear();
	}

	MapDataRepositoryBase::~MapDataRepositoryBase()
	{
		std::map<std::string, DataRepositoryBase*>::iterator beptr = m_dataMap.begin();
		while (beptr != m_dataMap.end())
		{
			if (beptr->second)
			{
				DeletePtr(beptr->second);//the resource should been delete 
			}
			beptr++;
		}
		m_dataMap.clear();
	}
	
	void* MapDataRepositoryBase::GetInfoVector()
	{
		return &m_dataMap;
	}
	/////////////////////
	class DataChannelTransferData :public ContainerInfoShaowBase
	{
	public:
		explicit DataChannelTransferData() { memset(&m_data, 0, sizeof(DataChannelTransferBase<>)); };
		virtual ~DataChannelTransferData() { memset(&m_data, 0, sizeof(DataChannelTransferBase<>)); };
	public:
		virtual void* GetInfoVector() { return &m_data; };
	private:
		DataChannelTransferBase<> m_data;
	};
	///////std::ifstream m_readFile
	class STLInStream:public ContainerInfoShaowBase
	{
	public:
		explicit STLInStream() { };
		virtual ~STLInStream() {  };
	public:
		virtual void* GetInfoVector() { return &m_stream; };
	private:
		std::ifstream m_stream;
	};
	////////
	class STLOutStream :public ContainerInfoShaowBase
	{
	public:
		explicit STLOutStream() { };
		virtual ~STLOutStream() {  };
	public:
		virtual void* GetInfoVector() { return &m_stream; };
	private:
		std::ofstream m_stream;
	};
	//////
	#define HTHREAD std::thread;
}
#define CreateSTLString TransferNamespace::CreateSTLString 
#define CreateSTLStringEx TransferNamespace::CreateSTLStringEx
#define DeleteSTLString TransferNamespace::DeleteSTLString
////////////
void FilePathParam::Set(const DWORD &suserid, const DWORD &ruserid)
{
	SetSuserID(suserid);
	SetRuserID(ruserid);
}

void  FilePathParam::Set(const FilePathParam &value)
{
	Set(value.sUserID, value.rUserID/*, value.filepath*/);
}

FilePathParam &  FilePathParam::operator=(const FilePathParam &value)
{
	Set(value);
	return (*this);
}

void FilePathParam::SetSuserID(DWORD id)
{
	sUserID = id;
}

void FilePathParam::SetRuserID(DWORD id)
{
	rUserID = id;
}
///////////////////
void FileResourceParam::Set(const FilePathParam &value, const char * sGuid, const char * rGuid)
{
	filepath.Set(value);
	SetSuserResGuid(sGuid);
	SetRuserResGuid(rGuid);
}

void FileResourceParam::SetSuserResGuid(const char *str)
{
	memset(sUserResGuid, 0, 40);
	if (str)
	{
		//不馆str是否超出了40个的长度，只是截取至多39个字符，
		size_t strLength = strlen(str);
		if (strLength > 40)
			strLength = 39;
		memcpy(sUserResGuid, str, strLength);
		sUserResGuid[strLength] = '\0';
	}
}

void FileResourceParam::SetRuserResGuid(const char * str)
{
	memset(rUserResGuid, 0, 40);
	if (str)
	{
		size_t strLength = strlen(str);
		if (strLength > 40)
		{
			strLength = 40;
		}

		memcpy(rUserResGuid, str, strLength);
		rUserResGuid[strLength] = '\0';
	}
}

FileResourceParam & FileResourceParam::operator=(const FileResourceParam & value)
{
	Set(value.filepath, value.sUserResGuid, value.rUserResGuid);
	return (*this);
}
////////////////////////
void FileDataBase::Set(const FileDataBase & pack)
{
	message_id = pack.message_id;
	owerInfo = pack.owerInfo;
}

FileDataBase::FileDataBase()
{
	message_id = -1;
}

FileDataBase & FileDataBase::operator=(const FileDataBase & value)
{
	Set(value);
	return (*this);
}

FileDataBase* FileDataBase::operator=(const FileDataBase * value)
{
	if (!value)
		return NULL;
	Set(*value);
	return (this);
}

int  FileDataBase::Serialize(uint8_t* out, int size)
{
	if (!out)
		return 0;
	memset(out, 0, size);
	int messageidSize = sizeof(DWORD);
	int owerInfoSize = sizeof(FileResourceParam);
	int sizeFileDataBase = messageidSize + owerInfoSize;
	int sizeType = sizeof(PacketType);
	int sizeLength = sizeof(int);
	int sizeHeader = sizeType + sizeLength;
	if (size < (sizeHeader + sizeFileDataBase))
		return 0;
	PacketType type = PacketType::FILEDATABASE;
	memcpy(out, &type, sizeType);
	memcpy(out + sizeType, &sizeFileDataBase, sizeLength);
	memcpy(out + sizeHeader, &message_id, messageidSize);
	memcpy(out + sizeHeader + messageidSize, &owerInfo, owerInfoSize);
	return sizeHeader + sizeFileDataBase;
}

int FileDataBase::Deserialize(uint8_t * in, int size)
{
	int sizeFileDataBase = 0;
	int sizeHeader = sizeof(PacketType) + sizeof(int);
	int messageidSize = sizeof(DWORD);
	int owerInfoSize = sizeof(FileResourceParam);
	if (!in || size < (sizeHeader + messageidSize + owerInfoSize))
		return 0;
	memcpy(&sizeFileDataBase, in + sizeof(PacketType), sizeof(int));
	memcpy(&message_id, in + sizeHeader, messageidSize);
	if (sizeFileDataBase != (messageidSize + owerInfoSize))
		return 0;
	memcpy(&owerInfo, in + sizeHeader + messageidSize, owerInfoSize);
	return sizeHeader + sizeFileDataBase;
}

////////////////////////
void FileDataPack::Set(const FileDataPack & pack)
{
	*data = *(pack.data);
    FileDataBase::Set(pack);
}

FileDataPack::FileDataPack()
{
	data = new DataChannelTransferBase<>();
    if(data != NULL)
        data->SetContent("");
}

FileDataPack::FileDataPack(const FileDataPack & pack)
{
	if(!data)
		data = new DataChannelTransferBase<>();
	*this = pack;
}

FileDataPack::~FileDataPack()
{
	DeleteContainer(data);
}

FileDataPack& FileDataPack::operator=(const FileDataPack & value)
{
	Set(value);//this is will be influnce efficiency
	return(*this);
}

FileDataPack* FileDataPack::operator=(const FileDataPack * value)
{
	if (!value)
		return NULL;
	Set(*value);
	return this;
}

int  FileDataPack::Serialize(uint8_t* out,int size)
{
	if (!out)
		return 0;
	memset(out, 0, size);
	int messageidSize = sizeof(DWORD);
	int owerInfoSize = sizeof(FileResourceParam);
	int sizeFileDataBase = messageidSize + owerInfoSize;
	int sizeFileDataChannel = sizeof(DataChannelTransferBase<>);
	int sizeType = sizeof(PacketType);
	int sizeLength = sizeof(int);
	int sizeHeader = sizeType + sizeLength;
	int sizeValueLength = sizeFileDataBase + sizeFileDataChannel;
	if (size < (sizeHeader + sizeValueLength))
		return 0;
	PacketType type = PacketType::FILEDATABASE;
	memcpy(out, &type, sizeType);
	memcpy(out + sizeType, &sizeValueLength,sizeLength);
	//this->message_id;
	//this->owerInfo;
	memcpy(out + sizeHeader, &message_id, messageidSize);
	memcpy(out + sizeHeader + messageidSize, &owerInfo, owerInfoSize);
	if (data)
		memcpy(out + sizeHeader + sizeFileDataBase, data, sizeFileDataChannel);
	return sizeHeader + sizeValueLength;
}

int FileDataPack::Deserialize(uint8_t  * in, int size)
{
	if (!in)
		return 0;
	int messageidSize = sizeof(DWORD);
	int owerInfoSize = sizeof(FileResourceParam);
	int sizeFileDataBase = messageidSize + owerInfoSize;
	int sizeType = sizeof(PacketType);
	int sizeLength = sizeof(int);
	int sizeHeader = sizeType + sizeLength;
	int sizeFileDataChannel = sizeof(DataChannelTransferBase<>);
	memcpy(&message_id, in + sizeHeader, messageidSize);//must copy the message_id
	if (size < (sizeHeader + sizeFileDataBase + sizeFileDataChannel))
		return 0;
	memcpy(&owerInfo, in + sizeHeader + messageidSize,  owerInfoSize);

	memcpy(data, in + sizeHeader + sizeFileDataBase, sizeFileDataChannel);
	return sizeHeader + sizeFileDataBase + sizeFileDataChannel;
}
//////////////////////
ResDescriptionBase::ResDescriptionBase()
{
	type = Empty_Type;
	guid = CreateSTLString();
}

ResDescriptionBase::~ResDescriptionBase()
{
	DeleteSTLString(guid);
}

ResDescriptionBase::ResDescriptionBase(const ResDescriptionBase & value)
{
	type = Empty_Type;
	guid = CreateSTLString();
	*this = value;
}

bool ResDescriptionBase::operator <(const ResDescriptionBase &value)const
{
	if (guid && value.guid)
	{
		if (guid->compare(*(value.guid)) < 0)
			return true;
	}
	return false;
}


bool ResDescriptionBase::operator==(const ResDescriptionBase &value)const
{
	if (guid && value.guid)
	{
		if (guid->compare(*(value.guid)) == 0)
			return true;
	}
	return false;
}

bool ResDescriptionBase::operator >(const ResDescriptionBase &value)const
{
	if (guid && value.guid)
	{
		if (guid->compare(*(value.guid)) > 0)
			return true;
	}
	return false;
}

ResDescriptionBase & ResDescriptionBase::operator =(const ResDescriptionBase & value)
{
	if (this == &value)
		return (*this);
	type = value.type;
	if (!guid)
	{
		guid = CreateSTLString();
	}
	if (value.guid)
		guid->SetString(value.guid->GetString());
	else
	{
		guid->SetString("");
	}
	return (*this);
}
/////////////////
FileResDescription::FileResDescription():ResDescriptionBase()
{
	filepath = CreateSTLString();
}

FileResDescription::~FileResDescription()
{
	DeleteSTLString(filepath);
}

FileResDescription::FileResDescription(const ResDescriptionBase & value)
{
	filepath = CreateSTLString();
}

bool FileResDescription::operator <(const FileResDescription &value)const
{
	return  ResDescriptionBase::operator<(value);
}

bool FileResDescription::operator ==(const FileResDescription &value)const
{
	bool returnValue = ResDescriptionBase::operator==(value);
	if (!returnValue)
		return returnValue;
	if (!filepath)
		return false;
	return filepath->compare(*(value.filepath));
}

bool FileResDescription::operator >(const FileResDescription &value)const
{
	return ResDescriptionBase::operator>(value);
}

FileResDescription & FileResDescription::operator=(const FileResDescription & value)
{
	ResDescriptionBase::operator>(value);
	if (filepath)
	{
		if (value.filepath)
			filepath->SetString(value.filepath->GetString());
	}
	return *this;
}

///////////////////
TransferProgressPacket::TransferProgressPacket(const std::string & id, double totalSize, double finishSize)
{
	guid = CreateSTLString();
	guid->SetString(id);
	totalTaskSize = totalSize;
	finishTaskSize = finishSize;
}

TransferProgressPacket::~TransferProgressPacket()
{
	DeleteSTLString(guid);
	totalTaskSize = 0.0;
	finishTaskSize = 0.0;
}

TransferProgressPacket::TransferProgressPacket(const TransferProgressPacket & value)
{
	guid = CreateSTLString();
	*this = value;
}

TransferProgressPacket::TransferProgressPacket()
{
	guid = CreateSTLString();
	totalTaskSize = 0.0;
	finishTaskSize = 0.0;
}

void TransferProgressPacket::Clear()
{
	totalTaskSize = 0.0;
	finishTaskSize = 0.0;
	guid->clear();
}

TransferProgressPacket & TransferProgressPacket::operator=(const TransferProgressPacket & value)
{
	if(!guid)
		guid = CreateSTLString();
	if (value.guid)
		guid->SetString(value.guid->GetString());
	else
		guid->SetString("");
	totalTaskSize = value.totalTaskSize;
	finishTaskSize = value.finishTaskSize;
	return *this;
}

TransferProgressPacket * TransferProgressPacket::operator=(const TransferProgressPacket * value)
{
	if (!guid)
		guid = CreateSTLString();
	if (!value)
	{
		guid->SetString("");
		totalTaskSize = 0.0;
		finishTaskSize = 0.0;
	}
	else
		*this = *value;
	return this;
}
//////////////
//////////
ServerInfo::ServerInfo()
{
	address = CreateSTLString();
	port = CreateSTLString();
	username = CreateSTLString();
	combination = CreateSTLString();
}

ServerInfo::ServerInfo(const ServerInfo & value)
{
	address = CreateSTLString();
	port = CreateSTLString();
	username = CreateSTLString();
	combination = CreateSTLString();
	*this = value;
}

ServerInfo::ServerInfo(const std::string &a, const std::string &p, const std::string &u)
{
	address = CreateSTLStringEx(a);
	port = CreateSTLStringEx(p);
	username = CreateSTLStringEx(u);
	combination = CreateSTLString();
}

ServerInfo::~ServerInfo()
{
	DeleteSTLString(address);
	DeleteSTLString(port);
	DeleteSTLString(username);
	DeleteSTLString(combination);
}

ServerInfo& ServerInfo::operator=(const ServerInfo & value)
{
	if (!address)
		address = CreateSTLString();
	if (!port)
		port = CreateSTLString();
	if (!username)
		username = CreateSTLString();
	if (!combination)
		combination = CreateSTLString();
	if (value.address)
		address->SetString((value.address->GetString()));//const_cast<std::string &>
	if (value.port)
		port->SetString(value.port->GetString());
	if (value.username)
		username->SetString(value.username->GetString());
	if (value.combination)
		combination->SetString(value.combination->GetString());
	return *this;
}

bool ServerInfo::operator<(const ServerInfo & value)const
{
	if (address)
		return address->compare(*value.address);
	else
		return false;
}

bool ServerInfo::operator==(const ServerInfo &value)const
{
	int result = 1;
	if (address)
	{
		result = address->compare(*value.address);
		if (0 == result && port)
		{
			result = port->compare(*value.port);
			if (0 == result && username)
			{
				result = username->compare(*value.username);
				if (0 == result && combination)
				{
					result = combination->compare(*value.combination);
					return result == 0 ? true : false;
				}
			}
		}
	}
	return false;
}

void ServerInfo::SetAddress(const std::string &a)
{
	address->SetString(a);
}

void ServerInfo::SetPort(const std::string &p)
{
	port->SetString(p);
}

void ServerInfo::SetUserName(const std::string &u)
{
	username->SetString(u);
}

std::string& ServerInfo::GetAddress()
{
	return const_cast<std::string &>(address->GetString());
}

std::string& ServerInfo::GetPort()
{
	return const_cast<std::string&>(port->GetString());
}

std::string& ServerInfo::GetAddressPort()
{
	combination->SetString("");
	combination->Add(*address);
	combination->Add(":");
	combination->Add(*port);
	return const_cast<std::string &>(combination->GetString());
}

std::string& ServerInfo::GetUserNameEx()
{
	return const_cast<std::string &>(username->GetString());
}
////////////
////////////
TurnserverInfo::TurnserverInfo()
{
	password = CreateSTLString();
	realm = CreateSTLString();
}

TurnserverInfo::TurnserverInfo(const std::string &a, const std::string &p, const std::string & u,
	const std::string &pwd, const std::string &r) :ServerInfo(a, p, u)
{
	password = CreateSTLStringEx(pwd);
	realm = CreateSTLStringEx(r);
}
TurnserverInfo::~TurnserverInfo()
{
	DeleteSTLString(password);
	DeleteSTLString(realm);
}

bool TurnserverInfo::operator<(const TurnserverInfo & value)const
{
	return ServerInfo::operator<(value);
}

bool TurnserverInfo ::operator==(const TurnserverInfo & value)const
{
	int result = ServerInfo::operator==(value)? 0:1;
	if (0 == result && password)
	{
		result = password->compare(*value.password);
		if (0 == result && realm)
		{
			result = realm->compare(*value.realm);
			return result ==0 ?true:false;
		}
	}
	return false;
}

TurnserverInfo::TurnserverInfo(const TurnserverInfo &value)
{
	password = CreateSTLString();
	realm = CreateSTLString();
	*this = value;
}

TurnserverInfo & TurnserverInfo::operator=(const TurnserverInfo & value)
{
    ServerInfo::operator=(value);
	if (!value.password)
		password = CreateSTLStringEx(value.password->GetString());
	else
		password->SetString(value.password->GetString());
	if (!value.realm)
		realm = CreateSTLStringEx(value.realm->GetString());
	else
		realm->SetString(value.realm->GetString());
	return *this;
}

void  TurnserverInfo::SetPassword(const std::string &p)
{
	if (password)
		password->SetString(p);
}

void TurnserverInfo::SetReaml(const std::string &r)
{
	if (realm)
		realm->SetString(r);
}

std::string& TurnserverInfo::GetPassword()
{
		return const_cast<std::string&>(password->GetString());
}

std::string& TurnserverInfo::GetRealm()
{
	return const_cast<std::string &>(realm->GetString());
}
/////////////////
WEBRTCP2PDLLSHARED_EXPORT TurnserverInfo * CreateTurnserverInfo(const std::string &a, const std::string &p, const std::string & u,
	const std::string &pwd, const std::string &r)
{
	return new TurnserverInfo(a,p, u, pwd, r);
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseTurnserverInfo(TurnserverInfo * value)
{
	DeletePtr(value);
}

///////////////
ConductorBase::~ConductorBase()
{

}
///////////////
ConductorPeerConnection::ConductorPeerConnection(PeerConnectIterator * iterator)
{
	m_resourceIterator = NULL;
	m_resVector = CreateContainer(TransferNamespace::ResDescriptionBaseVector);
	AttachDataLine(iterator);
	m_md5 = CreateSTLString();
	m_tempStr = CreateSTLString();
}

ConductorPeerConnection::~ConductorPeerConnection()
{
	DeleteContainer(m_resVector);
	UnAttachDataLine();
	DeleteSTLString(m_md5);
	DeleteSTLString(m_tempStr);
	m_resourceIterator = NULL;
}

void * ConductorPeerConnection::ExtractData(std::string& resGuid)
{
	if (m_resourceIterator)
	{
		return m_resourceIterator->Pop(resGuid);
	}
	return NULL;
}

double ConductorPeerConnection::GetResBlockSize(std::string& resGuid)
{
	if (m_resourceIterator)
	{
		return m_resourceIterator->GetResBlockSize(resGuid);
	}
	return 0;
}

void ConductorPeerConnection::SetResBlockSize(std::string& resGuid, double count)
{
	if (m_resourceIterator)
	{
		m_resourceIterator->SetResBlockSize(resGuid,count);
	}
}

TransferProgressPacket* ConductorPeerConnection::ResProgressDisk(std::string& resGuid)
{
	if (m_resourceIterator)
	{
		return m_resourceIterator->ResProgressDisk(resGuid);
	}
	return 0;
}

bool ConductorPeerConnection::Save(std::string& resGuid, void * value)
{
	if (m_resourceIterator)
	{
		return m_resourceIterator->Push(resGuid, value);
	}

	return false;
}


std::string& ConductorPeerConnection::CreateDataRepository(ResDescriptionBase &resdes, DataRepositoryBase * data)
{
	if (!m_tempStr)
		m_tempStr = CreateSTLString();
	m_tempStr->clear();
	if (m_resourceIterator)
	{
		m_tempStr->SetString(m_resourceIterator->CreateDataRepository(resdes, data));
		if (resdes.guid)
		{
			resdes.guid->SetString(m_tempStr->GetString());
			if (m_resVector)
			{
				std::vector<ResDescriptionBase>* vector = reinterpret_cast<std::vector<ResDescriptionBase>*>(m_resVector->GetInfoVector());
				if (vector)
					vector->push_back(resdes);
			}
		}
	}
	return const_cast<std::string &>(m_tempStr->GetString());
}


void ConductorPeerConnection::AttachDataLine(PeerConnectIterator * dataline)
{
	UnAttachDataLine();//before attach new dataline ,should UnAttach the old one ,so the can make it safe
	m_resourceIterator = dataline;
	if (m_resourceIterator)
		m_resourceIterator->AttachConductor(this);
}

void ConductorPeerConnection::UnAttachDataLine()
{
	if (m_resourceIterator)
	{
		m_resourceIterator->UnAttachConductor();
		ReleasePeerConnectIterator(m_resourceIterator);
	}
}

bool ConductorPeerConnection::IsExitRes(std::string& guid)
{
	std::vector<ResDescriptionBase>* vector = NULL;
	if (m_resVector)
	{
		vector = reinterpret_cast<std::vector<ResDescriptionBase>*>(m_resVector->GetInfoVector());
	}
	if (!vector)
		return false;
	////////////
	std::vector<ResDescriptionBase>::iterator beptr = std::find_if(vector->begin(), vector->end(),
		[&guid](const ResDescriptionBase &value)->bool
	{
		if (value.guid)
		{
			if (guid.compare(value.guid->GetString()) == 0)
				return true;
		}
		return false;
	});
	if (beptr == vector->end())
		return false;
	return true;
}

std::string& ConductorPeerConnection::GetResourceMd5(std::string& resGuid)
{
	std::vector<ResDescriptionBase>* vector = NULL;
	if (m_resVector)
	{
		vector = reinterpret_cast<std::vector<ResDescriptionBase>*>(m_resVector->GetInfoVector());
	}
	if (!vector)
	{
		if (!m_md5)
			m_md5 = CreateSTLString();
		return const_cast<std::string &>(m_md5->GetString());
	}
	if (!m_md5)
		m_md5 = CreateSTLString();
	m_md5->clear();
	if (!m_resourceIterator)
		return const_cast<std::string &>(m_md5->GetString());
	if (vector->begin() == vector->end())
		return const_cast<std::string &>(m_md5->GetString());
	return m_resourceIterator->GetResourceMd5(resGuid);
}

DataRepositoryBase * ConductorPeerConnection::GetRespositoryByGUID(std::string& guid)
{
	if (m_resourceIterator)
	{
		return m_resourceIterator->GetRespositoryByGUID(guid);
	}

	return NULL;
}

bool  ConductorPeerConnection::DeleteDataRepository(std::string & guid)
{
	if (m_resourceIterator)
	{
		return m_resourceIterator->DeleteDataRepository(guid);
	}
	return false;
}
//////////////////////
WEBRTCP2PDLLSHARED_EXPORT ConductorBase* CreateConductorBase(ConductorBase * value)
{
	return value;
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseConductorBase(ConductorBase * value)
{
	DeletePtr(value);
}
////////////////////////////////////////////////////////
PeerConnectIterator::~PeerConnectIterator()
{

}

/////////
PeerConnectNormalDataIterator::PeerConnectNormalDataIterator()
{
	m_dataBox = CreateContainer(TransferNamespace::MapDataRepositoryBase);//the data box first param is GUID string ,second param is the guid code's ower datarepository
	m_hostConnection = NULL;
	m_tempStr = CreateSTLString();
}

PeerConnectNormalDataIterator::~PeerConnectNormalDataIterator()
{
	DeleteContainer(m_dataBox);
	DeleteSTLString(m_tempStr);
}

bool PeerConnectNormalDataIterator::IsConnectPeer()
{

	return true;
}

bool PeerConnectNormalDataIterator::SetDataSwitch(DataRepositoryBase * res, bool value)
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (!res || !dataMap)
		return false;
	std::map<std::string, DataRepositoryBase*>::iterator ptr = dataMap->begin();
	while (ptr != dataMap->end())
	{
		if (ptr->second->Equal(res))
			break;
		ptr++;
	}
	if (ptr == dataMap->end())
		return false;
	if (ptr->second)
		ptr->second->SetSwitchState((RespositorySwitchState)value);
	return true;
}

bool PeerConnectNormalDataIterator::SetDataSwitch(std::string &guid, bool value)
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (guid.empty() || !dataMap)
		return false;
	std::map<std::string, DataRepositoryBase*>::iterator ptr = dataMap->find(guid);
	if (ptr == dataMap->end())
		return false;
	if (ptr->second)
		ptr->second->SetSwitchState((RespositorySwitchState)value);
	return true;
}

void PeerConnectNormalDataIterator::FitOnDataRespository(DataRepositoryBase * res)
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (res && dataMap)
		dataMap->insert(make_pair(res->GetGUID(), res));
}

void PeerConnectNormalDataIterator::DischargeDataRespository(std::string  &res)//res is the guid of the repository
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (!dataMap)
		return;
	std::map<std::string, DataRepositoryBase*>::iterator ptr = dataMap->find(res);
	//if find the repository ,then erase this res
	if (ptr != dataMap->end())
	{
		//the repository is should been delete 
		DeletePtr(ptr->second);
		dataMap->erase(ptr);
	}
}

void PeerConnectNormalDataIterator::AttachConductor(ConductorBase * conductor)
{
	m_hostConnection = conductor;
}

void PeerConnectNormalDataIterator::UnAttachConductor()
{
	m_hostConnection = NULL;
}

bool PeerConnectNormalDataIterator::Push(std::string &resGuid, void * data)
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	//not allow the null ptr,because the null ptr is invalid
	if (!data || !dataMap)
		return false;
	std::map<std::string, DataRepositoryBase*>::iterator beptr = dataMap->find(resGuid);
	//if the guid is areadly exist，just push_back it, otherwise not push the data
	if (beptr != dataMap->end())
	{
		(*dataMap)[resGuid]->PushBack(data);
		return true;
	}
	return false;
}

void*  PeerConnectNormalDataIterator::Pop(std::string& resGuid)
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (!dataMap)
		return NULL;
	std::map<std::string, DataRepositoryBase*>::iterator beptr = dataMap->find(resGuid);
	if (beptr != dataMap->end())
	{
		return (*dataMap)[resGuid]->PopUp();
	}

	return NULL;
}

double PeerConnectNormalDataIterator::GetResBlockSize(std::string& resGuid)
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (!dataMap)
		return 0.0;
	std::map<std::string, DataRepositoryBase*>::iterator beptr = dataMap->find(resGuid);
	if (beptr != dataMap->end())
	{
		return (*dataMap)[resGuid]->GetResBlockSize();
	}
	return 0;
}

void PeerConnectNormalDataIterator::SetResBlockSize(std::string& resGuid, double count)
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (!dataMap)
		return ;
	std::map<std::string, DataRepositoryBase*>::iterator beptr = dataMap->find(resGuid);
	if (beptr != dataMap->end())
	{
		(*dataMap)[resGuid]->SetResBlockSize(count);
	}
}

TransferProgressPacket* PeerConnectNormalDataIterator::ResProgressDisk(std::string& resGuid)
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (!dataMap)
		return NULL;
	std::map<std::string, DataRepositoryBase*>::iterator beptr = dataMap->find(resGuid);
	if (beptr != dataMap->end())
	{
		return (*dataMap)[resGuid]->ResProgressDisk();
	}
	return NULL;
}

std::string&  PeerConnectNormalDataIterator::CreateDataRepository(ResDescriptionBase &resdes, DataRepositoryBase * data)
{
	if (!m_tempStr)
		m_tempStr = CreateSTLString();
	m_tempStr->clear();
	if (data)
	{
		m_tempStr->SetString(data->Create(&resdes));
		FitOnDataRespository(data);
	}
	return const_cast<std::string &>(m_tempStr->GetString());
}

std::string& PeerConnectNormalDataIterator::GetResourceMd5(std::string& resGuid)
{
	if (!m_tempStr)
		m_tempStr = CreateSTLString();
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (!dataMap)
		m_tempStr->GetString();
	DataRepositoryBase* res = (*dataMap)[resGuid];
	m_tempStr->clear();
	if (res)
	{
		res->StartCaculateMd5();//open a thread to caculate the md5 of current resource ,then call GetResourceMd5 ,return
		//to  calledr
		m_tempStr->SetString(res->GetResourceMd5());
	}
	return const_cast<std::string &>(m_tempStr->GetString());
}

DataRepositoryBase * PeerConnectNormalDataIterator::GetRespositoryByGUID(std::string &guid)
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (!dataMap)
		return NULL;
	if (!dataMap->empty())
	{
		std::map<std::string, DataRepositoryBase *>::const_iterator iter = dataMap->find(guid);
		if (iter != dataMap->end())
			return (*dataMap)[guid];
	}
	return NULL;
}


bool PeerConnectNormalDataIterator::DeleteDataRepository(std::string & guid)
{
	std::map<std::string, DataRepositoryBase*>* dataMap = reinterpret_cast<std::map<std::string, DataRepositoryBase*>*>
		(m_dataBox->GetInfoVector());
	if (!dataMap)
		return false;
	if (!dataMap->empty())
	{
		std::map<std::string, DataRepositoryBase *>::iterator iter = dataMap->find(guid);
		if (iter != dataMap->end())
		{
			///delete the DataRepositoryBase
			DeletePtr(iter->second);
			dataMap->erase(iter);
			return true;
		}
	}
	return false;
}
/////////////////
WEBRTCP2PDLLSHARED_EXPORT PeerConnectIterator* CreatePeerConnectIterator()
{
	return new PeerConnectNormalDataIterator();
}

WEBRTCP2PDLLSHARED_EXPORT void ReleasePeerConnectIterator(PeerConnectIterator * value)
{
	DeletePtr(value);
}
///////////
DataRepositoryBaseExe::DataRepositoryBaseExe()
{
	m_ower = NULL;
	//m_resID = {0};
	m_switchState = Close_State;
	m_caculateMd5Thread = NULL;
	m_resGuid = CreateSTLString();
	m_resMd5  = CreateSTLString();
}

DataRepositoryBaseExe::~DataRepositoryBaseExe()
{
	ClearCaculateHandle();
	DeleteSTLString(m_resGuid);
	DeleteSTLString(m_resMd5);
}

void DataRepositoryBaseExe::CreateGUUIDCross()
{
	m_resGuid->SetString(XGUID::CreateGuidString().c_str()); //save the guid string value
}

int DataRepositoryBaseExe::GetSwitchState()
{
	return m_switchState;
}

void DataRepositoryBaseExe::SetSwitchState(RespositorySwitchState state)
{
	m_switchState = state;
}

std::string& DataRepositoryBaseExe::GetGUID()
{
	return const_cast<std::string &>(m_resGuid->GetString());
}

void  DataRepositoryBaseExe::SetGUID(const std::string &guid)
{
	m_resGuid->SetString(guid.c_str());
}

PeerConnectIterator * DataRepositoryBaseExe::GetOwerConnection()
{
	return m_ower;
}

void DataRepositoryBaseExe::StartCaculateMd5()
{
	//ThreadConvertBase<DataRepositoryBaseExe>  convertFun(&DataRepositoryBaseExe::CaculateMd5);
	//std::thread * ptr = new std::thread(convertFun(), this);
	//m_caculateMd5Thread = (void *)(ptr);
	//if (!m_caculateMd5Thread)
	//{
	//	DeletePtr(ptr);
	//	return;
	//}
	//ptr->detach();
	CaculateMd5();
}

std::string & DataRepositoryBaseExe::GetResourceMd5()
{
	//直到m_caculateThreadHandle被CloseHandle()掉，不然，一直堵塞在这里
	//while (m_caculateMd5Thread);
	return const_cast<std::string &>(m_resMd5->GetString());
}

void DataRepositoryBaseExe::ClearCaculateHandle()
{
	if (m_caculateMd5Thread)
	{
		std::thread * ptr = static_cast<std::thread *>(m_caculateMd5Thread);
		DeletePtr(ptr);
		m_caculateMd5Thread = NULL;
	}
}
////////////////////////////
DataRepositoryFile::DataRepositoryFile()
{
	this->m_ower = NULL;
	//this->m_resID = {0};
	this->m_switchState = Close_State;
	m_filepath = CreateSTLString();
	m_fileMD5 = CreateSTLString();
}

DataRepositoryFile::DataRepositoryFile(const std::string & filepath)
{
	this->m_ower = NULL;
	//this->m_resID = { 0 };
	this->m_switchState = Close_State;
	m_filepath = CreateSTLStringEx(filepath);
	m_fileMD5 = CreateSTLString();
}

DataRepositoryFile::~DataRepositoryFile()
{
	this->m_ower = NULL;
	//this->m_resID = { 0 };
	this->m_switchState = Close_State;
	DeleteSTLString(m_filepath);
	DeleteSTLString(m_fileMD5);
}

void DataRepositoryFile::Attach(PeerConnectIterator * connection)
{
	//PushBack(connection);
	m_ower = connection;
}

void DataRepositoryFile::UnAttach(PeerConnectIterator * connection)
{
	if (m_ower == connection)
		m_ower = NULL;
}


void DataRepositoryFile::PushBack(void *data)
{
}

void* DataRepositoryFile::PopUp()
{
	return NULL;
}


void DataRepositoryFile::Clear()
{
}


//////////////////////
P2PFileDataRepositorySender::P2PFileDataRepositorySender(std::string &filepath):DataRepositoryFile(filepath)
{
	m_sendPacketIndex = 1; //init the send packet index initilly is 1
	/*m_fileTotalSize = 0;
	m_currentSendSize = 0;*/
	m_progressInfo = new TransferProgressPacket();
	m_readFile = CreateContainer(TransferNamespace::STLInStream);
}

P2PFileDataRepositorySender::~P2PFileDataRepositorySender()
{
	Clear();
	DeleteContainer(m_readFile);
	DeletePtr(m_progressInfo);
}

void* P2PFileDataRepositorySender::PopUp()
{
	if (!m_readFile)
		return NULL;
	std::ifstream *fileStream = reinterpret_cast<std::ifstream *>(m_readFile->GetInfoVector());//this is a dog!
	if (!fileStream)
		return NULL;
	if (!fileStream->is_open())//if the ifstream is not open succesefully, then attemp to open it 
	{
		fileStream->close();
		return NULL;
	/*	if (!m_filepath->IsValid())
			return NULL;
		wstring unicodePath = MsToUc(m_filepath->GetString());
        fileStream->open(unicodePath, std::ios::binary | std::ios::in);
		if (!fileStream->is_open())
        {
			fileStream->close();
            return NULL;
		}*/
	}
	
	if (fileStream->good())
	{
		DataChannelTransferBase<> *message = new DataChannelTransferBase<>();
		fileStream->read(message->message, message->messageSize);
		int realsize = fileStream->gcount();
		if (message->messageSize != realsize)
		{
			message->messageSize = realsize;
		}
		message->packetIndex = m_sendPacketIndex;
		m_sendPacketIndex++;
		return message;
	}
	else
	{
		fileStream->close();
		return NULL;
	}
}

void P2PFileDataRepositorySender::Clear()
{
	if (!m_filepath)
		m_filepath->clear();
	m_sendPacketIndex = 0;
	/*m_fileTotalSize = 0;
	m_currentSendSize = 0;*/
	m_progressInfo->Clear();
}

std::string& P2PFileDataRepositorySender::Create(ResDescriptionBase * res)
{
	FileResDescription * fileRes = (FileResDescription *)(res);
	if (!m_resGuid)
		m_resGuid = CreateSTLString();
	m_resGuid->clear();
	if (!fileRes)
		m_resGuid->SetString("");
	if (fileRes->filepath)
	{
		SetFilePath(const_cast<std::string &>(fileRes->filepath->GetString()));
		//CreateGUUIDCross();
		m_resGuid->SetString(fileRes->guid->GetString());
		if (m_progressInfo && m_progressInfo->guid)
		{
			m_progressInfo->guid->SetString(m_resGuid->GetString());
		}
	}
	////should open filepath first time
	if (!m_filepath->IsValid())
		m_resGuid->SetString("");
	std::ifstream *fileStream = reinterpret_cast<std::ifstream *>(m_readFile->GetInfoVector());//this is a dog!
	wstring unicodePath = MsToUc(m_filepath->GetString());
	fileStream->open(unicodePath, std::ios::binary | std::ios::in);
	if (!fileStream->is_open())
	{
		fileStream->close();
		m_resGuid->SetString("");
	}
	//not need splite The file block
	return const_cast<std::string &>(m_resGuid->GetString());
}

unsigned int  __stdcall  P2PFileDataRepositorySender::CaculateMd5()
{
	//Tian_FileMd5 test;
	WebrtcFileMd5Base * test = CreateWebrtcFileMd5Base(new WebrtcFileMd5());
	if (test)
	{
		if (!m_filepath)
			m_filepath = CreateSTLString();
		m_resMd5->SetString(test->CaculateFileMd5(const_cast<std::string &>(m_filepath->GetString())));
		ClearCaculateHandle();
		ReleaseWebrtcFileMd5Base(test);
		return 1;
	}
	ReleaseWebrtcFileMd5Base(test);
	return 0;
}

double P2PFileDataRepositorySender::GetResBlockSize()
{
	//need calculate the m_filepath content size
	if (m_filepath && !m_filepath->IsValid())
		return 0;
	ifstream infile;
	std::wstring filepathUnicode = MsToUc(m_filepath->GetString());
    infile.open(filepathUnicode, ios::binary | ios::in);
	if (!infile.is_open())
	{
		assert("P2PFileDataRepository::SplitTheFileData filepath is open failure!");
		return 0.0;
	}
	infile.seekg(0,ios::end);
	double filesize = infile.tellg();
	infile.close();// close the file which open before
	return filesize;
}

void P2PFileDataRepositorySender::SetResBlockSize(double count)
{

}

WEBRTCP2PDLLSHARED_EXPORT DataRepositoryBase* CreateP2PFileDataRepositorySender(std::string & filepath)
{
	return new P2PFileDataRepositorySender(filepath);
}
WEBRTCP2PDLLSHARED_EXPORT void ReleaseP2PFileDataRepositorySender(DataRepositoryBase * value)
{
	DeletePtr(value);
}

TransferProgressPacket* P2PFileDataRepositorySender::ResProgressDisk()
{
	//when the file ptr is go to the tail of the current file. then it read finish
	//return m_currentSendSize / m_fileTotalSize;
	return m_progressInfo;
}

void P2PFileDataRepositorySender::SetFilePath(std::string& path)
{
	if (m_filepath)
		m_filepath->SetString(path);
	//when set the m_filepath , then calculate the file total size
	//m_fileTotalSize = GetResBlockSize();
	m_progressInfo->totalTaskSize = GetResBlockSize();
}

std::string& P2PFileDataRepositorySender::GetFilePath()
{
	if (!m_filepath)
		m_filepath = CreateSTLString();
	return const_cast<std::string &>(m_filepath->GetString());
}

///////////////////
P2PFileDataRepositoryReciever::P2PFileDataRepositoryReciever()
{
	if (m_filepath)
		m_filepath->clear();
	/*m_totalPackCount = 0;
	m_recievePackCount = 0;*/
	m_progressInfo = new TransferProgressPacket();
	m_writeStream = CreateContainer(TransferNamespace::STLOutStream);
	m_oldFinishPercent = 0.0;
}

P2PFileDataRepositoryReciever::~P2PFileDataRepositoryReciever()
{
	Clear();
	DeleteContainer(m_writeStream);
	DeletePtr(m_progressInfo);
}

void P2PFileDataRepositoryReciever::PushBack(void *data)
{
	if (!m_writeStream)
		return;
	std::ofstream * outStream = reinterpret_cast<std::ofstream *>(m_writeStream->GetInfoVector());
	if (!outStream)
		return;
	if (!outStream->is_open())//if the ifstream is not open succesefully, then attemp to open it 
	{
		outStream->close();
		LOG(INFO) << __FUNCTION__ << "the file is can't been open!";
		return;
		/*if (m_filepath && !m_filepath->IsValid())
			return;
		std::wstring saveFilePath = MsToUc(m_filepath->GetString());
		outStream->open(saveFilePath.c_str(), ios::binary | ios::out | ofstream::trunc);
		if (!outStream->is_open())
		{
			return;
		}*/
	}
	DataChannelTransferBase<> *message = static_cast<DataChannelTransferBase<>*>(data);
	if (message)
	{
		outStream->write(message->message, message->messageSize);
		/*m_recievePackCount*/m_progressInfo->finishTaskSize += message->messageSize;//record currentlly have recieve packets count
	}
	else
		LOG(INFO) << __FUNCTION__ << "void * can't convert to the DataChannelTransferBase<>*";
}


std::string& P2PFileDataRepositoryReciever::Create(ResDescriptionBase * res)
{
	FileResDescription * fileRes = (FileResDescription *)(res);
	if (!fileRes)
	{
		m_resGuid->SetString("");
		return const_cast<std::string &>(m_resGuid->GetString());
	}
	//CreateGUUIDCross();
	m_resGuid->SetString(fileRes->guid->GetString());
	if (m_progressInfo && m_progressInfo->guid)
		m_progressInfo->guid->SetString(m_resGuid->GetString());
	if (fileRes->filepath)
	{
		this->CreateFileEx(const_cast<std::string &>(fileRes->filepath->GetString()));
	}
	/////open the file first
	if (m_filepath && !m_filepath->IsValid())
		m_resGuid->SetString("");
	std::ofstream * outStream = reinterpret_cast<std::ofstream *>(m_writeStream->GetInfoVector());
	std::wstring saveFilePath = MsToUc(m_filepath->GetString());
	outStream->open(saveFilePath.c_str(), ios::binary | ios::out | ofstream::trunc);
	if (!outStream->is_open())
	{
		outStream->close();
		m_resGuid->SetString("");
	}
	////
	return const_cast<std::string &>(m_resGuid->GetString());
}

void P2PFileDataRepositoryReciever::CreateFileEx(std::string& filepath)
{
	FileDealFunction filedeal;
	if (filedeal.IsExitFile(filepath))
	{
		//filepath = filedeal.RenameFile(filepath);
		filedeal.ReleaseFile(filepath);
		//clear the exited file's content
	}
	this->SetFilePath(filepath);
	filedeal.NewFile(filepath);
}

void P2PFileDataRepositoryReciever::SetFilePath(std::string& path)
{
	if (!m_filepath)
		m_filepath = CreateSTLStringEx(path);
	else
		m_filepath->SetString(path);
}

std::string& P2PFileDataRepositoryReciever::GetFilePath()
{
	if (!m_filepath)
		m_filepath = CreateSTLString();
	return const_cast<std::string &>(m_filepath->GetString());
}

unsigned int __stdcall   P2PFileDataRepositoryReciever:: CaculateMd5()
{
	//combination the all file data ,caculate the combinationly file ,compare the file's md5 with the sender's file md5
	CombinationDataToFile();
	//
	WebrtcFileMd5Base *test = CreateWebrtcFileMd5Base(new WebrtcFileMd5());
	if (test)
	{
		if (!m_filepath)
			m_filepath = CreateSTLString();
		m_resMd5->SetString(test->CaculateFileMd5(const_cast<std::string &>(m_filepath->GetString())));
		ClearCaculateHandle();
		ReleaseWebrtcFileMd5Base(test);
		return 1;
	}
	ReleaseWebrtcFileMd5Base(test);

	return 0;
}

double P2PFileDataRepositoryReciever::GetResBlockSize()
{
	return /*m_totalPackCount*/m_progressInfo->totalTaskSize;
}

TransferProgressPacket* P2PFileDataRepositoryReciever::ResProgressDisk()
{
	double currentPercent = m_progressInfo->finishTaskSize / m_progressInfo->totalTaskSize;
	if (currentPercent - m_oldFinishPercent >= TRANSFER_ACK_SIZE_WINDOW || currentPercent - m_oldFinishPercent <= -TRANSFER_ACK_SIZE_WINDOW || currentPercent == 1.0)
	{
		m_oldFinishPercent = currentPercent;
		//return /*m_oldFinishPercent*/m_progressInfo;
		return m_progressInfo;
	}
	return NULL;
}

void P2PFileDataRepositoryReciever::CombinationDataToFile()
{
	Clear();
}

bool P2PFileDataRepositoryReciever::WriteDataToFile(const DataChannelTransferBase<> * data)
{

	return true;
}

void P2PFileDataRepositoryReciever::Clear()
{
	if (!m_writeStream)
		return;
	std::ofstream * outStream = reinterpret_cast<std::ofstream *>(m_writeStream->GetInfoVector());
	if (!outStream)
		return;
	if (outStream->is_open())
		outStream->close();//last should close the file
}


void  P2PFileDataRepositoryReciever::SetResBlockSize(double count)
{
	m_progressInfo->totalTaskSize = count;
}
////////////////
WEBRTCP2PDLLSHARED_EXPORT DataRepositoryBase* CreateP2PFileDataRepositoryReciever(std::string & guid)
{
	return new P2PFileDataRepositoryReciever();
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseP2PFileDataRepositoryReciever(DataRepositoryBase * value)
{
	DeletePtr(value);
}
////////////////
FileDealFunction::FileDealFunction(std::string& path)
{
	NewFile(path);
	m_tempStr = CreateSTLString();
	m_filepath = CreateSTLString();
}

FileDealFunction::FileDealFunction()
{
	m_filepath  = CreateSTLString();
	m_tempStr   = CreateSTLString();
}

FileDealFunction::~FileDealFunction()
{
	DeleteSTLString(m_filepath);
	DeleteSTLString(m_tempStr);
}
	
bool FileDealFunction::NewFile(std::string& path)
{
	ifstream  in;
	std::wstring filepathUnicode = MsToUc(path);
	in.open(filepathUnicode.c_str(), ifstream::out | ifstream::app);

	if (!in)
	{
		return false;
	}
	in.close();
	return true;
}

bool FileDealFunction::ReleaseFile(std::string& path)
{
	if (remove(path.c_str()) == -1)
		return false;
	return true;
}

/*cout << "文件无法重命名(可能原因如下):" << endl;
cout << "\t" << "1. " << newname << " 已存在" << endl
<< "\t" << "2. " << newname << " 正在使用,未关闭." << endl
<< "\t" << "3. " << "你没有权限重命名此文件." << endl;*/
std::string& FileDealFunction::RenameFile(std::string& path)
{
	if (!m_tempStr)
		m_tempStr = CreateSTLString();
	m_tempStr->clear();
	if (!IsExitFile(path))//file is not exit
	{
		m_tempStr->SetString(path);
	}
	else//file is exit, modify the path, add number Suffix,example make test.txt to test01.txt
	{
		std::string tempStr = "";
		AddFileNameSuffix(path, "_01",tempStr);
		m_tempStr->SetString(const_cast<std::string &>(RenameFile(tempStr)));
	}
	return const_cast<std::string &>(m_tempStr->GetString());
}

bool FileDealFunction::IsExitFile(std::string& path )
{
	if (path.empty())
		path = const_cast<std::string &>(m_filepath->GetString());
	ifstream in;
	bool exsit = false;
	std::wstring filepathUnicode = MsToUc(path);
	in.open(filepathUnicode.c_str());

	if (in)
	{
		exsit = true;
	}
	in.close();
	return exsit;
}
////////////
////////////////
bool DataRepositoryBaseExe::Equal(DataRepositoryBase *value)
{
	if (!value)
		return false;
	if (m_resGuid->GetString().compare(value->GetGUID()) == 0)
		return  true;
	return false;
}

////////
FileDealBaseIterator::FileDealBaseIterator(FileDealBase * deal)
{
	m_fileDeal = deal;
}

FileDealBaseIterator::~FileDealBaseIterator()
{
	m_fileDeal = NULL;
}

//////////////
char* UnicodeToUtf8Fun(const wchar_t* unicode)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
	return szUtf8;
}

wchar_t * UTF82WCSFun(const char* szU8)
{
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
	wszString[wcsLen] = '\0';
	return wszString;
}
////////////////
UnicodeToUtf8::UnicodeToUtf8(const wchar_t * str)
{
	m_string = UnicodeToUtf8Fun(str);
}

UnicodeToUtf8::~UnicodeToUtf8()
{
	DeletePtr(m_string);
}

void * UnicodeToUtf8::c_str()
{
	return m_string;
}

////////////
UTF82WCS::UTF82WCS(const char * str)
{
	m_string = UTF82WCSFun(str);
}

UTF82WCS::~UTF82WCS()
{
	DeletePtr(m_string);
}

void * UTF82WCS::c_str()
{
	return m_string;
}