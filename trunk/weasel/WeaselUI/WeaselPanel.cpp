#include "stdafx.h"
#include "WeaselPanel.h"
#include <string>
#include <vector>
#include <WeaselCommon.h>

using namespace weasel;
using namespace std;

WeaselPanel::WeaselPanel()
{
	//test info
	Text aux(L"zhong");
	Text preedit(L"中");
	CandidateInfo cinfo;
	std::vector<Text> candies;

	Text t1(L"中");
	Text t2(L"总");
	Text t3(L"种");
	candies.push_back(t1);
	candies.push_back(t2);
	candies.push_back(t3);

	cinfo.totalPages = 10;
	cinfo.currentPage = 1;
	cinfo.highlighted = 1;
	cinfo.candies = candies;

	m_Info.aux = aux;
	m_Info.preedit = preedit;
	m_Info.cinfo = cinfo;
}

void WeaselPanel::SetContent(const weasel::ZIMEInfo &info)
{
	m_Info = info;
}

void WeaselPanel::SetStatus(const weasel::ZIMEStatus &status)
{
	m_Status = status;
}

//更新界面
void WeaselPanel::_UpdateUI()
{

}

LRESULT WeaselPanel::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	WeaselPanel* pT = static_cast<WeaselPanel*>(this);
	ATLASSERT(::IsWindow(pT->m_hWnd));

	if(wParam != NULL)
	{
		RECT rect = { 0 };
		pT->GetClientRect(&rect);
		CMemoryDC dcMem((HDC)wParam, rect);
		pT->DoPaint(dcMem.m_hDC);
	}
	else
	{
		CPaintDC dc(pT->m_hWnd);
		CMemoryDC dcMem(dc.m_hDC, dc.m_ps.rcPaint);
		pT->DoPaint(dcMem.m_hDC);
	}

	return 0;
}

//draw client area
void WeaselPanel::DoPaint(CDCHandle dc)
{
	// must be implemented in a derived class
	RECT rc;
	GetClientRect(&rc);

	COLORREF bgColor = GetSysColor(COLOR_WINDOW);
	COLORREF fgColor = GetSysColor(COLOR_WINDOWTEXT);

	// white background
	HBRUSH brush = CreateSolidBrush(bgColor);
	HRGN rgn = CreateRectRgnIndirect(&rc);
	dc.FillRgn(rgn, brush);
	DeleteObject(brush);
	DeleteObject(rgn);

	// black border
	HPEN pen = CreatePen(PS_SOLID | PS_INSIDEFRAME, BORDER, fgColor);
	HPEN oldPen = (HPEN)SelectObject(dc.m_hDC, pen);
	Rectangle(dc.m_hDC, rc.left, rc.top, rc.right, rc.bottom);
	SelectObject(dc.m_hDC, oldPen);
	DeleteObject(pen);

	long height = -MulDiv(FONT_POINT_SIZE, GetDeviceCaps(dc.m_hDC, LOGPIXELSY), 72);
	HFONT font = CreateFont(height, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
	HFONT oldFont = (HFONT)SelectObject(dc.m_hDC, font);

	SetTextColor(dc.m_hDC, fgColor);
	SetBkColor(dc.m_hDC, bgColor);

	SIZE sz;
	RECT rout;
	int xLeft = rc.left + X_MARGIN;
	int xRight = rc.right - X_MARGIN;
	int y = rc.top + Y_MARGIN;
	int yBottom = rc.bottom - Y_MARGIN;

	// draw preedit string
	ZeroMemory(&sz, sizeof(sz));
	std::wstring preedit = m_Info.preedit.str;
	GetTextExtentPoint32(dc.m_hDC, preedit.c_str(), preedit.length(), &sz);	
	SetRect(&rout, xLeft, y, xRight, y + sz.cy);
	ExtTextOut(dc.m_hDC, xLeft, y, ETO_CLIPPED | ETO_OPAQUE, &rout, preedit.c_str(), preedit.length(), NULL);
	y += sz.cy + SPACING;

	// draw aux string
	ZeroMemory(&sz, sizeof(sz));
	std:wstring aux = m_Info.aux.str;
	GetTextExtentPoint32(dc.m_hDC, aux.c_str(), aux.length(), &sz);
	SetRect(&rout, xLeft, y, xRight, y + sz.cy);
	ExtTextOut(dc.m_hDC, xLeft, y, ETO_CLIPPED | ETO_OPAQUE, &rout, aux.c_str(), aux.length(), NULL);
	y += sz.cy + SPACING;

	// draw candidates
	WCHAR cand[100];
	vector<Text> candidates = m_Info.cinfo.candies;
	for (size_t i = 0; i < candidates.size(); ++i)
	{
		_snwprintf(cand, sizeof(cand), L"%d.  %s", (i + 1), candidates[i].str.c_str());
		ZeroMemory(&sz, sizeof(sz));
		GetTextExtentPoint32(dc.m_hDC, cand, wcslen(cand), &sz);
		SetRect(&rout, xLeft, y, xRight, min(int(y + sz.cy), yBottom));
		ExtTextOut(dc.m_hDC, xLeft, y, ETO_CLIPPED | ETO_OPAQUE, &rout, cand, wcslen(cand), NULL);
		y += sz.cy + SPACING;
		if (y >= yBottom)
			break;
	}

	SelectObject(dc.m_hDC, oldFont);
	DeleteObject(font);
	
	//draw others
	//

}

LRESULT WeaselPanel::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	MoveWindow(0, 0, screenX / 4, screenY / 4);
	CenterWindow();
	SendMessage(WM_PAINT);
	return TRUE;
}

LRESULT WeaselPanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

void WeaselPanel::CloseDialog(int nVal)
{
	
}

