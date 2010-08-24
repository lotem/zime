#pragma once
#include <WeaselCommon.h>

static const int FONT_POINT_SIZE = 12;
static LPCWSTR FONT_FACE = L"Microsoft YaHei";

static const int MIN_WIDTH = 200;
static const int MIN_HEIGHT = 0;
static const int BORDER = 2;
static const int MARGIN_X = 12;
static const int MARGIN_Y = 10;
static const int SPACING = 2;
static const int HIGHLIGHT_PADDING_TOP = 0;
static const int HIGHLIGHT_PADDING_BOTTOM = 1;

typedef CWinTraits<WS_POPUP|WS_CLIPSIBLINGS|WS_DISABLED, WS_EX_TOOLWINDOW|WS_EX_TOPMOST> CWeaselPanelTraits;

class WeaselPanel : 
	public CWindowImpl<WeaselPanel, CWindow, CWeaselPanelTraits>,
	CDoubleBufferImpl<WeaselPanel>
{
public:
	BEGIN_MSG_MAP(WeaselPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CDoubleBufferImpl<WeaselPanel>)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void CloseDialog(int nVal);

	WeaselPanel() {}
	~WeaselPanel() {}
	void SetContext(const weasel::Context &ctx);
	void SetStatus(const weasel::Status &status);
	void MoveTo(RECT const& rc);
	void DoPaint(CDCHandle dc);
private:
	void _Refresh();
	void _ResizeWindow();
	void _RepositionWindow();
	CRect m_inputPos;
	weasel::Context m_ctx;
	weasel::Status m_status;
};
