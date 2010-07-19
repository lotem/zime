#include "stdafx.h"
#include <WeaselUI.h>
#include "WeaselPanel.h"

class WeaselUI::Impl : public WeaselPanel
{

};

bool WeaselUI::Create(HWND parent)
{
	if (pimpl_)
		Destroy();

	pimpl_ = new WeaselUI::Impl();
	if (!pimpl_)
	{
		return false;
	}

	pimpl_->Create(parent);
	return true;
}

void WeaselUI::Destroy()
{
	if (pimpl_)
	{
		pimpl_->DestroyWindow();
		delete pimpl_;
		pimpl_ = 0;
	}
}

void WeaselUI::Show()
{
	if (pimpl_)
		pimpl_->ShowWindow(SW_SHOWNA);
}

void WeaselUI::Hide()
{
	if (pimpl_)
		pimpl_->ShowWindow(SW_HIDE);

}

void WeaselUI::UpdateInputPosition(RECT const& rc)
{
	if (pimpl_)
	{
		pimpl_->MoveTo(rc);
	}
}

void WeaselUI::UpdateContent(const weasel::ZIMEInfo &info)
{
	if (pimpl_)
	{
		pimpl_->SetContent(info);		
	}
}

void WeaselUI::UpdateStatus(const weasel::ZIMEStatus &status)
{
	if (pimpl_)
	{
		pimpl_->SetStatus(status);
	}
}
