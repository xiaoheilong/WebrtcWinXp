#ifndef SOCKET_DECORATOR_HEADER
#define SOCKET_DECORATOR_HEADER
#include"./DllInterface.h"
#pragma once
EXTERN_C_BEGIN
WEBRTCP2PDLLSHARED_EXPORT class SocketBase
{
public:
	virtual int Send(char * buffer, int len) = 0;
	virtual int Recv(char* buffer, int len) = 0;
	virtual void Close() = 0;
	virtual int GetState() = 0;
	virtual bool Connect(void* server) = 0;
	virtual bool IsCloseState() = 0;
};

DllExtern_Fixed_Interface_1(SocketBase,void*)

EXTERN_C_END
#endif
