// WeaselIME.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <ResponseParser.h>
#include "WeaselIME.h"

HINSTANCE WeaselIME::_hModule = 0;
map<HIMC, shared_ptr<WeaselIME> > WeaselIME::_instances;
boost::mutex WeaselIME::_mutex;


static bool launch_server()
{
	WCHAR WEASEL[] = L"小狼毫";

	// 從註冊表取得server位置
	HKEY hKey;
	LSTATUS ret = RegOpenKey(HKEY_LOCAL_MACHINE, WeaselIME::GetRegKey(), &hKey);
	if (ret != ERROR_SUCCESS)
	{
		MessageBox(NULL, L"註冊表信息無影了", WEASEL, MB_ICONERROR | MB_OK);
		return false;
	}

	WCHAR value[MAX_PATH];
	DWORD len = sizeof(value);
	DWORD type = 0;
	ret = RegQueryValueEx(hKey, L"WeaselRoot", NULL, &type, (LPBYTE)value, &len);
	if (ret != ERROR_SUCCESS)
	{
		MessageBox(NULL, L"未設置 WeaselRoot", WEASEL, MB_ICONERROR | MB_OK);
		RegCloseKey(hKey);
		return false;
	}
	wpath weaselRoot(value);

	len = sizeof(value);
	type = 0;
	ret = RegQueryValueEx(hKey, L"ServerExecutable", NULL, &type, (LPBYTE)value, &len);
	if (ret != ERROR_SUCCESS)
	{
		MessageBox(NULL, L"未設置 ServerExecutable", WEASEL, MB_ICONERROR | MB_OK);
		RegCloseKey(hKey);
		return false;
	}
	wpath serverPath(weaselRoot / value);

	RegCloseKey(hKey);

	// 啓動服務進程
	wstring exe = serverPath.native_file_string();
	wstring dir = weaselRoot.native_file_string();
	int retCode = (int)ShellExecute(NULL, L"open", exe.c_str(), NULL, dir.c_str(), SW_HIDE);
	if (retCode <= 32)
	{
		MessageBox(NULL, L"服務進程啓動不起來 :(", WEASEL, MB_ICONERROR | MB_OK);
		return false;
	}
	return true;
}

WeaselIME::WeaselIME(HIMC hIMC) 
	: m_hIMC(hIMC), m_alwaysDetectCaretPos(false)
{
	WCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, _countof(path));
	wstring exe = wpath(path).filename();
	if (boost::iequals(L"chrome.exe", exe))
		m_alwaysDetectCaretPos = true;
}

LPCWSTR WeaselIME::GetIMEName()
{
	return L"中州韻輸入法引擎";
}

LPCWSTR WeaselIME::GetIMEFileName()
{
	return L"weasel.ime";
}

LPCWSTR WeaselIME::GetRegKey()
{
	return L"Software\\ZIME\\Weasel";
}

HINSTANCE WeaselIME::GetModuleInstance()
{
	return _hModule;
}

void WeaselIME::SetModuleInstance(HINSTANCE hModule)
{
	_hModule = hModule;
}

HRESULT WeaselIME::RegisterUIClass()
{
	WNDCLASSEX wc;
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_IME;
	wc.lpfnWndProc    = WeaselIME::UIWndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 2 * sizeof(LONG);
	wc.hInstance      = GetModuleInstance();
	wc.hCursor        = NULL;
	wc.hIcon          = NULL;
	wc.lpszMenuName   = NULL;
	wc.lpszClassName  = GetUIClassName();
	wc.hbrBackground  = NULL;
	wc.hIconSm        = NULL;

	if (RegisterClassExW(&wc) == 0)
	{
		DWORD dwErr = GetLastError();
		return HRESULT_FROM_WIN32(dwErr);
	}
	
	return S_OK;
}

HRESULT WeaselIME::UnregisterUIClass()
{
	if (!UnregisterClassW(GetUIClassName(), GetModuleInstance()))
	{
		DWORD dwErr = GetLastError();
		return HRESULT_FROM_WIN32(dwErr);
	}
	return S_OK;
}

LPCWSTR WeaselIME::GetUIClassName()
{
	return L"WeaselUIClass";
}

LRESULT WINAPI WeaselIME::UIWndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
	HIMC hIMC = (HIMC)GetWindowLongPtr(hWnd, 0);
	if (hIMC)
	{
		shared_ptr<WeaselIME> p = WeaselIME::GetInstance(hIMC);
		if (!p)
			return 0;
		return p->OnUIMessage(hWnd, uMsg, wp, lp);
	}
	else
	{
		if (!IsIMEMessage(uMsg))
		{
			return DefWindowProcW(hWnd, uMsg, wp, lp);
		}
	}

	return 0;
}

BOOL WeaselIME::IsIMEMessage(UINT uMsg)
{
	switch(uMsg)
	{
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_COMPOSITION:
	case WM_IME_NOTIFY:
	case WM_IME_SETCONTEXT:
	case WM_IME_CONTROL:
	case WM_IME_COMPOSITIONFULL:
	case WM_IME_SELECT:
	case WM_IME_CHAR:
		return TRUE;
	default:
		return FALSE;
	}

	return FALSE;
}

shared_ptr<WeaselIME> WeaselIME::GetInstance(HIMC hIMC)
{
	boost::lock_guard<boost::mutex> lock(_mutex);
	shared_ptr<WeaselIME>& p = _instances[hIMC];
	if (!p)
	{
		p.reset(new WeaselIME(hIMC));
	}
	return p;
}

void WeaselIME::Cleanup()
{
	boost::lock_guard<boost::mutex> lock(_mutex);
	for (map<HIMC, shared_ptr<WeaselIME> >::const_iterator i = _instances.begin(); i != _instances.end(); ++i)
	{
		shared_ptr<WeaselIME> p = i->second;
		p->OnIMESelect(FALSE);
	}
	_instances.clear();
}

LRESULT WeaselIME::OnIMESelect(BOOL fSelect)
{
	if (fSelect)
	{
		if (!m_ui.Create(NULL))
			return 0;
		
		// initialize weasel client
		m_client.Connect(launch_server);
		m_client.StartSession();

		wstring ignored;
		m_ctx.clear();
		m_status.reset();
		weasel::ResponseParser parser(ignored, m_ctx, m_status);
		m_client.GetResponseData(boost::ref(parser));

		m_ui.UpdateContext(m_ctx);
		//m_ui.UpdateStatus(m_status);
		
		return _Initialize();
	}
	else
	{
		m_client.EndSession();
		m_ui.Destroy();

		return _Finalize();
	}
}

LRESULT WeaselIME::OnIMEFocus(BOOL fFocus)
{
	if (fFocus)
	{
		LPINPUTCONTEXT lpIMC = ImmLockIMC(m_hIMC);
		if(lpIMC)
		{
			if(!(lpIMC->fdwInit & INIT_COMPFORM))
			{
				lpIMC->cfCompForm.dwStyle = CFS_DEFAULT;
				GetCursorPos(&lpIMC->cfCompForm.ptCurrentPos);
				ScreenToClient(lpIMC->hWnd, &lpIMC->cfCompForm.ptCurrentPos);
				lpIMC->fdwInit |= INIT_COMPFORM;
			}
			ImmUnlockIMC(m_hIMC);
		}
		
		if (!m_ctx.empty())
			m_ui.Show();
	}
	else
	{
		m_ui.Hide();
	}

	return 0;
}

LRESULT WeaselIME::OnUIMessage(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
	LPINPUTCONTEXT lpIMC = (LPINPUTCONTEXT)ImmLockIMC(m_hIMC);
	switch (uMsg)
	{
	case WM_IME_NOTIFY:
		{
			_OnIMENotify(lpIMC, wp, lp);
		}
		break;
	case WM_IME_SELECT:
		{
			// detect caret pos
			POINT pt = {-1, -1};
			this->_UpdateInputPosition(lpIMC, pt);
		}
		break;
	default:
		if (!IsIMEMessage(uMsg)) 
		{
			ImmUnlockIMC(m_hIMC);
			return DefWindowProcW(hWnd, uMsg, wp, lp);
		}
	}

	ImmUnlockIMC(m_hIMC);
	return 0;
}

LRESULT WeaselIME::_OnIMENotify(LPINPUTCONTEXT lpIMC, WPARAM wp, LPARAM lp)
{			
	switch (wp)
	{
	case IMN_SETCANDIDATEPOS:
		{
			POINT pt = lpIMC->cfCandForm[0].ptCurrentPos;
			_UpdateInputPosition(lpIMC, pt);
		}

	case IMN_SETCOMPOSITIONWINDOW:
		{
			POINT pt = {-1, -1};
			switch (lpIMC->cfCompForm.dwStyle)
			{
			case CFS_DEFAULT:
				// require caret pos detection
				break;
			default:
				pt = lpIMC->cfCompForm.ptCurrentPos;
				break;
			}
			_UpdateInputPosition(lpIMC, pt);
		}
		break;
	}

	return 0;
}

BOOL WeaselIME::ProcessKeyEvent(UINT vKey, KeyInfo kinfo, const LPBYTE lpbKeyState)
{
	bool taken = false;

#ifdef KEYCODE_VIEWER
	{
		weasel::KeyEvent ke;
		if (!ConvertKeyEvent(vKey, kinfo, lpbKeyState, ke))
		{
			// unknown key event
			m_ctx.clear();
			m_ctx.aux.str = (boost::wformat(L"unknown key event vKey: %x, scanCode: %x, isKeyUp: %u") % vKey % kinfo.scanCode % kinfo.isKeyUp).str();
			return FALSE;
		}
		m_ctx.aux.str = (boost::wformat(L"keycode: %x, mask: %x, isKeyUp: %u") % ke.keycode % ke.mask % kinfo.isKeyUp).str();
		_UpdateContext(m_ctx);
		return FALSE;
	}
#endif

	// 要处理KEY_UP事件（宫保拼音用KEY_UP）
	//if (kinfo.isKeyUp)
	//{
	//	return FALSE;
	//}

	if (!m_client.Echo())
	{
		m_client.Connect(launch_server);
		m_client.StartSession();
	}
	
	weasel::KeyEvent ke;
	if (!ConvertKeyEvent(vKey, kinfo, lpbKeyState, ke))
	{
		// unknown key event
		return FALSE;
	}

	taken = m_client.ProcessKeyEvent(ke);

	wstring commit;
	weasel::ResponseParser parser(commit, m_ctx, m_status);
	bool ok = m_client.GetResponseData(boost::ref(parser));
	if (!ok)
	{
		// may suffer loss of data...
		m_ctx.aux.clear();
		m_ctx.aux.str = L"未能完整讀取候選信息！";
		//return TRUE;
	}

	if (!commit.empty())
	{
		if (!m_status.composing)
		{
			_StartComposition();
		}
		_EndComposition(commit.c_str());
		m_status.composing = false;
	}

	if (!m_ctx.preedit.empty())
	{
		if (!m_status.composing)
		{
			_StartComposition();
			m_status.composing = true;
		}
	}
	else
	{
		if (m_status.composing)
		{
			_EndComposition(L"");
			m_status.composing = false;
		}
	}

	_UpdateContext(m_ctx);

	return (BOOL)taken;
}

HRESULT WeaselIME::_Initialize()
{
	LPINPUTCONTEXT lpIMC = ImmLockIMC(m_hIMC);
	if(!lpIMC)
		return E_FAIL;

	lpIMC->fOpen = TRUE;

	HIMCC& hIMCC = lpIMC->hCompStr;
	if (!hIMCC)
		hIMCC = ImmCreateIMCC(sizeof(CompositionInfo));
	else
		hIMCC = ImmReSizeIMCC(hIMCC, sizeof(CompositionInfo));
	if(!hIMCC)
	{
		ImmUnlockIMC(m_hIMC);
		return E_FAIL;
	}

	CompositionInfo* pInfo = (CompositionInfo*)ImmLockIMCC(hIMCC);
	if (!pInfo)
	{
		ImmUnlockIMC(m_hIMC);
		return E_FAIL;
	}

	pInfo->Reset();
	ImmUnlockIMCC(hIMCC);
	ImmUnlockIMC(m_hIMC);

	return S_OK;
}

HRESULT WeaselIME::_Finalize()
{
	LPINPUTCONTEXT lpIMC = ImmLockIMC(m_hIMC);
	if (lpIMC)
	{
		lpIMC->fOpen = FALSE;
		if (lpIMC->hCompStr)
		{
			ImmDestroyIMCC(lpIMC->hCompStr);
			lpIMC->hCompStr = NULL;
		}
	}
	ImmUnlockIMC(m_hIMC);

	return S_OK;
}

HRESULT WeaselIME::_StartComposition()
{
	_AddIMEMessage(WM_IME_STARTCOMPOSITION, 0, 0);
	_AddIMEMessage(WM_IME_NOTIFY, IMN_OPENCANDIDATE, 0);

	return S_OK;
}

HRESULT WeaselIME::_EndComposition(LPCWSTR composition)
{
	LPINPUTCONTEXT lpIMC;
	LPCOMPOSITIONSTRING lpCompStr;

	lpIMC = ImmLockIMC(m_hIMC);
	if (!lpIMC)
		return E_FAIL;

	lpCompStr = (LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
	if (!lpCompStr)
	{
		ImmUnlockIMC(m_hIMC);
		return E_FAIL;
	}

	CompositionInfo* pInfo = (CompositionInfo*)lpCompStr;
	wcscpy_s(pInfo->szResultStr, composition);
	lpCompStr->dwResultStrLen = wcslen(pInfo->szResultStr);
	
	ImmUnlockIMCC(lpIMC->hCompStr);
	ImmUnlockIMC(m_hIMC);

	_AddIMEMessage(WM_IME_COMPOSITION, 0, GCS_COMP|GCS_RESULTSTR);
	_AddIMEMessage(WM_IME_ENDCOMPOSITION, 0, 0);
	_AddIMEMessage(WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 0);
	
	return S_OK;
}

HRESULT WeaselIME::_AddIMEMessage(UINT msg, WPARAM wp, LPARAM lp)
{
	if(!m_hIMC)
		return S_FALSE;

	LPINPUTCONTEXT lpIMC = (LPINPUTCONTEXT)ImmLockIMC(m_hIMC);
	if(!lpIMC)
		return E_FAIL;

	HIMCC hBuf = ImmReSizeIMCC(lpIMC->hMsgBuf, 
							   sizeof(TRANSMSG) * (lpIMC->dwNumMsgBuf + 1));
	if(!hBuf)
	{
		ImmUnlockIMC(m_hIMC);
		return E_FAIL;
	}
	lpIMC->hMsgBuf = hBuf;

	LPTRANSMSG pBuf = (LPTRANSMSG)ImmLockIMCC(hBuf);
	if(!pBuf)
	{
		ImmUnlockIMC(m_hIMC);
		return E_FAIL;
	}

	DWORD last = lpIMC->dwNumMsgBuf;
	pBuf[last].message = msg;
	pBuf[last].wParam = wp;
	pBuf[last].lParam = lp;
	lpIMC->dwNumMsgBuf++;
	ImmUnlockIMCC(hBuf);

	ImmUnlockIMC(m_hIMC);

	if (!ImmGenerateMessage(m_hIMC))
	{
		return E_FAIL;
	}

	return S_OK;
}

void WeaselIME::_UpdateInputPosition(LPINPUTCONTEXT lpIMC, POINT pt)
{
	if (m_alwaysDetectCaretPos || pt.x == -1 && pt.y == -1)
	{
		// caret pos detection required
		GUITHREADINFO threadInfo;
		threadInfo.cbSize = sizeof(GUITHREADINFO);
		BOOL ret = GetGUIThreadInfo(GetCurrentThreadId(), &threadInfo);
		if (ret && IsWindow(threadInfo.hwndCaret) && IsWindowVisible(threadInfo.hwndCaret))
		{
			pt.x = threadInfo.rcCaret.left;
			pt.y = threadInfo.rcCaret.top;
		}
		else
			pt.x = pt.y = 0;
	}

	ClientToScreen(lpIMC->hWnd, &pt);
	int height = abs(lpIMC->lfFont.W.lfHeight);
	if (height == 0)
	{
		HDC hDC = GetDC(lpIMC->hWnd);
		SIZE sz = {0};
		GetTextExtentPoint(hDC, L"A", 1, &sz);
		height = sz.cy;
		ReleaseDC(lpIMC->hWnd, hDC);
	}
	const int width = 6;
	RECT rc;
	SetRect(&rc, pt.x, pt.y, pt.x + width, pt.y + height);
	m_ui.UpdateInputPosition(rc);
}

void WeaselIME::_UpdateContext(weasel::Context const& ctx)
{
	if (!ctx.empty())
	{
		m_ui.UpdateContext(m_ctx);
		m_ui.Show();
	}
	else
	{
		m_ui.Hide();
		m_ui.UpdateContext(m_ctx);
	}
}
