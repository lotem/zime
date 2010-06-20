#pragma once

#include <WeaselCommon.h>

//
// 输入法界面接口类
//
class WeaselUI
{
public:
	WeaselUI() : pimpl_(0)
	{
	}

	virtual ~WeaselUI()
	{
		if (pimpl_)
			Destroy();
	}

	// 创建输入法界面
	bool Create(HWND parent);

	// 销毁界面
	void Destroy();
	
	// 界面显隐
	void Show();
	void Hide();
	
	// 置输入焦点位置（光标跟随时移动候选窗）
	void UpdateInputPosition(int x, int y);

	// 更新界面显示内容
	void UpdateContent(weasel::ZIMEInfo const& info);

	// 更新输入法状态
	void UpdateStatus(weasel::ZIMEStatus const& status);

private:
	class Impl;
	Impl* pimpl_;
};
