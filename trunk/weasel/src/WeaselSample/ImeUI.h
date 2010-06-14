// MainDlg.h : interface of the CImeUI class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "resource.h "
#include <vector>
#include <string>

using namespace std;

	typedef struct _Candidate
	{
		string pinyin;
		string candList[6];
		int candSub;//0-5
		int totalPages;
		int currentPage;
	}Candidate, *pCandidate;

typedef CWinTraits<WS_BORDER|WS_POPUP|WS_CLIPSIBLINGS|WS_DISABLED, WS_EX_TOOLWINDOW|WS_EX_TOPMOST> CImeUITraits;

class CImeUI : public CWindowImpl<CImeUI, CWindow, CImeUITraits>
{
public:

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
	Candidate mCand;
	/*
	Methods
	*/
public:
	static CImeUI* getInstance();
	void deleteInstance();
	void Show();
	void Hide();
	void Update(int posx, int posy);
	void Move(int posx, int posy);
	void getWindowSize(int &width, int &height);

	~CImeUI(){};
private:
	CImeUI();
	void getFontSize(int &fWidth, int &fHeight);
public:
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
