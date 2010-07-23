#pragma once

#include <WeaselCommon.h>

//
// ���뷨����ӿ���
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

	// �������뷨����
	bool Create(HWND parent);

	// ���ٽ���
	void Destroy();
	
	// ��������
	void Show();
	void Hide();
	
	// �����뽹��λ�ã�������ʱ�ƶ���ѡ����
	void UpdateInputPosition(RECT const& rc);

	// ���½�����ʾ����
	void UpdateContext(weasel::Context const& ctx);

	// �������뷨״̬
	void UpdateStatus(weasel::Status const& status);

private:
	class Impl;
	Impl* pimpl_;
};
