#ifndef WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_DEFAULTS_H_
#define WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_DEFAULTS_H_
#pragma once

#include <string>
#include <vector>
#include "DllInterface.h" 
#include <sstream>
#include<memory>
template <class T>
std::string ConvertToString(T value) 
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

template <class T>
void convertFromString(T &value, const std::string &s) 
{
	std::stringstream ss(s);
	ss >> value;
}
template <typename T, typename Deleter = std::default_delete<T>>
using scoped_ptr = std::unique_ptr<T, Deleter>;
EXTERN_C_BEGIN
///////
void AddFileNameSuffix(std::string &path, char * str,std::string &returnStr);
//WEBRTCP2PDLLSHARED_EXPORT void OpenFileFolderWnd(char* , int size);
//WEBRTCP2PDLLSHARED_EXPORT void ANSIToUnicode(wchar_t * ,const char *);
//WEBRTCP2PDLLSHARED_EXPORT void UnicodeToANSI(char* ,const wchar_t *);
WEBRTCP2PDLLSHARED_EXPORT bool IPStrIsValid(const char * ipstr);
///////////input text string class
//////////////对应宏的处理
namespace  MessageTypeDefine 
{
	////////相关宏定义操作
	/////delete one pointer
#define DeletePtr(ptr)  if (ptr) \
	 {\
		delete ptr; \
		ptr = NULL; \
	  }
	////////delete pointer array
#define DeletePtrArray(ptr) \
	if (ptr)\
	{\
		delete[] ptr;\
		ptr = NULL;\
	}

}
#define SEND_CLIENT
EXTERN_C_END
std::string UcToMs(const std::wstring& ws);
std::wstring MsToUc(const std::string& s);
////////
typedef unsigned char      uint8_t;
//////////
#endif  //
