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
	return TRUE;
}

LRESULT CImeUI::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

void CImeUI::CloseDialog(int nVal)
{
	
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

	HFONT g_hfFont = (HFONT)GetStockObject(SYSTEM_FONT);
	HFONT hfOld = (HFONT)SelectObject(hdc, g_hfFont);
	
	int dx, dy;
	getFontSize(dx, dy);

	int x = dx;
	int len =  mCand.pinyin.length();

	TextOut(hdc, x, 4, mCand.pinyin.c_str(), len);

	for(int i = 0; i < 6; i++)
	{
		len =  mCand.candList[i].length();
		TextOut(hdc, x, 8 + dy, mCand.candList[i].c_str(), len);
		x += dx*(len + 2);
	}

	SelectObject(hdc, hfOld);

    EndPaint(&ps);
    return 0;
}

CImeUI::CImeUI()
{
	mCand.pinyin = "zzgz";
	mCand.candList[0] = "1.中国改革";
	mCand.candList[1] = "2.政治改革";
	mCand.candList[2] = "3.置之高阁";
	mCand.candList[3] = "4.站长广告";
	mCand.candList[4] = "5.在中国";
	mCand.candList[5] = "6.珍珠港";
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
	int dx, dy;
	getFontSize(dx, dy);

	string str = mCand.pinyin;
	int len = 0;// = str.length();
	for(int i = 0; i < 6; i++)
	{
		str = mCand.candList[i];
		len += (str.length() + 2);
	}
	
	width = len * dx;
	height = 2 * dy + 12;
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

void CImeUI::getFontSize(int &fWidth, int &fHeight)
{
	HFONT g_hfFont = (HFONT)GetStockObject(SYSTEM_FONT);
	LOGFONT lf;
	GetObject(g_hfFont, sizeof(LOGFONT), &lf);
	fWidth = (int)lf.lfWidth;
	fHeight = (int)lf.lfHeight;
}