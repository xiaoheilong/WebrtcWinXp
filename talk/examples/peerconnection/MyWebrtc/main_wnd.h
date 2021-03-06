﻿/*
*  Copyright 2012 The WebRTC Project Authors. All rights reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_MAIN_WND_H_
#define WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_MAIN_WND_H_
#pragma once

#include <map>
#include <string>
#include "P2PData/TransferData.h"
#include "P2PData/WebrtcClientOutterInterface.h"
#include "P2PData/WebrtcOutterClientExe.h"
#include "P2PData/defaults.h"
#include "P2PData/P2PDllInterface.h"

#include "./CThread.h"
/////////////////
////////////////定义datachannel中传输的数据的类型
// Pure virtual interface for the main window.
#ifdef WIN32
//////////////////////////
class MainWnd : public OutterInterfaceBase
{
public:
	static const wchar_t kClassName[];
	MainWnd(const char* server, const char* chatdefaultStr, int port, bool auto_connect, bool auto_call);
	~MainWnd();

	virtual bool Create();
	virtual bool Destroy();
	virtual bool PreTranslateMessage(MSG* msg);

	virtual bool IsWindow();
	virtual void SwitchToConnectUI();
	virtual void SwitchToPeerList(void * param);
	virtual void SwitchToStreamingUI();
	virtual void MessageBox(const char* caption, const char* text,bool is_error);
	virtual UI current_ui() { return ui_; }

	virtual void StartLocalRenderer(void* local_video);
	virtual void StopLocalRenderer();
	virtual void StartRemoteRenderer(void* remote_video);
	virtual void StopRemoteRenderer();
	virtual void QueueUIThreadCallback(int msg_id, void* data);
	//virtual void TransferProgress(void* data);
	virtual void TransferProgress(const char* taskGuid, const double totalSize, const double finishSize);
	virtual void TransferFileComplete(const char * resGuid, const int& state);
	virtual void OnDisConnection(const int &type, const int & state);
	P2PEnvironmentBase* GetCallBack();
	virtual void RegisterObserver(void* callback);

	virtual HWND handle() const { return wnd_; }

protected:
	enum ChildWindowID
	{
		EDIT_ID = 1,
		BUTTON_ID,
		LABEL1_ID,
		LABEL2_ID,
		LISTBOX_ID,
		EDIT_CHAT_WORD,//add by  in 2016.10.20
		SEND_MESSAGE_BTN,
		SEND_FILE_PATH,
		RECIEVE_FILE_PATH,
		RECIEVE_FILE_PATH_BTN,
		LOCALTION_FILE_EDIT,
		LOCALTION_FILE_BUTTON,
		FILE_CONTROL_PROCESS,
		PROGRESS_TOOL_TIP = 45
	};

	virtual void OnPaint();
	virtual void OnDestroyed();

	virtual void OnDefaultAction();
	virtual void OnSendMessageBtn();
	virtual void OnSendFileBtn();
	virtual void OnSaveFileBtn(); //add by  in 2016.12.17
	virtual void OnMesureLocaltionFile();
	virtual void OnMesureOppositeFile();
	virtual void OnInterruptFileTask();
	virtual void OnCancleFileTask();

	virtual bool OnMessage(UINT msg, WPARAM wp, LPARAM lp, LRESULT* result);

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	static bool RegisterWindowClass();

	//void CreateChildWindow(HWND* wnd, ChildWindowID id, const wchar_t* class_name,
	//                  DWORD control_style, DWORD ex_style);
	virtual void CreateChildWindow(HWND* wnd, ChildWindowID id, const wchar_t * class_name,
		DWORD control_style, DWORD ex_style, RECT &pos = RECT());
	virtual void CreateChildWindows();

	virtual void LayoutConnectUI(bool show);
	virtual void LayoutPeerListUI(bool show);

	virtual void HandleTabbing();

	virtual void RecieveDataChannel(const wchar_t *message);
	virtual void OnConnectToPeer(const int & state);
	virtual void OnConnectTurnServer(const int & state);
public:
	P2PEnvironmentBase* callback_;
protected:
	scoped_ptr<VideoRenderBase> local_renderer_;
	scoped_ptr<VideoRenderBase> remote_renderer_;
	UI ui_;
	HWND wnd_;
	DWORD ui_thread_id_;
	HWND edit1_;
	HWND edit2_;
	HWND label1_;
	HWND label2_;
	HWND button_;
	HWND listbox_;
	HWND edit_chat_word;//add by  in 2016.10.20
	HWND send_file_path;//add by  in 2016.11.29
	HWND send_message_btn;
	HWND send_file_path_btn;//add by  in 2016.11.29
	HWND recieve_file_path;//add by  in 2016.12.16
	HWND recieve_file_path_btn;
	HWND interrupt_file_task_btn;
	HWND cancle_file_task_btn; // cancle the file task
	///////
	HWND localtionFileEdit;//a file's path name,the file include the opposite client sdp and candidate information
	HWND localtionFileButton;
	HWND oppositeFileEdit;
	HWND oppositeFileButton;
	HWND progressToolTip; //progress tooltip 
	HWND progressToolTip1;
	//////
	bool destroyed_;
	void* nested_msg_;
	//P2PEnvironmentBase* callback_;
	static ATOM wnd_class_;
	std::string server_;
	std::string port_;
	std::string m_chatDefaultStr;//add by  in 2016.10.20
	std::string m_wantToSendFileName;//add by  in 2016.11.29 
	std::string m_wantToSaveFileName;
	bool auto_connect_;
	bool auto_call_;
	bool m_interruptFileTask;
};

class SingleWnd:public MainWnd ,public CThread
{
public:
	SingleWnd(const char* server, const char* chatdefaultStr, int port, bool auto_connect, bool auto_call);
	virtual ~SingleWnd();
	enum MsgType
	{
		CREATE_CONNECTION_ = 1,
	};
public:
	virtual void OnDefaultAction() override;
	virtual void LayoutConnectUI(bool show)override;
	virtual void OnMesureLocaltionFile()override;
	virtual void OnMesureOppositeFile()override;
	virtual void OnConnectToPeer(const int & state)override;
	virtual void OnConnectTurnServer(const int& state)override;
	//virtual void OnFileCancel(const char * param);
	virtual void CreateP2P();
	//////////
	void run();
	//virtual void OnMessage(rtc::Message* msg);

protected:
	//rtc::Win32Thread *m_thread;
};

class SingleSendWnd:public SingleWnd
{
public:
	SingleSendWnd(const char* server, const char* chatdefaultStr, int port, bool auto_connect, bool auto_call);
};

class SingleRecieveWnd:public SingleWnd
{
public:
	SingleRecieveWnd(const char* server, const char* chatdefaultStr, int port, bool auto_connect, bool auto_call);
public:
	virtual void OnDefaultAction() override;

};
///////////////////
bool RectIsEmpty(RECT & rect);
#endif  // WIN32

#endif  // WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_MAIN_WND_H_
