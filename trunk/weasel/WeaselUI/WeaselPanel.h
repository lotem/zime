#pragma once
#include <WeaselCommon.h>

static const int FONT_POINT_SIZE = 12;
static LPCWSTR FONT_FACE = L"Microsoft YaHei";

static const int BORDER = 2;
static const int X_MARGIN = 12;
static const int Y_MARGIN = 12;
static const int SPACING = 6;
static const int HIGHLIGHT_PADDING_TOP = 1;
static const int HIGHLIGHT_PADDING_BOTTOM = 2;

typedef CWinTraits<WS_POPUP|WS_CLIPSIBLINGS|WS_DISABLED, WS_EX_TOOLWINDOW|WS_EX_TOPMOST> CWeaselPanelTraits;

class WeaselPanel : 
	public CWindowImpl<WeaselPanel, CWindow, CWeaselPanelTraits>
{
public:

	BEGIN_MSG_MAP(WeaselPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void CloseDialog(int nVal);
	void MoveTo(RECT const& rc);

private:
	CSize _GetWindowSize();
	void _Refresh();

	void DoPaint(CDCHandle dc);
	weasel::Context m_ctx;
	weasel::Status m_status;

public:
	WeaselPanel();
	~WeaselPanel(){};	
	void SetContext(const weasel::Context &ctx);
	void SetStatus(const weasel::Status &status);
};
