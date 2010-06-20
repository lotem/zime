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
	void UpdateInputPosition(int x, int y);

	// ���½�����ʾ����
	void UpdateContent(weasel::ZIMEInfo const& info);

	// �������뷨״̬
	void UpdateStatus(weasel::ZIMEStatus const& status);

private:
	class Impl;
	Impl* pimpl_;
};
