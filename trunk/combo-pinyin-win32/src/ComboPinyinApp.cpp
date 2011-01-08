// ComboPinyin.cpp
//
// ComboPinyin Source
//
// Copyright (C) 2007
//
// Author: GONG Chen
// E-mail: contralisk@mail.com
// Project Home: http://zime.googlecode.com
// Licence: GPL v3
//
//////////////////////////////////////////////////////////////////////

// combo-pinyin.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ComboPinyinApp.h"
#include "ComboPinyinTrayIcon.h"
#include "ComboPinyin.h"
#include "EventDef.h"

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
HANDLE hMutex;									// handle to AppEntry mutex

TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
TCHAR sz_zhung[MAX_LOADSTRING];
TCHAR sz_alpha[MAX_LOADSTRING];
TCHAR sz_layout[MAX_LOADSTRING];
UINT layout_len;

ComboPinyin module;
ComboPinyinTrayIcon tray_icon(module);

void event_handler(UINT event_id, UINT param)
{
	bool enabled;
	LPCTSTR text = NULL;
	static TCHAR buffer[MAX_LOADSTRING];

	switch(event_id)
	{
	case TOGGLE:
		enabled = (param != 0);
		text = (enabled ? sz_zhung : sz_alpha);
		break;

	case LAYOUT_CHANGE:
		enabled = module.is_enabled();
		_tcscpy(buffer, sz_layout);
		_tcsncpy(
			buffer + layout_len, 
			module.get_config_title(module.get_active_config()).c_str(), 
			MAX_LOADSTRING - 1 - layout_len
		);
		buffer[MAX_LOADSTRING - 1] = _T('\0');

		// remove trailing \(&.\)
		{
			size_t len = _tcslen(buffer);
			if (len > 4 &&
				buffer[len - 4] == _T('(') &&
				buffer[len - 3] == _T('&') && 
				buffer[len - 1] == _T(')'))
			{
				_tcsnset(buffer + (len - 4), _T('\0'), 4);
			}
		}
		
		text = buffer;
		break;

	default:
		break;
	}

	if (text == NULL)
	{
		_tcscpy(buffer, _T("..."));
		text = buffer;
	}

	tray_icon.SetIcon( enabled ? IDI_ZH : IDI_ALPHA);
	tray_icon.ShowBalloon(text, szTitle);
}

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	                   HINSTANCE hPrevInstance,
	                   LPTSTR    lpCmdLine,
	                   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// check for existing instance of ComboPinyin app
	HANDLE hMutex = CreateMutex(NULL, FALSE, COMBO_PINYIN_MUTEX_NAME);
	DWORD last_error = GetLastError();
	if (last_error == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		hMutex = NULL;
		return FALSE;
	}

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_COMBO_PINYIN, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_ZH, sz_zhung, MAX_LOADSTRING);
	LoadString(hInstance, IDS_ALPHA, sz_alpha, MAX_LOADSTRING);
	layout_len = LoadString(hInstance, IDS_LAYOUT_CHANGES, sz_layout, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SKENOTYPE));

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COMBO_PINYIN));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_COMBO_PINYIN);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_COMBO_PINYIN));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
	   return FALSE;
	}

	// Create the tray icon
	if (!tray_icon.Create(hInstance,
	                      hWnd,                   // Parent window
	                      WM_ICON_NOTIFY,         // Icon notify message to use
	                      szTitle,				  // tooltip
	                      ::LoadIcon(hInstance, (LPCTSTR)IDI_ALPHA),
	                      IDR_POPUP_MENU)) 
	{
		return FALSE;
	}

	module.create(hInstance, hWnd);
	module.connect(&event_handler);

	module.initialize();

	//ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);

	return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_ICON_NOTIFY:
		return tray_icon.OnTrayNotification(wParam, lParam);

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_TOGGLE:
			module.toggle();
			break;
		case IDM_RELOAD:
			module.initialize();
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			module.destroy();
			tray_icon.RemoveIcon();
			DestroyWindow(hWnd);
			break;
		default:
			if (wmId >= IDM_LAYOUT && wmId < IDM_LAYOUT + MAX_LAYOUTS)
			{
				module.configure(wmId - IDM_LAYOUT);
				break;
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// noop...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
