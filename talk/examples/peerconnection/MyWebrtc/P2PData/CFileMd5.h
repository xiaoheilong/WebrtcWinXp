#ifndef WEBRTC_FILE_MD5
#define WEBRTC_FILE_MD5
#include "DllInterface.h"
#include<string>
#define PER_BLOCK_SIZE   1024

EXTERN_C_BEGIN
class WEBRTCP2PDLLSHARED_EXPORT WebrtcFileMd5Base
{
public:
	virtual ~WebrtcFileMd5Base();
public:
	virtual std::string & CaculateFileMd5(std::string &filepath, size_t blocksize = PER_BLOCK_SIZE) = 0;
};

class WebrtcFileMd5Parasitic;
class WEBRTCP2PDLLSHARED_EXPORT WebrtcFileMd5 :public WebrtcFileMd5Base
{
public:
	WebrtcFileMd5();
	virtual ~WebrtcFileMd5();
public:
	virtual std::string & CaculateFileMd5(std::string &filepath, size_t blocksize = PER_BLOCK_SIZE);
private:
	WebrtcFileMd5Parasitic * m_parasiticClass;
};
DllExtern_Dynamic_Interface(WebrtcFileMd5Base)
EXTERN_C_END
#endif
