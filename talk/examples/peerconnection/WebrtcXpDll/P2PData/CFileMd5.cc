#include"CFileMd5.h"
#include"webrtc/base/logging.h"
#include"webrtc/base/md5.h"
#include"defaults.h"
#include<iostream>
using namespace std;
#include<memory.h>
#include<fstream>
const std::string emptyStr = "";
///////////
WebrtcFileMd5Base::~WebrtcFileMd5Base()
{

}
////////////////
class WebrtcFileMd5Parasitic
{
public:
	WebrtcFileMd5Parasitic();
	virtual ~WebrtcFileMd5Parasitic();
public:
	// blocksize must  less than PER_BLOCK_SIZE
	virtual std::string & CaculateFileMd5(std::string &filepath, size_t blocksize = PER_BLOCK_SIZE);
private:
	std::string m_md5String;
};

WebrtcFileMd5Parasitic::WebrtcFileMd5Parasitic()
{
	m_md5String.clear();
}

WebrtcFileMd5Parasitic::~WebrtcFileMd5Parasitic()
{
	m_md5String.clear();
}

std::string & WebrtcFileMd5Parasitic::CaculateFileMd5(std::string &filepath, size_t blocksize)
{
	if (blocksize > PER_BLOCK_SIZE)
		return const_cast<std::string &>(emptyStr);
	struct rtc::MD5Context context;
	MD5Init(&context);
	char tempStr[PER_BLOCK_SIZE + 1];
	ifstream inFile;
	std::wstring filepathW = MsToUc(filepath);
	inFile.open(filepathW.c_str(), ios::binary | ios::in);
	if (!inFile || !inFile.is_open())
		LOG(INFO) << const_cast<wchar_t *>(filepathW.c_str()) << L"open failure!" << endl;
	inFile.seekg(0, ios::end);
	long filesize = inFile.tellg();
	inFile.seekg(0, ios::beg);
	int currentReadSize = blocksize;//PER_BLOCK_SIZE
	while (inFile.good())
	{
		size_t curseek = inFile.tellg();
		if ((filesize - curseek) < PER_BLOCK_SIZE)
		{
			currentReadSize = filesize - curseek;
		}
		if (!currentReadSize)
			break;
		inFile.read(tempStr, currentReadSize);
		int readSize = inFile.gcount();
		MD5Update(&context, (const uint8_t *)tempStr, readSize);
		memset(tempStr, 0, PER_BLOCK_SIZE + 1);
	}
	////
	unsigned char digest[16];
	MD5Final(&context, (uint8_t *)digest);
	char md5string[33];
	for (int i = 0; i < 16; ++i)
		sprintf(&md5string[i * 2], "%02x", digest[i]);//(unsigned int)
	md5string[32] = '\0';
	inFile.close();
	m_md5String = md5string;
	//LOG(INFO) << "the file  name:" << filepathW.c_str() << "Md5's value is: " << md5string;
	return m_md5String;
}
///////////
WebrtcFileMd5::WebrtcFileMd5()
{
	m_parasiticClass = new WebrtcFileMd5Parasitic();
}


WebrtcFileMd5::~WebrtcFileMd5()
{
	DeletePtr(m_parasiticClass);
}

std::string  & WebrtcFileMd5::CaculateFileMd5(std::string &filepath, size_t blocksize)
{
	if (!m_parasiticClass)
		m_parasiticClass = new WebrtcFileMd5Parasitic();
	return m_parasiticClass->CaculateFileMd5(filepath, blocksize);
}


WEBRTCP2PDLLSHARED_EXPORT WebrtcFileMd5Base * CreateWebrtcFileMd5Base(WebrtcFileMd5Base *Value)
{
	return Value;
}

WEBRTCP2PDLLSHARED_EXPORT void ReleaseWebrtcFileMd5Base(WebrtcFileMd5Base * md5)
{
	DeletePtr(md5);
}
