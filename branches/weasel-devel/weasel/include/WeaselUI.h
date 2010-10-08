#pragma once

#include <WeaselCommon.h>

namespace weasel
{

	struct UIStyle
	{
		std::wstring fontFace;
		int fontPoint;
		UIStyle() : fontFace(), fontPoint(0) {}
	};

	class UIImpl;

	//
	// ���뷨����ӿ���
	//
	class UI
	{
	public:
		UI() : pimpl_(0)
		{
		}

		virtual ~UI()
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
		
		// ���ý�����ʽ
		void SetStyle(UIStyle const& style);

		// �����뽹��λ�ã�������ʱ�ƶ���ѡ����
		void UpdateInputPosition(RECT const& rc);

		// ���½�����ʾ����
		void UpdateContext(weasel::Context const& ctx);

		// �������뷨״̬
		void UpdateStatus(weasel::Status const& status);

	private:
		UIImpl* pimpl_;
	};

}