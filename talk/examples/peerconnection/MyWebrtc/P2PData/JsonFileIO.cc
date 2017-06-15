#include "JsonFileIO.h"
#include "defaults.h"
#include "webrtc/base/json.h"
#include<iostream>
using namespace std;
#include<fstream>
//////////////////
const Json::Value emptyJson;
class JsonFileIODisposableParasitic
{
public:
	JsonFileIODisposableParasitic();
	JsonFileIODisposableParasitic(const std::string& filepath);
	virtual ~JsonFileIODisposableParasitic();
public:
	virtual bool Write();
	virtual void SetFilePath(const std::string &filepath);

	virtual bool AddNode(const std::string &nodename, const std::string &content);
	virtual bool AddNode(const std::string &nodename, const std::vector<std::string> &content);

	virtual const Json::Value & GetJson();
public:
	std::string m_filePath;
	Json::Value m_json;
};

JsonFileIODisposableParasitic::JsonFileIODisposableParasitic()
{
	SetFilePath("");
	m_json.clear();
}

JsonFileIODisposableParasitic::JsonFileIODisposableParasitic(const std::string &filepath)
{
	SetFilePath(filepath);
	m_json.clear();
}

JsonFileIODisposableParasitic::~JsonFileIODisposableParasitic()
{
	m_filePath.clear();
	m_json.clear();
}

bool JsonFileIODisposableParasitic::Write()
{
	ofstream os;
	os.open(m_filePath, ios::binary);
	if (!os.is_open())
	{
		return false;
	}
	Json::StyledWriter sw;
	os << sw.write(m_json);
	os.close();
	return true;
}


void JsonFileIODisposableParasitic::SetFilePath(const std::string &filepath)
{
	m_filePath = filepath;
}

bool JsonFileIODisposableParasitic::AddNode(const std::string &nodename, const std::string &content)
{
	m_json[nodename] = Json::Value(content);
	return true;
}

bool JsonFileIODisposableParasitic::AddNode(const std::string &nodename, const std::vector<std::string> &content)
{
	Json::Value json = StringVectorToJsonArray(content);/* = rtc::StringVectorToJsonArray(content);*/
	m_json[nodename] = json;
	return true;
}

const Json::Value & JsonFileIODisposableParasitic::GetJson()
{
	/*Json::StyledWriter sw;
	value = sw.write(m_json);*/
	return m_json;
}

///////////
JsonFileIODisposable::JsonFileIODisposable(const std::string &filepath)
{
	m_jsonParalicit = new JsonFileIODisposableParasitic(filepath);
}

JsonFileIODisposable::~JsonFileIODisposable()
{
	DeletePtr(m_jsonParalicit);
}


bool JsonFileIODisposable::Write()
{
	if (m_jsonParalicit)
		return m_jsonParalicit->Write();
	return false;
}


void JsonFileIODisposable::SetFilePath(const std::string &filepath)
{
	if (m_jsonParalicit)
		m_jsonParalicit->SetFilePath(filepath);
}

bool JsonFileIODisposable::AddNode(const std::string &nodename, const std::string &content)
{
	if (m_jsonParalicit)
		return m_jsonParalicit->AddNode(nodename, content);
	return false;
}

bool JsonFileIODisposable::AddNode(const std::string &nodename, const std::vector<std::string> &content)
{
	if (m_jsonParalicit)
		return m_jsonParalicit->AddNode(nodename, content);
	return false;
}

const Json::Value & JsonFileIODisposable::GetJson()
{
	if (m_jsonParalicit)
		return m_jsonParalicit->GetJson();
	return emptyJson;
}
/////////////////////
class JsonFileIOReadParasitic
{
public:
	JsonFileIOReadParasitic(const std::string &filepath, bool fileStyle = true);
	virtual ~JsonFileIOReadParasitic();
public:
	virtual bool Read();
	virtual void SetFilePath(const std::string &filepath);
	virtual bool GetNode(const std::string &nodename, std::string &content);
	virtual bool GetNode(const std::string &nodename, std::vector<std::string>& content);

	virtual const Json::Value & GetJson();
private:
	std::string m_filePath;
	Json::Value m_json;
	std::string * m_textContent;
};

JsonFileIOReadParasitic::JsonFileIOReadParasitic(const std::string &filepath, bool fileStyle)
{
	if (fileStyle)
	{
		SetFilePath(filepath);
		m_json.clear();
		m_textContent = new std::string();
		m_textContent->clear();
	}
	else
	{
		SetFilePath("");
		m_json.clear();
		m_textContent = new std::string();
		*m_textContent = filepath;
	}
}


JsonFileIOReadParasitic::~JsonFileIOReadParasitic()
{
	m_filePath.clear();
	m_json.clear();
	DeletePtr(m_textContent);
}

bool JsonFileIOReadParasitic::Read()
{
	if (!m_filePath.empty())
	{
		Json::Reader reader;
		ifstream in(m_filePath, ios::binary);
		if (in.is_open())
		{
			m_json.clear();
			reader.parse(in, m_json);
			in.close();
			return true;
		}
	}
	else if (m_textContent && !m_textContent->empty())
	{
		Json::Reader reader;
		if (m_textContent && reader.parse(*m_textContent, m_json))
			return true;
	}
	return false;
}

void JsonFileIOReadParasitic::SetFilePath(const std::string &filepath)
{
	m_filePath = filepath;
}

bool JsonFileIOReadParasitic::GetNode(const std::string &nodename, std::string & content)
{
	content = m_json[nodename].asString();
	return true;
}

bool JsonFileIOReadParasitic::GetNode(const std::string &nodename, std::vector<std::string>& content)
{
	content.clear();
    for(Json::Value::iterator value = m_json[nodename].begin();value!=m_json[nodename].end();value++)
	{
        Json::Value &tempValue = *value;
        content.push_back(tempValue.asString());
	}
	return true;
}

const Json::Value & JsonFileIOReadParasitic::GetJson()
{
	return m_json;
}
/////////////
JsonFileIORead::JsonFileIORead(const std::string &filepath, bool fileStyle)
{
	m_jsonParasicit = new JsonFileIOReadParasitic(filepath, fileStyle);
}

JsonFileIORead::~JsonFileIORead()
{
	DeletePtr(m_jsonParasicit);
}

bool JsonFileIORead::Read()
{
	if (m_jsonParasicit)
		return m_jsonParasicit->Read();
	return false;
}

void JsonFileIORead::SetFilePath(const std::string &filepath)
{
	if (m_jsonParasicit)
		m_jsonParasicit->SetFilePath(filepath);
}

bool JsonFileIORead::GetNode(const std::string &nodename,std::string & content)
{
	if (m_jsonParasicit)
		return m_jsonParasicit->GetNode(nodename, content);
	return false;
}

bool JsonFileIORead::GetNode(const std::string &nodename, std::vector<std::string>& content)
{
	if (m_jsonParasicit)
		return m_jsonParasicit->GetNode(nodename, content);
	return false;
}

const Json::Value & JsonFileIORead::GetJson()
{
	if (m_jsonParasicit)
		return m_jsonParasicit->GetJson();
	return emptyJson;
}
////
JsonStyledWriter::JsonStyledWriter()
{
	m_string = TransferNamespace::CreateSTLString();
}

JsonStyledWriter::~JsonStyledWriter()
{
	TransferNamespace::DeleteSTLString(m_string);
}

STLStringShaow*  const JsonStyledWriter::Write(const Json::Value &  value)
{
	if (!m_string)
		return NULL;
	m_string->clear();
	Json::StyledWriter write;
	m_string->SetString(write.write(value));
	return m_string;
}




