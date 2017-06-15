/*
*  Copyright 2012 The WebRTC Project Authors. All rights reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#include "main_wnd.h"
#include "P2PData/JsonFileIO.h"
#include <math.h>
#include "webrtc/base/bind.h"
#include "P2PData/defaults.h"
#include "P2PData/P2PDllInterface.h"
#include "Commctrl.h"
#pragma comment(lib,"comctl32.lib")
#include<iostream>
using namespace std;
#include<fstream>
#include <stdlib.h>
#include<assert.h>
//////////
#if defined _M_IX86  
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")  
#elif defined _M_IA64  
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")  
#elif defined _M_X64  
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")  
#else  
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")  
#endif  

template <typename T, size_t N> char(&ArraySizeHelper(T(&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))
/////////
ATOM MainWnd::wnd_class_ = 0;
const wchar_t MainWnd::kClassName[] = L"WebRTC_MainWnd";
const wchar_t progressControlName[] = L"zly";
wchar_t connect_turnserver_str[] = L"连接";//"ConnectToTurnServer";
const wchar_t disconnect_turnserver_str[] = L"断开";//"DisconnectTurnserver";
//ProgressControlTask progressGolabel;
using rtc::sprintfn;

namespace {

	const char kConnecting[] = "Connecting... ";
	const char kNoVideoStreams[] = "(no video streams either way)";
	const char kNoIncomingStream[] = "(no incoming video)";

	void CalculateWindowSizeForText(HWND wnd, const wchar_t* text,
		size_t* width, size_t* height) {
		HDC dc = ::GetDC(wnd);
		RECT text_rc = { 0 };
		::DrawText(dc, text, -1, &text_rc, DT_CALCRECT | DT_SINGLELINE);
		::ReleaseDC(wnd, dc);
		RECT client, window;
		::GetClientRect(wnd, &client);
		::GetWindowRect(wnd, &window);

		*width = text_rc.right - text_rc.left;
		*width += (window.right - window.left) -
			(client.right - client.left);
		*height = text_rc.bottom - text_rc.top;
		*height += (window.bottom - window.top) -
			(client.bottom - client.top);
	}

	HFONT GetDefaultFont() {
		static HFONT font = reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		return font;
	}

	std::string GetWindowText(HWND wnd) {
		char text[5000] = { 0 };
		::GetWindowTextA(wnd, &text[0], ARRAYSIZE(text));
		return text;
	}

	void AddListBoxItem(HWND listbox, const std::string& str, LPARAM item_data) {
		LRESULT index = ::SendMessageA(listbox, LB_ADDSTRING, 0,
			reinterpret_cast<LPARAM>(str.c_str()));
		::SendMessageA(listbox, LB_SETITEMDATA, index, item_data);
	}

}  
//////////////////////////////////////////
MainWnd::MainWnd(const char* server, const char * chatdefaultstr, int port, bool auto_connect,
	bool auto_call)
	: ui_(CONNECT_TO_SERVER), wnd_(NULL), edit1_(NULL), edit2_(NULL),
	label1_(NULL), label2_(NULL), button_(NULL), listbox_(NULL), edit_chat_word(NULL),send_file_path(NULL), send_message_btn(NULL),
	destroyed_(false), callback_(NULL), nested_msg_(NULL),
	send_file_path_btn(NULL), recieve_file_path(NULL), recieve_file_path_btn(NULL), localtionFileEdit(NULL),
	localtionFileButton(NULL),oppositeFileEdit(NULL),oppositeFileButton(NULL), interrupt_file_task_btn(NULL), cancle_file_task_btn(NULL),progressToolTip(NULL), progressToolTip1(NULL),server_(server), m_chatDefaultStr(chatdefaultstr), m_wantToSendFileName(""), m_wantToSaveFileName(""), auto_connect_(auto_connect), auto_call_(auto_call)
{
	char buffer[10] = { 0 };
	sprintfn(buffer, sizeof(buffer), "%i", port);
	port_ = buffer;
	m_interruptFileTask = false;
	//m_tempMsg = new FileDataPack();
	/*callback_ = CreateP2PEnvironment("120.76.112.94", 3478, "helloword", "helloword", "");
	RegisterOutter(callback_,this);*/
}

MainWnd::~MainWnd() 
{
	DestroyWindow(label1_);
	DestroyWindow(edit1_);
	DestroyWindow(label2_);
	DestroyWindow(edit2_);
	DestroyWindow(button_);
	DestroyWindow(edit_chat_word);
	DestroyWindow(send_message_btn);
	DestroyWindow(send_file_path);
	DestroyWindow(send_file_path_btn);
	DestroyWindow(recieve_file_path);
	DestroyWindow(recieve_file_path_btn);
	DestroyWindow(localtionFileEdit);
	DestroyWindow(localtionFileButton);
	DestroyWindow(oppositeFileEdit);
	DestroyWindow(oppositeFileButton);
	DestroyWindow(interrupt_file_task_btn);
	DestroyWindow(cancle_file_task_btn);
	DestroyWindow(progressToolTip);
	DestroyWindow(progressToolTip1);
	assert(!IsWindow());
	m_interruptFileTask = false;
	if (callback_)
	{
		CloseP2PEnvironment(callback_);
	}
}

bool MainWnd::Create() {
	assert(wnd_ == NULL);
	if (!RegisterWindowClass())
		return false;

	ui_thread_id_ = ::GetCurrentThreadId();
	/////////////////
#ifdef USE_WEBRTC_CONDUCTOR
	wnd_ = ::CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, kClassName, L"mengxiaolong",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,// 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, GetModuleHandle(NULL), this);
#else
	wnd_ = ::CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, kClassName, L"mengxiaolong",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,// 
		0, 0, 640, 480,
		NULL, NULL, GetModuleHandle(NULL), this);
	DWORD   dwStyle = GetWindowLong(wnd_, GWL_STYLE);
	dwStyle &= ~(WS_SIZEBOX);
	dwStyle &= ~(WS_MAXIMIZEBOX);
	dwStyle &= ~(WS_MINIMIZEBOX);
	SetWindowLong(wnd_, GWL_STYLE, dwStyle);
#endif
	
	::SendMessage(wnd_, WM_SETFONT, reinterpret_cast<WPARAM>(GetDefaultFont()),
		TRUE);

	CreateChildWindows();
	SwitchToConnectUI();

	return wnd_ != NULL;
}

bool MainWnd::Destroy() {
	BOOL ret = FALSE;
	if (IsWindow()) {
		ret = ::DestroyWindow(wnd_);
	}

	return ret != FALSE;
}

bool MainWnd::IsWindow() {
	return wnd_ && ::IsWindow(wnd_) != FALSE;
}

bool MainWnd::PreTranslateMessage(MSG* msg) {
	bool ret = false;
	if (msg->message == WM_CHAR) {
		if (msg->wParam == VK_TAB) {
			HandleTabbing();
			ret = true;
		}
		else if (msg->wParam == VK_RETURN) {
			OnDefaultAction();
			ret = true;
		}
		else if (msg->wParam == VK_ESCAPE) 
		{
	
		}
	}
	else if (msg->message == UI_TASK_PROGRESS_CALLBACK)
	{
		/*double totalSize = 0;
		double finishSize = 0;
		FileTaskProgress((void*)(msg->wParam), totalSize, finishSize);
		double progress = finishSize / totalSize;
		if (recieve_file_path)
		{
			SendMessage(recieve_file_path, PBM_SETPOS, (WPARAM)progress, 0);
		}*/
	}
	else if (msg->message == WM_LBUTTONDBLCLK)
	{
		if (msg->hwnd == send_file_path)
		{
			/*char filepath[1000];
			memset(filepath, 0, 1000);
			bool showProgress = false;
			OpenFileFolderWnd(filepath, 1000);
			if (!callback_)
			{
				ret = false;
				return ret;
			}
			char guid[1000];
			memset(guid, 0, 1000);
			bool result = CreateSendFile(callback_, filepath, guid);
			SetWindowTextA(send_file_path, guid);
			if (result)
				::ShowWindow(recieve_file_path, SW_SHOWNA);
			else
				::ShowWindow(recieve_file_path, SW_HIDE);*/

		}
	}
	else if (msg->message == WM_MOUSEMOVE)
	{
		if (msg->hwnd == recieve_file_path)
		{
			double progress = SendMessage(recieve_file_path, PBM_GETPOS, (WPARAM)0, 0);
			//show the progressToolTip to the progress control top
			::SetWindowTextA(progressToolTip, ConvertToString(progress).c_str());
			TOOLINFO g_toolItem;
			memset(&g_toolItem, 0, sizeof(TOOLINFO));
			g_toolItem.cbSize = sizeof(TOOLINFO);
			g_toolItem.uFlags = TTF_SUBCLASS;
			g_toolItem.hwnd = recieve_file_path;
			g_toolItem.hinst = GetModuleHandle(NULL);
			//std::wstring progress1 = L"";
			int strSize = ConvertToString(progress).size();
			std::wstring progress1;
			//ANSIToUnicode(progress1, ConvertToString(progress).c_str());
			progress1 = MsToUc(ConvertToString(progress).c_str());
			progress1 += L"%";
			g_toolItem.lpszText = const_cast<wchar_t *>(progress1.c_str());
			GetClientRect(recieve_file_path, &g_toolItem.rect);
			SendMessage(progressToolTip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&g_toolItem);
		}
	}
	return ret;
}

void MainWnd::SwitchToConnectUI() 
{
	assert(IsWindow());
	LayoutPeerListUI(false);
	ui_ = CONNECT_TO_SERVER;
	LayoutConnectUI(true);
	::SetFocus(edit1_);

	if (auto_connect_)
		::PostMessage(button_, BM_CLICK, 0, 0);
}

void MainWnd::SwitchToPeerList(void * param) {

}

void MainWnd::SwitchToStreamingUI() {
	LayoutConnectUI(false);
	LayoutPeerListUI(false);
	ui_ = STREAMING;
}

void MainWnd::MessageBox(const char* caption, const char* text, bool is_error) {
	DWORD flags = MB_OK;
	if (is_error)
		flags |= MB_ICONERROR;

	::MessageBoxA(handle(), text, caption, flags);
}


void MainWnd::StartLocalRenderer(/*webrtc::VideoTrackInterface*/void* local_video) {
	//local_renderer_.reset(new VideoRenderer(handle(), 1, 1, local_video));
}

void MainWnd::StopLocalRenderer() {
	//local_renderer_.reset();
}

void MainWnd::StartRemoteRenderer(/*webrtc::VideoTrackInterface*/void* remote_video) {
	//remote_renderer_.reset(new VideoRenderer(handle(), 1, 1, remote_video));
}

void MainWnd::StopRemoteRenderer() {
	//remote_renderer_.reset();
}


void MainWnd::QueueUIThreadCallback(int msg_id, void* data) {
	::PostThreadMessage(ui_thread_id_, msg_id,
		(WPARAM)(data), NULL);
}
void MainWnd::TransferProgress(/*void* data*/const char* taskGuid, const double totalSize, const double finishSize)
{
	//QueueUIThreadCallback(UI_TASK_PROGRESS_CALLBACK, data);
	//FileTaskProgress(data, totalSize, finishSize);
	double progress = finishSize / totalSize;
	if (recieve_file_path)
	{
		SendMessage(recieve_file_path, PBM_SETPOS, (WPARAM)(progress*100), 0);
	}
	if(progress == 1.0)
		std::cout << "finish!" << std::endl;
}

void MainWnd::TransferFileComplete(const char * resGuid, const int& state)
{
 	const char * guid = static_cast<const char*>(resGuid);
	if (guid)
	{
		std::string tip;
		tip += guid;
		switch (state)
		{
		case FILE_TRANSFER_SUCESS:
		{
			tip += " transfer succesefully!";
		}
		break;
		case FILE_TRANSFER_FAILURE:
		{
			tip += " transfer failure!";
		}
		default:
			break;
		}
		this->MessageBox("tip",tip.c_str(), false);
	}
}

void MainWnd::OnDisConnection(const int &type, const int & state)
{
	switch (type)
	{
	case TurnServer:
	{
		int a = 0;
		a++;
	}
	break;
	case PeerClient:
	{
		int a = 0;
		a++;
	}
	default:
		break;
	}
}

void MainWnd::RegisterObserver(void* callback)
{
	callback_ = static_cast<P2PEnvironmentBase *>(callback);
}

P2PEnvironmentBase* MainWnd::GetCallBack()
{
	return callback_;
}

void MainWnd::OnPaint() {
	PAINTSTRUCT ps;
	::BeginPaint(handle(), &ps);

	RECT rc;
	::GetClientRect(handle(), &rc);



	HBRUSH brush = ::CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
	::FillRect(ps.hdc, &rc, brush);
	::DeleteObject(brush);

	::EndPaint(handle(), &ps);
}

void MainWnd::OnDestroyed() {
	PostQuitMessage(0);
}

void MainWnd::OnDefaultAction()
{
	if (!callback_)
		return;
	if (ui_ == CONNECT_TO_SERVER)
	{
		std::string server(GetWindowText(edit1_));
		std::string port_str(GetWindowText(edit2_));
		int port = port_str.length() ? atoi(port_str.c_str()) : 0;
	}
	else
	{
		MessageBoxA(wnd_, "OK!", "Yeah", MB_OK);
	}
}

void MainWnd::OnSendMessageBtn()
{
	//send message
	wchar_t text[1024];
	memset(text, 0, 1024);
	::GetWindowText(edit_chat_word, text , 1023);
	int textsize = strlen(text);

	size_t number = wcslen(text) + 1;
	size_t convertSize = 0;
	char goaldata[1024] = { 0 };
	string curLocale = setlocale(LC_ALL, NULL);
	const char * chs = "chs";
	setlocale(LC_ALL, chs);
	wcstombs_s(&convertSize, goaldata, number, text, _TRUNCATE);
	SendText(callback_, text, textsize);
	setlocale(LC_ALL, curLocale.c_str());
}

//////makesure send the file that selected ,the file name is the m_wantToSendFileName edit's content
void MainWnd::OnSendFileBtn()
{
	if (send_file_path)
	{
		char  filepath[MAX_PATH] ;
		memset(filepath, 0, MAX_PATH);
		::GetWindowTextA(send_file_path , filepath, MAX_PATH);
		if (callback_)
		{
			::ShowWindow(recieve_file_path, SW_SHOWNA);
			char path[1000];
			memset(path, 0, 1000);
			//std::string pathString = GetWindowText(edit_chat_word);
			//memcpy(path, pathString.c_str(), pathString.size());
			wchar_t path1[1000];
			memset(path1, 0, 0);
			::GetWindowTextW(edit_chat_word, path1, 1000);
			std::wstring strWTest = path1;
			std::string strTest = UcToMs(strWTest);
			memcpy(path, strTest.c_str(), strTest.size());
			std::string str1 = path;
			std::wstring str2 = MsToUc(str1);
			
			SendFile(callback_, filepath, const_cast<char*>(str1.c_str()));
		}
	}
}

void MainWnd::OnSaveFileBtn()
{
	if (recieve_file_path_btn)
	{
		char  filepath[MAX_PATH];
		memset(filepath, 0, MAX_PATH);
		::GetWindowTextA(send_file_path, filepath, MAX_PATH);
		if (callback_)
		{
			::ShowWindow(recieve_file_path, SW_SHOWNA);
			std::wstring savePath = L"C:\\良心剧.txt";
			std::string  saveMPath = UcToMs(savePath);
			if (CreateRecieveFile(callback_, filepath, const_cast<char*>(saveMPath.c_str())))
			{
				::MessageBoxA(NULL, "CreateRecieveFile succesefully!", "", MB_OK);
			}
			else
				::MessageBoxA(NULL, "CreateRecieveFile failurelly!", "", MB_OK);
		}
	}
}

void MainWnd::OnMesureLocaltionFile()
{
	
}

void MainWnd::OnMesureOppositeFile()
{
}

void MainWnd::OnInterruptFileTask()
{
	//interrup file task
	std::string filepath1 = GetWindowText(oppositeFileEdit);/* "d:\\localtionJson_p.json"*/
	int size = filepath1.size();
	char *filepath = new char[size + 1];
	memset(filepath, 0, size + 1);
	memcpy(filepath, filepath1.c_str(), size);
	filepath[filepath1.size()] = '\0';
	SetOppositeSdpIcecandidate(callback_, filepath);
	delete filepath;
	filepath = NULL;
	::MessageBoxA(NULL, "Set Opposite Information File Successfully!", "", MB_OK);
}

void MainWnd::OnCancleFileTask()
{
	char  filepath[MAX_PATH];
	memset(filepath, 0, MAX_PATH);
	::GetWindowTextA(send_file_path, filepath, MAX_PATH);
	CancleFileTask(callback_, filepath);
}

bool MainWnd::OnMessage(UINT msg, WPARAM wp, LPARAM lp, LRESULT* result) {
	switch (msg) {
	case WM_ERASEBKGND:
		*result = TRUE;
		return true;

	case WM_PAINT:
		OnPaint();
		return true;

	case WM_SETFOCUS:
		if (ui_ == CONNECT_TO_SERVER) {
			SetFocus(edit1_);
		}
		return true;

	case WM_SIZE:
		if (ui_ == CONNECT_TO_SERVER) {
			LayoutConnectUI(true);
		}
		break;

	case WM_CTLCOLORSTATIC:
		*result = reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_WINDOW));
		return true;

	case WM_COMMAND:
		if (button_ == reinterpret_cast<HWND>(lp))
		{
			if (BN_CLICKED == HIWORD(wp))
				OnDefaultAction();
		}
		else if (listbox_ == reinterpret_cast<HWND>(lp))
		{
			if (LBN_DBLCLK == HIWORD(wp)) {
				OnDefaultAction();
			}
		}
		else if (send_message_btn == reinterpret_cast<HWND>(lp))
		{
			//if (LBN_DBLCLK == HIWORD(wp))
			OnSendMessageBtn();
		}
		else if (localtionFileButton == reinterpret_cast<HWND>(lp))
		{
			OnMesureLocaltionFile();
		}
		else if (oppositeFileButton == reinterpret_cast<HWND>(lp))
		{
			OnMesureOppositeFile();
		}
		else if (interrupt_file_task_btn == reinterpret_cast<HWND>(lp))
		{
			OnInterruptFileTask();
		}
		else if (cancle_file_task_btn == reinterpret_cast<HWND>(lp))
		{
			OnCancleFileTask();
		}
		else if (send_file_path_btn == reinterpret_cast<HWND>(lp))
		{
			OnSendFileBtn();
		}
		else if (recieve_file_path_btn == reinterpret_cast<HWND>(lp))
		{
			OnSaveFileBtn();
		}
		else if (send_file_path == reinterpret_cast<HWND>(lp))
		{
			if (EN_CHANGE == HIWORD(wp))
			{
				if (recieve_file_path)
				{
					::ShowWindow(recieve_file_path, SW_HIDE);
					SendMessage(recieve_file_path, PBM_SETPOS, (WPARAM)0, 0);
				}
			}
		}
		return true;

	case WM_CLOSE:
	{
		//should  delete the p2pconnection
		CloseTurnServerConnection(callback_);
	}
		break;
	}
	return false;
}

// static
LRESULT CALLBACK MainWnd::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	MainWnd* me = reinterpret_cast<MainWnd*>(
		::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (!me && WM_CREATE == msg) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lp);
		me = reinterpret_cast<MainWnd*>(cs->lpCreateParams);
		me->wnd_ = hwnd;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(me));
	}

	LRESULT result = 0;
	if (me) {
		void* prev_nested_msg = me->nested_msg_;
		me->nested_msg_ = &msg;

		bool handled = me->OnMessage(msg, wp, lp, &result);
		if (WM_NCDESTROY == msg) {
			me->destroyed_ = true;
		}
		else if (!handled) {
			result = ::DefWindowProc(hwnd, msg, wp, lp);
		}

		if (me->destroyed_ && prev_nested_msg == NULL) {
			me->OnDestroyed();
			me->wnd_ = NULL;
			me->destroyed_ = false;
		}

		me->nested_msg_ = prev_nested_msg;
	}
	else {
		result = ::DefWindowProc(hwnd, msg, wp, lp);
	}

	return result;
}

// static
bool MainWnd::RegisterWindowClass() {
	if (wnd_class_)
		return true;

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_DBLCLKS;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.lpfnWndProc = &WndProc;
	wcex.lpszClassName = kClassName;
	wnd_class_ = ::RegisterClassEx(&wcex);
	assert(wnd_class_ != 0);
	return wnd_class_ != 0;
}

void MainWnd::CreateChildWindow(HWND* wnd, ChildWindowID id, const wchar_t * class_name,
	DWORD control_style, DWORD ex_style, RECT &pos)
{
	if (::IsWindow(*wnd))
		return;
	if (RectIsEmpty(pos))
	{
		pos = { 100, 100, 200 , 200 };
	}
	// Child windows are invisible at first, and shown after being resized. pos.left, pos.top, pos.right - pos.left, pos.bottom - pos.top,
	DWORD style = WS_CHILD | control_style;
	*wnd = ::CreateWindowEx(ex_style, class_name, L"", control_style, 
		pos.left, pos.top, pos.right - pos.left, pos.bottom - pos.top, wnd_,
		reinterpret_cast<HMENU>(id),
		GetModuleHandle(NULL), NULL);
	assert(::IsWindow(*wnd) != FALSE);
	::SendMessage(*wnd, WM_SETFONT, reinterpret_cast<WPARAM>(GetDefaultFont()),
		TRUE);
}

void MainWnd::CreateChildWindows() {
	// Create the child windows in tab order.
	CreateChildWindow(&label1_, LABEL1_ID, L"Static", WS_CHILD| ES_CENTER | ES_READONLY, 0);
	CreateChildWindow(&edit1_, EDIT_ID, L"Edit",
		WS_BORDER|WS_CHILD | ES_LEFT | ES_NOHIDESEL | WS_TABSTOP| ES_MULTILINE, WS_EX_CLIENTEDGE);
	CreateChildWindow(&label2_, LABEL2_ID, L"Static", WS_CHILD | ES_CENTER | ES_READONLY, 0);
	CreateChildWindow(&edit2_, EDIT_ID, L"Edit",
		WS_BORDER | WS_CHILD | ES_LEFT | ES_NOHIDESEL | WS_TABSTOP| ES_MULTILINE, WS_EX_CLIENTEDGE);
	CreateChildWindow(&button_, BUTTON_ID, L"Button", WS_CHILD | BS_CENTER | WS_TABSTOP, 0);

	CreateChildWindow(&listbox_, LISTBOX_ID, L"ListBox",
		WS_CHILD | LBS_HASSTRINGS | LBS_NOTIFY, WS_EX_CLIENTEDGE);
	//////add by in 2016.10.20
	RECT chat_word_pos = { 50,300,200,350 };
	CreateChildWindow(&edit_chat_word, EDIT_CHAT_WORD, L"Edit",
		WS_BORDER | WS_CHILD | ES_LEFT | ES_NOHIDESEL | WS_TABSTOP| ES_MULTILINE, WS_EX_STATICEDGE, chat_word_pos);
	///////////
	RECT send_message_btn_rect = { 300,300,350,350 };
	CreateChildWindow(&send_message_btn, SEND_MESSAGE_BTN, L"Button", WS_CHILD | BS_CENTER | WS_TABSTOP, WS_EX_STATICEDGE, send_message_btn_rect);
	/////////add by  in 2016.11.29 send_file_path
	RECT send_file_path_pos = {400,300,550,350};
	CreateChildWindow(&send_file_path,SEND_FILE_PATH,L"Edit", WS_BORDER | ES_MULTILINE |WS_CHILD | ES_LEFT | ES_NOHIDESEL | WS_TABSTOP, WS_EX_STATICEDGE, send_file_path_pos);
	////////////
	CreateChildWindow(&send_file_path_btn, SEND_MESSAGE_BTN, L"Button", WS_CHILD | BS_CENTER | WS_TABSTOP, WS_EX_STATICEDGE, send_file_path_pos);
	CreateChildWindow(&recieve_file_path, FILE_CONTROL_PROCESS, PROGRESS_CLASS, WS_CHILD | WS_BORDER| WS_CHILD | WS_VISIBLE| PBS_SMOOTH, 0, send_file_path_pos);
	SendMessage(recieve_file_path, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	SendMessage(recieve_file_path, PBM_DELTAPOS, (WPARAM)1, 0);
	SendMessage(recieve_file_path, PBM_SETBARCOLOR, NULL, RGB(0, 255, 0));
	SendMessage(recieve_file_path, PBM_SETBKCOLOR, NULL, RGB(255, 255, 255));  //进度条背景颜色
	SendMessage(recieve_file_path, PBM_SETPOS, (WPARAM)0, 0);

	CreateChildWindow(&progressToolTip1, FILE_CONTROL_PROCESS, PROGRESS_CLASS, WS_CHILD | WS_BORDER | WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 0, send_file_path_pos);
	SendMessage(progressToolTip1, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	SendMessage(progressToolTip1, PBM_DELTAPOS, (WPARAM)1, 0);
	SendMessage(progressToolTip1, PBM_SETBARCOLOR, NULL, RGB(0, 255, 0));
	SendMessage(progressToolTip1, PBM_SETBKCOLOR, NULL, RGB(255, 255, 255));  //进度条背景颜色
	SendMessage(progressToolTip1, PBM_SETPOS, (WPARAM)0, 0);

	CreateChildWindow(&recieve_file_path_btn, RECIEVE_FILE_PATH_BTN, L"Button", WS_CHILD | BS_CENTER | WS_TABSTOP, WS_EX_STATICEDGE, send_file_path_pos);
	
	CreateChildWindow(&localtionFileEdit, LOCALTION_FILE_EDIT, L"Edit", WS_BORDER | ES_MULTILINE |WS_CHILD | BS_CENTER | WS_TABSTOP| WS_VSCROLL, WS_EX_STATICEDGE, send_file_path_pos);
	CreateChildWindow(&localtionFileButton, LOCALTION_FILE_BUTTON, L"Button", WS_CHILD | BS_CENTER | WS_TABSTOP, WS_EX_STATICEDGE, send_file_path_pos);
	
	CreateChildWindow(&oppositeFileEdit, LOCALTION_FILE_EDIT, L"Edit", WS_BORDER | ES_MULTILINE |WS_CHILD | BS_CENTER | WS_TABSTOP | WS_VSCROLL, WS_EX_STATICEDGE, send_file_path_pos);
	CreateChildWindow(&oppositeFileButton, LOCALTION_FILE_BUTTON, L"Button", WS_CHILD | BS_CENTER | WS_TABSTOP, WS_EX_STATICEDGE, send_file_path_pos);
	CreateChildWindow(&interrupt_file_task_btn, LOCALTION_FILE_BUTTON, L"Button", WS_CHILD | BS_CENTER | WS_TABSTOP, WS_EX_STATICEDGE, send_file_path_pos);
	CreateChildWindow(&cancle_file_task_btn, LOCALTION_FILE_BUTTON, L"Button", WS_CHILD | BS_CENTER | WS_TABSTOP, WS_EX_STATICEDGE, send_file_path_pos);
	progressToolTip = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wnd_,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);
	SetWindowPos(progressToolTip, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	//////////////////////
	::SetWindowTextA(edit1_, server_.c_str());
	::SetWindowTextA(edit2_, port_.c_str());
	::SetWindowTextA(edit_chat_word, m_chatDefaultStr.c_str());
	::SetWindowTextA(send_file_path, m_wantToSendFileName.c_str());
}

void MainWnd::LayoutConnectUI(bool show) {
	struct Windows {
		HWND wnd;
		const wchar_t* text;
		size_t width;
		size_t height;
	} windows[] = {
		{ label1_, L"Server" },
		{ edit1_, L"120.76.112.94" },
		{ label2_, L":" },
		{ edit2_, L"XyXyX" },
		{ button_, L"Connect" },
		{ edit_chat_word,L"please enter some words" },//add by in 2016.10.20
		{ send_message_btn,L"发送:" },
		{send_file_path,L"send file name"},
		{ send_file_path_btn,L"send file" },
		{ recieve_file_path,L"" },
		{ recieve_file_path_btn,L"save file" },
	};

	if (show)
	{
		const size_t kSeparator = 5;
		size_t total_width = (ARRAYSIZE(windows) - 4) * kSeparator;

		for (size_t i = 0; i < ARRAYSIZE(windows) - 6; ++i) {
			CalculateWindowSizeForText(windows[i].wnd, windows[i].text,
				&windows[i].width, &windows[i].height);
			total_width += windows[i].width;
		}

		RECT rc;
		::GetClientRect(wnd_, &rc);
		size_t x = (rc.right / 2) - (total_width / 2);
		size_t y = rc.bottom / 2;
		for (size_t i = 0; i < ARRAYSIZE(windows) - 6; ++i) {
			size_t top = y - (windows[i].height / 2);
			::MoveWindow(windows[i].wnd, static_cast<int>(x), static_cast<int>(top),
				static_cast<int>(windows[i].width),
				static_cast<int>(windows[i].height),
				TRUE);
			x += kSeparator + windows[i].width;
			if (windows[i].text[0] != 'X')
				::SetWindowText(windows[i].wnd, windows[i].text);
			::ShowWindow(windows[i].wnd, SW_SHOWNA);
		}
		//////chat_edit width and height
		size_t index = ARRAYSIZE(windows) - 6;
		::MoveWindow(windows[index].wnd, static_cast<int>(50), static_cast<int>(400),
			static_cast<int>(200),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_HIDE);
		/////////
		index = ARRAYSIZE(windows) - 5;
		::MoveWindow(windows[index].wnd, static_cast<int>(350), static_cast<int>(400),
			static_cast<int>(50),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_HIDE);
		////////send_file_path
		index = ARRAYSIZE(windows) - 4;
		::MoveWindow(windows[index].wnd, static_cast<int>(450), static_cast<int>(400),
			static_cast<int>(150),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_HIDE);
		/////////
		index = ARRAYSIZE(windows) - 3;
		::MoveWindow(windows[index].wnd, static_cast<int>(620), static_cast<int>(400),
			static_cast<int>(60),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_HIDE);
		/////////
		index = ARRAYSIZE(windows) - 2;
		::MoveWindow(windows[index].wnd, static_cast<int>(690), static_cast<int>(400),
			static_cast<int>(150),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_HIDE);
		/////////
		index = ARRAYSIZE(windows) - 1;
		::MoveWindow(windows[index].wnd, static_cast<int>(840), static_cast<int>(400),
			static_cast<int>(60),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_HIDE);
		///////////

		//////////////
		::SetWindowText(localtionFileEdit, L"localtion json file!");
		::MoveWindow(localtionFileEdit, static_cast<int>(40), static_cast<int>(200),
			static_cast<int>(200),
			static_cast<int>(20),
			TRUE);
		::ShowWindow(localtionFileEdit, SW_HIDE);
		::SetWindowText(localtionFileButton, L"OK");
		::MoveWindow(localtionFileButton, static_cast<int>(240), static_cast<int>(200),
			static_cast<int>(60),
			static_cast<int>(20),
			TRUE);
		::ShowWindow(localtionFileButton, SW_HIDE);

		::SetWindowText(oppositeFileEdit, L"opposite json file!");
		::MoveWindow(oppositeFileEdit, static_cast<int>(340), static_cast<int>(200),
			static_cast<int>(200),
			static_cast<int>(20),
			TRUE);
		::ShowWindow(oppositeFileEdit, SW_HIDE);

		::SetWindowText(oppositeFileButton, L"OK");
		::MoveWindow(oppositeFileButton, static_cast<int>(540), static_cast<int>(200),
			static_cast<int>(60),
			static_cast<int>(20),
			TRUE);
		::ShowWindow(oppositeFileButton, SW_HIDE);

	}
	else
	{
		for (size_t i = 0; i < ARRAYSIZE(windows) - 6; ++i) {
			::ShowWindow(windows[i].wnd, SW_HIDE);
		}
		::ShowWindow(recieve_file_path, SW_HIDE);
		::ShowWindow(localtionFileEdit, SW_HIDE);
		::ShowWindow(localtionFileButton, SW_HIDE);
		::ShowWindow(oppositeFileEdit, SW_HIDE);
		::ShowWindow(oppositeFileButton, SW_HIDE);
	}
	//::ShowWindow(recieve_file_path_btn, SW_HIDE);
}

void MainWnd::LayoutPeerListUI(bool show) {
	if (show) {
		RECT rc;
		::GetClientRect(wnd_, &rc);
		::MoveWindow(listbox_, 0, 0, rc.right, rc.bottom - 400, TRUE);
		::ShowWindow(listbox_, SW_SHOWNA);
		::ShowWindow(edit_chat_word, SW_HIDE);
		::ShowWindow(send_message_btn, SW_HIDE);
		::ShowWindow(send_file_path, SW_HIDE);
		::ShowWindow(send_file_path_btn, SW_HIDE);
		::ShowWindow(recieve_file_path, SW_HIDE);
		::ShowWindow(recieve_file_path_btn, SW_HIDE);
		::ShowWindow(localtionFileEdit, SW_HIDE);
		::ShowWindow(localtionFileButton, SW_HIDE);
		::ShowWindow(oppositeFileEdit, SW_HIDE);
		::ShowWindow(oppositeFileButton, SW_HIDE);
	}
	else 
	{
		::ShowWindow(listbox_, SW_HIDE);
		::ShowWindow(edit_chat_word, SW_SHOWNA);
		::ShowWindow(send_message_btn, SW_SHOWNA);
		::ShowWindow(send_file_path, SW_SHOWNA);
		::ShowWindow(send_file_path_btn, SW_SHOWNA);
		::ShowWindow(recieve_file_path, SW_SHOWNA);
		::ShowWindow(recieve_file_path_btn, SW_SHOWNA);
		::ShowWindow(localtionFileEdit, SW_SHOWNA);
		::ShowWindow(localtionFileButton, SW_SHOWNA);
		::ShowWindow(oppositeFileEdit, SW_SHOWNA);
		::ShowWindow(oppositeFileButton, SW_SHOWNA);
	}
	InvalidateRect(wnd_, NULL, TRUE);
}

void MainWnd::HandleTabbing() {
	bool shift = ((::GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
	UINT next_cmd = shift ? GW_HWNDPREV : GW_HWNDNEXT;
	UINT loop_around_cmd = shift ? GW_HWNDLAST : GW_HWNDFIRST;
	HWND focus = GetFocus(), next;
	do {
		next = ::GetWindow(focus, next_cmd);
		if (IsWindowVisible(next) &&
			(GetWindowLong(next, GWL_STYLE) & WS_TABSTOP)) {
			break;
		}

		if (!next) {
			next = ::GetWindow(focus, loop_around_cmd);
			if (IsWindowVisible(next) &&
				(GetWindowLong(next, GWL_STYLE) & WS_TABSTOP)) {
				break;
			}
		}
		focus = next;
	} while (true);
	::SetFocus(next);
}

void MainWnd::RecieveDataChannel(const wchar_t *message)
{
	if (!message)
		return;
	if (!edit_chat_word)
		return;
	::SetWindowText(edit_chat_word, message);
}


void MainWnd::OnConnectToPeer(const int & state)
{

}

void MainWnd::OnConnectTurnServer(const int & state)
{
	int a = 0;
	a++;
}

///////////
SingleWnd::SingleWnd(const char* server, const char* chatdefaultStr, int port, bool auto_connect, bool auto_call)
	:MainWnd(server, chatdefaultStr, port, auto_connect, auto_call)
{
	//m_thread = new rtc::Win32Thread();
}

SingleWnd::~SingleWnd()
{
	/*if (!m_thread->IsQuitting())
		m_thread->Quit();
	DeletePtr(m_thread);*/
}


void SingleWnd::OnDefaultAction()
{
	/*if (!callback_)
		return;*/
	std::wstring control_button_string = L"";
	std::wstring state_connect_str = L"";
	if (ui_ == CONNECT_TO_SERVER)
	{
		CreateP2P();
		RegisterOutter(callback_, this);
		CThread::Start();
		ui_ = DISCONNECT_TO_TURNSERVER;
		control_button_string = disconnect_turnserver_str;
		state_connect_str = L"未连接....";
	}
	else
	{
		CloseTurnServerConnection(callback_);
		ui_ = CONNECT_TO_SERVER;
		control_button_string = connect_turnserver_str;
		state_connect_str = L"正在连接.......";
	}
	::SetWindowText(button_, control_button_string.c_str());
	//::SetWindowText();
}

void SingleWnd::LayoutConnectUI(bool show)
{
	struct Windows {
		HWND wnd;
		const wchar_t* text;
		size_t width;
		size_t height;
	} windows[] = {
		{ label1_, L"Server" },
		{ edit1_, L"120.76.112.94" },
		{ label2_, L":" },
		{ edit2_, L"XyXyX" },
		{ button_,connect_turnserver_str },
		{ edit_chat_word,L"未连接....." },//add by  in 2016.10.20
		{ send_message_btn,L"发送文字" },
		{ send_file_path,L"双击选择文件!" },
		{ send_file_path_btn,L"发送文件" },
		{ recieve_file_path,L"" },
		{ recieve_file_path_btn,L"接收文件" },
	};
	int oneLineY = 0;
	int twoLineY = 50;
	int threeLineY = 110;
	int fourLineY = 150;
	if (show)
	{
		const size_t kSeparator = 5;
		size_t total_width = (ARRAYSIZE(windows) - 4) * kSeparator;

		for (size_t i = 0; i < ARRAYSIZE(windows) - 6; ++i) {
			CalculateWindowSizeForText(windows[i].wnd, windows[i].text,
				&windows[i].width, &windows[i].height);
			total_width += windows[i].width;
		}

		RECT rc;
		::GetClientRect(wnd_, &rc);
		size_t x = (rc.right / 2) - (total_width / 2);
		size_t y = rc.bottom / 2;
		for (size_t i = 0; i < ARRAYSIZE(windows) - 6; ++i) {
			size_t top = y - (windows[i].height / 2);
			::MoveWindow(windows[i].wnd, static_cast<int>(x), static_cast<int>(top),
				static_cast<int>(windows[i].width),
				static_cast<int>(windows[i].height),
				TRUE);
			x += kSeparator + windows[i].width;
			if (windows[i].text[0] != 'X')
				::SetWindowText(windows[i].wnd, windows[i].text);
			if (i == 4)
			{
				::ShowWindow(windows[i].wnd, SW_SHOWNA);
				::MoveWindow(windows[i].wnd, static_cast<int>(0), static_cast<int>(oneLineY),
					static_cast<int>(200),
					static_cast<int>(40),
					TRUE);
			}
			else
				::ShowWindow(windows[i].wnd, SW_HIDE);
		}
		//////chat_edit width and height
		size_t index = ARRAYSIZE(windows) - 6;
		::MoveWindow(windows[index].wnd, static_cast<int>(0), static_cast<int>(twoLineY),//100
			static_cast<int>(200),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_SHOWNA);
		/////////
		index = ARRAYSIZE(windows) - 5;
		::MoveWindow(windows[index].wnd, static_cast<int>(200), static_cast<int>(twoLineY),//100
			static_cast<int>(60),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_SHOWNA);
		////////send_file_path
		index = ARRAYSIZE(windows) - 4;
		::MoveWindow(windows[index].wnd, static_cast<int>(290), static_cast<int>(twoLineY),//100
			static_cast<int>(150),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_SHOWNA);
		/////////
		index = ARRAYSIZE(windows) - 3;
		::MoveWindow(windows[index].wnd, static_cast<int>(460), static_cast<int>(twoLineY),//100
			static_cast<int>(60),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_SHOWNA);
		/////////
		index = ARRAYSIZE(windows) - 2;
		::MoveWindow(windows[index].wnd, static_cast<int>(210)/*290*/, static_cast<int>(fourLineY),//180
			static_cast<int>(120),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_SHOWNA);


		/////////
		index = ARRAYSIZE(windows) - 1;
		::MoveWindow(windows[index].wnd, static_cast<int>(530), static_cast<int>(twoLineY),
			static_cast<int>(60),
			static_cast<int>(50),
			TRUE);
		if (windows[index].text[0] != 'X')
			::SetWindowText(windows[index].wnd, windows[index].text);
		::ShowWindow(windows[index].wnd, SW_SHOWNA);

		::SetWindowText(localtionFileEdit, L"localtion json file!");
		::MoveWindow(localtionFileEdit, static_cast<int>(0)/*40*/, static_cast<int>(fourLineY),//260
			static_cast<int>(200),
			static_cast<int>(200),
			TRUE);
		::ShowWindow(localtionFileEdit, SW_SHOWNA);
		::SetWindowText(localtionFileButton, L"获取本地信息文件");//GetLocalFileJson
		::MoveWindow(localtionFileButton, static_cast<int>(0), static_cast<int>(threeLineY),//200
			static_cast<int>(120),
			static_cast<int>(40),
			TRUE);
		::ShowWindow(localtionFileButton, SW_SHOWNA);

		::SetWindowText(oppositeFileEdit, L"opposite json file!");//
		::MoveWindow(oppositeFileEdit, static_cast<int>(340)/*340*/, static_cast<int>(fourLineY),//260
			static_cast<int>(200),
			static_cast<int>(200),/*20*/
			TRUE);
		::ShowWindow(oppositeFileEdit, SW_SHOWNA);
		::SetWindowText(oppositeFileButton, L"设置远端信息文件");//SetOppositeFileJson
		::MoveWindow(oppositeFileButton, static_cast<int>(340), static_cast<int>(threeLineY),//200
			static_cast<int>(120),
			static_cast<int>(40),
			TRUE);
		::ShowWindow(oppositeFileButton, SW_SHOWNA);

		::SetWindowText(interrupt_file_task_btn, L"中断任务");
		::MoveWindow(interrupt_file_task_btn, static_cast<int>(530), static_cast<int>(threeLineY - 10),
			static_cast<int>(60),
			static_cast<int>(50),
			TRUE);
		::ShowWindow(interrupt_file_task_btn, SW_SHOWNA);

		::SetWindowText(cancle_file_task_btn, L"取消任务");
		::MoveWindow(cancle_file_task_btn, static_cast<int>(460), static_cast<int>(threeLineY - 10),
			static_cast<int>(60),
			static_cast<int>(50),
			TRUE);
		::ShowWindow(cancle_file_task_btn, SW_SHOWNA);
		////////progress1 hwnd
		::MoveWindow(progressToolTip1, static_cast<int>(210)/*290*/, static_cast<int>(fourLineY + 60),//180
			static_cast<int>(120),
			static_cast<int>(50),
			TRUE);
		   ::SetWindowText(progressToolTip1, L"");
		   ::ShowWindow(progressToolTip1, SW_SHOWNA);
	}
	else
	{
		for (size_t i = 0; i < ARRAYSIZE(windows) - 6; ++i) {
			::ShowWindow(windows[i].wnd, SW_HIDE);
		}
		::ShowWindow(localtionFileEdit, SW_HIDE);
		::ShowWindow(localtionFileButton, SW_HIDE);
		::ShowWindow(oppositeFileEdit, SW_HIDE);
		::ShowWindow(oppositeFileButton, SW_HIDE);
		::ShowWindow(interrupt_file_task_btn, SW_HIDE);
	}
	::ShowWindow(recieve_file_path, SW_HIDE);
}



void SingleWnd::OnMesureLocaltionFile()
{
	if (!callback_)
		return;
	char info[10000] = {0};
	GetLocaltionSdpIcecandidate(callback_, info);
	::SetWindowTextA(localtionFileEdit, info);
	::MessageBoxA(NULL, "Get Localtion Information File Successfully!","",MB_OK);
}

void SingleWnd::OnMesureOppositeFile()
{
	//"120.76.112.94", 3478, "helloword", "helloword", ""
	//callback_ = CreateP2PEnvironment("120.76.112.94", 3478, "helloword", "helloword", "");
	callback_ = CreateP2PEnvironment("58.63.39.43", 3478, "yiluxiangxi", "gzmiracle@123this", "realm");
	RegisterOutter(callback_, this);//"58.63.39.43", 3478, "yiluxiangxi", "gzmiracle@123this", "realm"

	std::string filepath1 = GetWindowText(oppositeFileEdit);/* "d:\\localtionJson_p.json"*/
	int size = filepath1.size();
	char *filepath = new char[size + 1];
	memset(filepath, 0, size + 1);
	memcpy(filepath, filepath1.c_str(),size);
	filepath[filepath1.size()] = '\0';
	SetOppositeSdpIcecandidate(callback_, filepath);
	delete filepath;
	filepath = NULL;
	::MessageBoxA(NULL, "Set Opposite Information File Successfully!", "", MB_OK);
}


void SingleWnd::OnConnectToPeer(const int & state)
{
	std::string str = "";
	switch (state)
	{
	case Connect_Success:
	{
		str = "ConnectToPeer Successfully!";
	}
	break;
	case Connect_Failure:
	{
		str = "ConnectToPeer Failure!";
		//CloseTurnServerConnection(callback_);
	}
	break;
	default:
		break;
	}
	::SetWindowTextA(edit_chat_word, str.c_str());
}

void SingleWnd::OnConnectTurnServer(const int & state)
{
	std::wstring tips = L"";
	switch (state)
	{
	case Connect_Success:
	{
		tips = L"连接成功";
		ui_ = DISCONNECT_TO_TURNSERVER;
	}
	break;
	case Connect_Failure:
	{
		tips = L"连接";
		ui_ = CONNECT_TO_SERVER;
	}
	default:
		break;
	}
	::SetWindowText(button_, tips.c_str());
}

//void SingleWnd::OnFileCancel(const char * param)
//{
//	const char * taskGuid = static_cast<const char *>(param);
//	int a = 0;
//	a++;
//}

void SingleWnd::CreateP2P()
{
	//callback_ = CreateP2PEnvironment("120.76.112.94", 3478, "helloword", "helloword", "");
	callback_ = CreateP2PEnvironment("58.63.39.43", 3478, "yiluxiangxi", "gzmiracle@123this", "realm");
}

void SingleWnd::run()
{
	CreateTurnServerConnection(callback_);
}
////////////////////
SingleSendWnd::SingleSendWnd(const char* server, const char* chatdefaultStr, int port, bool auto_connect, bool auto_call)
	:SingleWnd(server, chatdefaultStr, port, auto_connect,auto_call)
{

}
/////////////////////
SingleRecieveWnd::SingleRecieveWnd(const char* server, const char* chatdefaultStr, int port, bool auto_connect, bool auto_call)
	: SingleWnd(server, chatdefaultStr, port, auto_connect, auto_call)
{
	//connect_turnserver_str = L"断开";
	memset(connect_turnserver_str, 0 , sizeof(connect_turnserver_str));
	memcpy(connect_turnserver_str, L"断开", sizeof(L"断开"));//set the windows text
}

void SingleRecieveWnd::OnDefaultAction()
{
	if (!callback_)
		return;
	CloseTurnServerConnection(callback_);
	::SetWindowText(button_, disconnect_turnserver_str);
}
////////////////////
bool RectIsEmpty(RECT & rect)
{
	if (rect.left == 0 && rect.right == 0 && rect.top == 0 && rect.bottom == 0)
		return true;
	return false;
}