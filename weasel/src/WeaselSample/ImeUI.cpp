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

    TextOut(hdc, 0, 0, TEXT("中国"), lstrlen( TEXT("中国")));

    EndPaint(&ps);
    return 0;
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
	//getmax
	string str = mCand.pinyin;
	int len;// = str.length();
	for(int i = 0; i < 5; i++)
	{
		str = mCand.candList[i];
		len += (str.length() + 1);
	}
	width = len * 20;
	height = 2 * 20;
}

void CImeUI::Move(CRect rc)
{
	//ShowWindow(SW_HIDE);
}

void CImeUI::Update()
{
	for(int i = 0; i < 5; i++)
	{
		//str = mCand.candList[i];
	}
	//ShowWindow(SW_HIDE);
}
