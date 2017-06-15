#ifndef JSON_FILE_IO_HEADER
#define JSON_FILE_IO_HEADER
#pragma once
#include "DllInterface.h"
#include "TransferData.h"
#include<string>
#include<vector>
EXTERN_C_BEGIN
namespace Json
{
	class Value;
	class StyledWriter;
}

class WEBRTCP2PDLLSHARED_EXPORT NodeDealWriteBase
{
public:
	virtual bool AddNode(const std::string &nodename, const std::string &content) = 0;
	virtual bool AddNode(const std::string &nodename, const std::vector<std::string> &content) = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT NodeDealReadBase
{
public:
	virtual bool GetNode(const std::string &nodename,std::string & content) = 0;
	virtual bool GetNode(const std::string &nodename,std::vector<std::string>& content) = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT JsonFileIOBase
{
public:
	virtual void SetFilePath(const std::string &filepath) = 0;
	virtual const Json::Value & GetJson() = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT JsonFileIOWriteBase :public NodeDealWriteBase, public JsonFileIOBase
{
public:
	virtual bool Write() = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT JsonFileIOReadBase : public NodeDealReadBase, public JsonFileIOBase
{
public:
	virtual bool Read() = 0;
};

class JsonFileIODisposableParasitic;
class WEBRTCP2PDLLSHARED_EXPORT JsonFileIODisposable:public JsonFileIOWriteBase
{
public:
	JsonFileIODisposable(const std::string &filepath = "");
	virtual ~JsonFileIODisposable();
public:
	virtual bool Write();
	virtual void SetFilePath(const std::string &filepath);

	virtual bool AddNode(const std::string &nodename, const std::string &content);
	virtual bool AddNode(const std::string &nodename, const std::vector<std::string> &content);
	virtual const Json::Value & GetJson();
private:
	JsonFileIODisposableParasitic * m_jsonParalicit;
};


class JsonFileIOReadParasitic;
class WEBRTCP2PDLLSHARED_EXPORT JsonFileIORead :public JsonFileIOReadBase
{
public:
	JsonFileIORead(const std::string &filepath, bool fileStyle = true);
	~JsonFileIORead();
public:
	virtual bool Read();
	virtual void SetFilePath(const std::string &filepath);
	virtual bool GetNode(const std::string &nodename, std::string &content);
	virtual bool GetNode(const std::string &nodename, std::vector<std::string>& content);
	virtual const Json::Value & GetJson();
private:
	JsonFileIOReadParasitic *m_jsonParasicit;
};

class WEBRTCP2PDLLSHARED_EXPORT JsonWriteBase
{
public:
	virtual TransferNamespace::STLStringShaow* const Write(const Json::Value &  value) = 0;
};

class WEBRTCP2PDLLSHARED_EXPORT JsonStyledWriter:public JsonWriteBase
{
public:
	explicit JsonStyledWriter();
	virtual ~JsonStyledWriter();
public:
	virtual TransferNamespace::STLStringShaow* const Write(const Json::Value &  value);
private:
	TransferNamespace::STLStringShaow * m_string;
};

EXTERN_C_END
#endif
