#ifndef DLL_INTERFACE_HEADER
#define DLL_INTERFACE_HEADER
#pragma once

#ifdef  __cplusplus 
#define EXTERN_C_BEGIN   extern "C" { 
#else
#define EXTERN_C_BEGIN
#endif

#ifdef  __cplusplus 
#define EXTERN_C_END }
#else
#define EXTERN_C_END
#endif

#define DLL_PORT
#define DLL_OUT
#ifdef	DLL_PORT
	#ifdef DLL_OUT
		#define WEBRTCP2PDLLSHARED_EXPORT __declspec(dllexport)
	#else
		#define  WEBRTCP2PDLLSHARED_EXPORT  __declspec(dllimport)
	#endif
#else
	#define	WEBRTCP2PDLLSHARED_EXPORT
#endif

///////////Create##X(X* value) is smillar with Create##X(new Class()),the class is hrit the X
#define DllExtern_Dynamic_Interface(X) \
EXTERN_C_BEGIN   \
WEBRTCP2PDLLSHARED_EXPORT X * Create##X(X * value); \
WEBRTCP2PDLLSHARED_EXPORT void Release##X(X* value); \
EXTERN_C_END

#define DllExtern_Fixed_Interface(X) \
EXTERN_C_BEGIN   \
WEBRTCP2PDLLSHARED_EXPORT X * Create##X(); \
WEBRTCP2PDLLSHARED_EXPORT void Release##X(X* value); \
EXTERN_C_END

#define DllExtern_Fixed_Interface_1(X,Y) \
EXTERN_C_BEGIN   \
WEBRTCP2PDLLSHARED_EXPORT X * Create##X(Y value); \
WEBRTCP2PDLLSHARED_EXPORT void Release##X(X* value); \
EXTERN_C_END

#define DllExtern_Fixed_Interface_2(X,Y,Z) \
EXTERN_C_BEGIN   \
WEBRTCP2PDLLSHARED_EXPORT X * Create##X(Y valueY, Z valueZ); \
WEBRTCP2PDLLSHARED_EXPORT void Release##X(X* value); \
EXTERN_C_END

#define DllExtern_Fixed_Interface_3(X,Y,Z,U) \
EXTERN_C_BEGIN   \
WEBRTCP2PDLLSHARED_EXPORT X * Create##X(Y valueY, Z valueZ, U valueU); \
WEBRTCP2PDLLSHARED_EXPORT void Release##X(X* value); \
EXTERN_C_END

#define DllExtern_Fixed_Interface_5(X,Y,Z,U, M, N) \
EXTERN_C_BEGIN   \
WEBRTCP2PDLLSHARED_EXPORT X * Create##X(Y valueY, Z valueZ, U valueU, M valueM, N valueN); \
WEBRTCP2PDLLSHARED_EXPORT void Release##X(X* value); \
EXTERN_C_END



#endif
