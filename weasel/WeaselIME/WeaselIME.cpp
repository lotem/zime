// WeaselIME.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "WeaselIME.h"

HINSTANCE WeaselIME::_hModule = 0;
map<HIMC, shared_ptr<WeaselIME> > WeaselIME::_instances;
boost::mutex WeaselIME::_mutex;


bool launch_server()
{
	//// TODO: 暂写定一个路径 应改为从注册表读取安装目录
	int ret = (int)ShellExecute( NULL, L"open", L"D:\\home\\devel\\weasel\\Debug\\TestWeaselIPC.exe", L"/start", NULL, SW_HIDE );
	if (ret <= 32)
	{
		MessageBox(NULL, L"服務進程啓動不起來:(", L"小狼毫", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

bool read_context(LPWSTR buffer, UINT length, weasel::Context& ctx)
{
	wbufferstream bs(buffer, length);
	// TODO: parse context data
	wstring line;
	getline(bs, line);
	if (!bs.good())
		return false;
	ctx.preedit = line;

	return bs.good();
}

LPCWSTR WeaselIME::GetIMEName()
{
	return L"中州韻輸入法引擎";
}

LPCWSTR WeaselIME::GetIMEFileName()
{
	return L"weasel.ime";
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
		// initialize weasel client
		m_client.Connect(launch_server);
		m_client.StartSession();
		
		if (!m_ui.Create(NULL))
			return 0;
		
		m_ui.UpdateStatus(m_status);
		
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
	case IMN_OPENCANDIDATE:
		{
			POINT pt = lpIMC->cfCompForm.ptCurrentPos;
			_UpdateInputPosition(lpIMC, pt);
		}
		break;

	case IMN_SETCANDIDATEPOS:
		{
			POINT pt = lpIMC->cfCandForm[0].ptCurrentPos;
			_UpdateInputPosition(lpIMC, pt);
		}

	case IMN_SETCOMPOSITIONWINDOW:
		{
			POINT pt;
			switch (lpIMC->cfCompForm.dwStyle)
			{
			case CFS_DEFAULT:  // Excel
				GetCaretPos(&pt);
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
	BOOL bEaten = FALSE;

	// TODO: 暂不处理KEY_UP事件（宫保拼音用KEY_UP）
	if (kinfo.isKeyUp)
	{
		return FALSE;
	}

	if (!m_client.Echo())
	{
		m_client.Connect(launch_server);
		m_client.StartSession();
	}
	
	if (m_client.ProcessKeyEvent(weasel::KeyEvent(vKey, 0)))
	{
		weasel::Context result;
		m_client.GetResponseData(boost::bind<bool>(read_context, _1, _2, boost::ref(result)));

		if (m_ctx.empty())
		{
			if (!result.empty())
				_StartComposition();
		}
		else
		{
			if (result.empty())
				_EndComposition(m_ctx.preedit.str.c_str());
		}
		_UpdateContext(result);

		bEaten = TRUE;
	}
	
	return bEaten;
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
		m_ctx.preedit = ctx.preedit;
		m_ctx.aux = weasel::Text(L"testing");
		m_ui.UpdateContext(m_ctx);
		m_ui.Show();
	}
	else
	{
		m_ctx.clear();
		m_ui.Hide();
		m_ui.UpdateContext(m_ctx);
	}
}
