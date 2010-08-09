#pragma once
#include <WeaselIPC.h>
#include <WeaselUI.h>

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

struct KeyInfo
{
	UINT repeatCount: 16;
	UINT scanCode: 8;
	UINT isExtended: 1;
	UINT reserved: 4;
	UINT contextCode: 1;
	UINT prevKeyState: 1;
	UINT isKeyUp: 1;

	static inline KeyInfo Create(LPARAM lparam)
	{
		return *(KeyInfo*)&lparam;
	}
};

class WeaselIME
{
public:
	static LPCWSTR GetIMEName();
	static LPCWSTR GetIMEFileName();
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
	BOOL ProcessKeyEvent(UINT vKey, KeyInfo kinfo, CONST LPBYTE lpbKeyState);

private:
	HRESULT _Initialize();
	HRESULT _Finalize();
	LRESULT _OnIMENotify(LPINPUTCONTEXT lpIMC, WPARAM wp, LPARAM lp);
	HRESULT _StartComposition();
	HRESULT _EndComposition(LPCWSTR composition);
	HRESULT _AddIMEMessage(UINT msg, WPARAM wp, LPARAM lp);
	void _UpdateInputPosition(LPINPUTCONTEXT lpIMC, POINT pt);
	void _UpdateContext(wstring const& composition);

private:
	static HINSTANCE _hModule;
	static std::map<HIMC, boost::shared_ptr<WeaselIME> > _instances;
	static boost::mutex _mutex;
	HIMC m_hIMC;
	WeaselUI m_ui;
	WeaselClient m_client;
	weasel::Context m_ctx;
	weasel::Status m_status;
};

// TODO: make these private memeber functions
bool read_buffer(LPWSTR buffer, UINT length, std::wstring& dest);
bool launch_server();
