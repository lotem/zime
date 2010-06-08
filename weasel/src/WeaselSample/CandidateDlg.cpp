// MainDlg.cpp : implementation of the CCandidateDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "CandidateDlg.h"

CCandidateDlg* CCandidateDlg::mInstance = NULL;

BOOL CCandidateDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CCandidateDlg::OnIdle()
{
	return FALSE;
}

//不使用全局messageloop

LRESULT CCandidateDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	//// set icons
	//HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
	//	IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	//SetIcon(hIcon, TRUE);
	//HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
	//	IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	//SetIcon(hIconSmall, FALSE);

	//// register object for message filtering and idle updates
	//CMessageLoop* pLoop = _Module.GetMessageLoop();
	//ATLASSERT(pLoop != NULL);
	//pLoop->AddMessageFilter(this);
	//pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	return TRUE;
}

LRESULT CCandidateDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	//CMessageLoop* pLoop = _Module.GetMessageLoop();
	//ATLASSERT(pLoop != NULL);
	//pLoop->RemoveMessageFilter(this);
	//pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CCandidateDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CCandidateDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}

LRESULT CCandidateDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CCandidateDlg::CloseDialog(int nVal)
{
	//DestroyWindow();
	//在退出时销毁窗口
}

//单例
CCandidateDlg* CCandidateDlg::getInstance()
{
	if(!mInstance)
	{
		mInstance = new CCandidateDlg();
	}

	if(	mInstance->Create(NULL) == NULL)
	{
		ATLTRACE(_T("The CCandidateDlg creation failed!\n"));
	}

	return mInstance;
};

//销毁实例
void CCandidateDlg::deleteInstance()
{
	if(mInstance)
	{
		mInstance->DestroyWindow();

		delete mInstance;
		mInstance = NULL;
	}
};

