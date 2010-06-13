// MainDlg.cpp : implementation of the CImeUI class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "ImeUI.h"
#include <vector>
#include <string>

using namespace std;


CImeUI* CImeUI::mInstance = NULL;

//not using messageloop

LRESULT CImeUI::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	//for(int i = 0; i < 5; i++)
	//{
	//	ctrls[i].Create(m_hWnd);
	//}
	return TRUE;
}

LRESULT CImeUI::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

void CImeUI::CloseDialog(int nVal)
{
	for(int i = 0; i < 5; i++)
	{
		ctrls[i].DestroyWindow();
	}
	//DestroyWindow();
	//do not destroy here
}

LRESULT CImeUI::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    HDC hdc;
    PAINTSTRUCT ps;
	hdc = BeginPaint(&ps);

	CRect rcWnd;
	GetWindowRect(&rcWnd);

    COLORREF m_clrBackColor=RGB(255,255,255);
	
	CClientDC cdc(m_hWnd);
    cdc.FillSolidRect(rcWnd, m_clrBackColor );

    SetBkMode(hdc, TRANSPARENT);

	//HFONT font = GetFont();
	//CLogFont f = font;
	int margin = 2;
	int interval = 10;//f.GetHeight();
	int x = margin;
	for(int i = 0; i < 6; i++)
	{
		int len =  mCand.candList[i].length();
		TextOut(hdc,x, interval, mCand.candList[i].c_str(), len);
		x += interval*(len + 1);
	}

    EndPaint(&ps);
    return 0;
}

CImeUI::CImeUI()
{
	mCand.pinyin = "fff";
	//string list[6] = {"fdfjdk", "ddfdfd", "gdffd", "dfdfdfd", "fd", "ss"};
	//mCand.candList[0] = "ffff";
	//mCand.candList[1] = "fff";
	//mCand.candList[2] = "fff";
	//mCand.candList[3] = "ff";
	//mCand.candList[4] = "fff";
	//mCand.candList[5] = "ff";
	mCand.candList[0] = "中国改革";
	mCand.candList[1] = "政治改革";
	mCand.candList[2] = "置之高阁";
	mCand.candList[3] = "站长广告";
	mCand.candList[4] = "在中国";
	mCand.candList[5] = "珍珠港";
}

//Singleton
CImeUI* CImeUI::getInstance()
{
	if(!mInstance)
	{
		mInstance = new CImeUI();
	}
	if(!mInstance->IsWindow())
	{
		if(	mInstance->Create(NULL) == NULL)
		{
			ATLTRACE(_T("The CImeUI creation failed!\n"));
		}
	}

	return mInstance;
};

//Delete
void CImeUI::deleteInstance()
{
	if(mInstance)
	{
		if(mInstance->IsWindow())
		{
			mInstance->DestroyWindow();
		}

		delete mInstance;
		mInstance = NULL;
	}
};

void CImeUI::Show()
{
	ShowWindow(SW_SHOWNOACTIVATE);
}

void CImeUI::Hide()
{
	ShowWindow(SW_HIDE);
}

/**

*/
void CImeUI::getWindowSize(int &width, int &height)
{
	//HFONT font = GetFont();
	//CLogFont f = font;
	int margin = 2;
	int interval = 10;//f.GetHeight();

	//getmax
	string str = mCand.pinyin;
	int len = 0;// = str.length();
	for(int i = 0; i < 5; i++)
	{
		str = mCand.candList[i];
		len += (str.length() + 1);
	}
	width = len * interval;
	height = 2 * 20;
}

void CImeUI::Move(int posx, int posy)
{
	//ShowWindow(SW_HIDE);
	int width, height;
	getWindowSize(width, height);
	MoveWindow(posx, posy, width, height);
}

void CImeUI::Update(int posx, int posy)
{
	Move(posx, posy);

	Show();
}
