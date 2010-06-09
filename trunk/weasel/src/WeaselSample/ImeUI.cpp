// MainDlg.cpp : implementation of the CImeUI class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "ImeUI.h"

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
