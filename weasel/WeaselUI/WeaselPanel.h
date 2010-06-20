#pragma once

#include <WeaselCommon.h>

typedef CWinTraits<WS_BORDER|WS_POPUP|WS_CLIPSIBLINGS|WS_DISABLED, WS_EX_TOOLWINDOW|WS_EX_TOPMOST> CWeaselPanelTraits;

class WeaselPanel : public CWindowImpl<WeaselPanel, CWindow, CWeaselPanelTraits>
{
public:
	DECLARE_WND_CLASS_EX(L"WeaselPanel", CS_HREDRAW|CS_VREDRAW|CS_DROPSHADOW, COLOR_WINDOW);

	WeaselPanel();
	void SetContent(const weasel::ZIMEInfo& info);
	void SetStatus(const weasel::ZIMEStatus& status);

private:
	BEGIN_MSG_MAP(WeaselPanel)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_PAINT(OnPaint)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT cs);
	void OnDestroy();
	void OnPaint(HDC hdc);
};
