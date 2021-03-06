﻿/*
*  Copyright 2012 The WebRTC Project Authors. All rights reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#include "defaults.h"
//#include "Shlobj.h"
#include <stdlib.h>
#include <string.h>
#include<iostream>
using namespace std;
#include<fstream>
#include<time.h>
#include<algorithm>
#include <stdio.h>
#include <regex>
//#ifdef WIN32
//#include <winsock2.h>
//#else
//#include <unistd.h>
//#endif
/************************
modified by  in 2017.6.8
********************/
//#include "webrtc/base/arraysize.h"
#include "webrtc/base/common.h"

void GetEnvVarOrDefault(std::string &returnStr, char* env_var_name, const char* default_value);
//void GetPeerName(std::string &returnStr);
bool MyLogOutput(const std::string &filepath, std::string &infoStr);
////////////////
void GetEnvVarOrDefault(std::string &returnStr, const char* env_var_name,const char* default_value)
{
	//
	if (!returnStr.empty())
		returnStr.clear();
	//
	const char* env_var = getenv(env_var_name);
	if (env_var)
		returnStr = env_var;

	if (returnStr.empty())
		returnStr = default_value;
}


//void GetPeerName(std::string & returnStr)
//{
//	///first clear the returnStr
//	if (!returnStr.empty())
//		returnStr.clear();
//	///
//	char computer_name[256];
//	GetEnvVarOrDefault(returnStr, "USERNAME", "user");
//	returnStr += '@';
//	if (gethostname(computer_name, arraysize(computer_name)) == 0)
//	{
//		returnStr += computer_name;
//	}
//	else
//	{
//		returnStr += "host";
//    }
//}

bool MyLogOutput(const std::string &filepath, std::string &infoStr)
{
	if (infoStr.empty())
		return false;
	ofstream out(filepath.c_str(), ios::app);
	if (!out)
		return false;
	out.seekp(0, ios::end);
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	out << asctime(timeinfo) << endl;
	out << infoStr.c_str();
	out << endl;
	out << "-----------------------------------------------------";
	out << endl;
	out.close();
	return true;
}


void AddFileNameSuffix(std::string &path, char * str,std::string & returnStr)
{
	returnStr.clear();
	if (!str)
	{
		returnStr = path;
		return;
	}
	string::size_type position = path.find(".");
	if (position == string::npos)
	{
		returnStr = path;
		return;
	}
	returnStr = path.substr(0, position);
	string suffix = path.substr(position);
	returnStr += str;
	returnStr += suffix;
}

//WEBRTCP2PDLLSHARED_EXPORT void OpenFileFolderWnd(char * str1, int size)
//{
//	if (!str1)
//		return;
//	TCHAR szBuffer[MAX_PATH] = { 0 };
//	BROWSEINFO bi;
//	ZeroMemory(&bi, sizeof(BROWSEINFO));
//	bi.hwndOwner = NULL;
//	bi.pszDisplayName = szBuffer;
//	bi.lpszTitle = L"choose a file from the blow:";
//	bi.ulFlags = BIF_EDITBOX | BIF_BROWSEINCLUDEFILES;
//	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
//	if (NULL == idl)
//	{
//		return;
//	}
//	SHGetPathFromIDList(idl, szBuffer);
//	std::wstring buffer(szBuffer);
//	std::string strPath = UcToMs(szBuffer);
//	int size1 = sizeof(wchar_t)*buffer.size();
//	memcpy(str1, strPath.c_str(), strPath.size());
//}

//WEBRTCP2PDLLSHARED_EXPORT void  ANSIToUnicode(wchar_t *value1, const char *str1)
//{
//	if (!value1 && !str1)
//		return;
//	int  len = 0;
//	len = strlen(str1);
//	int  unicodeLen = ::MultiByteToWideChar(CP_ACP,
//		0,
//		str1,
//		-1,
//		NULL,
//		0);
//	wchar_t *  pUnicode;
//	pUnicode = new  wchar_t[unicodeLen + 1];
//	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
//	::MultiByteToWideChar(CP_ACP,
//		0,
//		str1,
//		-1,
//		(LPWSTR)pUnicode,
//		unicodeLen);
//	//value1 = (wchar_t*)pUnicode;
//	wmemcpy(value1, pUnicode, unicodeLen + 1);
//	delete  pUnicode;
//}

//WEBRTCP2PDLLSHARED_EXPORT void UnicodeToANSI(char* value1, const wchar_t * str1)
//{
//	char*     pElementText;
//	int    iTextLen;
//	// wide char to multi char
//	iTextLen = WideCharToMultiByte(CP_ACP,
//		0,
//		str1,
//		-1,
//		NULL,
//		0,
//		NULL,
//		NULL);
//	pElementText = new char[iTextLen + 1];
//	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
//	::WideCharToMultiByte(CP_ACP,
//		0,
//		str1,
//		-1,
//		pElementText,
//		iTextLen,
//		NULL,
//		NULL);
//	//value = pElementText;
//	memcpy(value1, pElementText, iTextLen + 1);
//	delete[] pElementText;
//}

bool CountScope(int number,int low, int up)
{
	return number >= low && number <= up;
}
//***************
/*ipV4(第4版)地址的范围是1.0.0.0～254.255.255.254，由4节组成，每节取值范围0～255。但有一些地址是特殊的，
不能算作”有效值“。特殊的有‍：‍
255.255.255.255也是广播地‍‍址，255开头的‍不可使用。
每个网段的最后一个ip是广播地址，不能被某一个电脑使用。比如192.168.1.255（255不能使用）。可以理解为255结尾的不可使用。
127.0.0.1是表示自己，不能用于电脑间的通讯，127开头的不可用。
0.0.0.0表示无效地址，故不可用。
***
***
*********/

WEBRTCP2PDLLSHARED_EXPORT bool IPStrIsValid(const char * ipstr)
{
	if (ipstr)
	{
		std::string temp = ipstr;
		std::vector<int> vector;
		while (!temp.empty())
		{
			size_t position = temp.find('.');
			if (position != std::string::npos)
			{
				if (vector.size() == 3)
					break;
				std::string count = temp.substr(0, position);
				int tempCount = -1;
				bool allNumber = regex_match(count.c_str(), regex("^-?[1-9]\\d*$"));
				if (!allNumber)
				{
					break;
				}
				convertFromString<int>(tempCount, count);
				vector.push_back(tempCount);
				temp = temp.substr(position + 1, temp.size());
			}
			else
				break;
		}

		if (!temp.empty() && vector.size() == 3)
		{
			int param = 0;
			bool allNumber = regex_match(temp.c_str(), regex("^-?[1-9]\\d*$"));
			if (allNumber)
			{
				convertFromString<int>(param, temp);
				vector.push_back(param);
				if (vector.size() == 4)
				{
					if (CountScope(vector[0], 1, 254) && CountScope(vector[1], 0, 255) && CountScope(vector[2], 0, 255)
						&& CountScope(vector[3], 0, 254))
					{
						if (vector[0] != 127 && vector[1] != 0 && vector[2] != 0 && vector[3] != 1)
							return true;
					}
				}
			}
		}

	}

	return false;
}

/////////////
std::string UcToMs(const std::wstring& ws) {
	size_t convertedChars = 0;
	std::string curLocale = setlocale(LC_ALL, NULL); //curLocale="C"
	const char* chs = "chs";
	setlocale(LC_ALL, chs);
	const wchar_t* wcs = ws.c_str();
	size_t dByteNum = sizeof(wchar_t)*ws.size() + 1;

	char* dest = new char[dByteNum];
	wcstombs(dest,  wcs, dByteNum);
	//wcstombs_s(&convertedChars, dest, dByteNum, wcs, _TRUNCATE);
	std::string result = dest;
	delete[] dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

std::wstring MsToUc(const std::string& s) {
	size_t convertedChars = 0;
	std::string curLocale = setlocale(LC_ALL, NULL);   //curLocale="C"
	const char* chs = "chs";
	setlocale(LC_ALL, chs);
	const char* source = s.c_str();
	size_t charNum = sizeof(char)*s.size() + 1;

	wchar_t* dest = new wchar_t[charNum];
	mbstowcs(dest,  source, charNum);
	//mbstowcs_s(&convertedChars, dest, charNum, source, _TRUNCATE);
	std::wstring result = dest;
	delete[] dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}