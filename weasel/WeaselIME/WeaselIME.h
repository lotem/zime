#pragma once
#include <WeaselIPC.h>
#include <WeaselUI.h>
#include "KeyEvent.h"

#define MAX_COMPOSITION_SIZE 256

struct CompositionInfo
{
	COMPOSITIONSTRING cs;	
	WCHAR szCompStr[MAX_COMPOSITION_SIZE];
	WCHAR szResultStr[MAX_COMPOSITION_SIZE];
	void Reset()
	{
		memset(this, 0, sizeof(*this));
		cs.dwSize = sizeof(*this);
		cs.dwCompStrOffset = (char*)&szCompStr - (char*)this;
		cs.dwResultStrOffset = (char*)&szResultStr - (char*)this;
	}
};

typedef struct _tagTRANSMSG {
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
} TRANSMSG, *LPTRANSMSG;

class WeaselIME
{
public:
	static LPCWSTR GetIMEName();
	static LPCWSTR GetIMEFileName();
	static LPCWSTR GetRegKey();
	static LPCWSTR GetRegValueName();
	static HINSTANCE GetModuleInstance();
	static void SetModuleInstance(HINSTANCE hModule);
	static HRESULT RegisterUIClass();
	static HRESULT UnregisterUIClass();
	static LPCWSTR GetUIClassName();
	static LRESULT WINAPI UIWndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);
	static BOOL IsIMEMessage(UINT uMsg);
	static boost::shared_ptr<WeaselIME> GetInstance(HIMC hIMC);
	static void Cleanup();

	WeaselIME(HIMC hIMC) : m_hIMC(hIMC) {}
	LRESULT OnIMESelect(BOOL fSelect);
	LRESULT OnIMEFocus(BOOL fFocus);
	LRESULT OnUIMessage(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);
	BOOL ProcessKeyEvent(UINT vKey, const KeyInfo kinfo, const LPBYTE lpbKeyState);

private:
	HRESULT _Initialize();
	HRESULT _Finalize();
	LRESULT _OnIMENotify(LPINPUTCONTEXT lpIMC, WPARAM wp, LPARAM lp);
	HRESULT _StartComposition();
	HRESULT _EndComposition(LPCWSTR composition);
	HRESULT _AddIMEMessage(UINT msg, WPARAM wp, LPARAM lp);
	void _UpdateInputPosition(LPINPUTCONTEXT lpIMC, POINT pt);
	void _UpdateContext(weasel::Context const& ctx);

private:
	static HINSTANCE _hModule;
	static std::map<HIMC, boost::shared_ptr<WeaselIME> > _instances;
	static boost::mutex _mutex;
	HIMC m_hIMC;
	weasel::UI m_ui;
	weasel::Client m_client;
	weasel::Context m_ctx;
	weasel::Status m_status;
};
