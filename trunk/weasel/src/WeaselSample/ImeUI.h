// MainDlg.h : interface of the CImeUI class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include   "resource.h "

typedef CWinTraits<WS_BORDER|WS_POPUP|WS_CLIPSIBLINGS, WS_EX_TOOLWINDOW|WS_EX_TOPMOST> CImeUITraits;

class CImeUI : public CWindowImpl<CImeUI, CWindow, CImeUITraits>
{
public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CImeUI)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void CloseDialog(int nVal);

	//Custom
	/*
	Members
	*/
private:
	static CImeUI* mInstance;
	/*
	Methods
	*/
public:
	static CImeUI* getInstance();
	void deleteInstance();

	~CImeUI(){};
private:
	CImeUI(){};
public:
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
