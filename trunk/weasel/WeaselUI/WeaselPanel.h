#pragma once
#include <WeaselCommon.h>

static const int FONT_POINT_SIZE = 16;

static const int BORDER = 3;
static const int X_MARGIN = 18;
static const int Y_MARGIN = 12;
static const int SPACING = 4;

typedef CWinTraits<WS_BORDER|WS_POPUP|WS_CLIPSIBLINGS|WS_DISABLED, WS_EX_TOOLWINDOW|WS_EX_TOPMOST> CWeaselPanelTraits;

class WeaselPanel : 
	public CWindowImpl<WeaselPanel, CWindow, CWeaselPanelTraits>
{
public:

	BEGIN_MSG_MAP(WeaselPanel)
		MESSAGE_HANDLER(WM_CREATE, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void CloseDialog(int nVal);

private:
	void _UpdateUI();
	void DoPaint(CDCHandle /*dc*/);
	weasel::ZIMEInfo m_Info;
	weasel::ZIMEStatus m_Status;

public:
	WeaselPanel();
	~WeaselPanel(){};	
	void SetContent(const weasel::ZIMEInfo &info);
	void SetStatus(const weasel::ZIMEStatus &status);
};
