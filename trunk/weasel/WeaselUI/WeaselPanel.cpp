#include "stdafx.h"
#include "WeaselPanel.h"
#include <string>
#include <vector>
#include <WeaselCommon.h>

using namespace weasel;
using namespace std;

WeaselPanel::WeaselPanel()
{
}

void WeaselPanel::SetContext(const weasel::Context &ctx)
{
	m_ctx = ctx;
	_Refresh();
}

void WeaselPanel::SetStatus(const weasel::Status &status)
{
	m_status = status;
	_Refresh();
}

CSize WeaselPanel::_GetWindowSize()
{
	wstring const& preedit = m_ctx.preedit.str;
	wstring const& aux = m_ctx.aux.str;
	vector<Text> const& candidates = m_ctx.cinfo.candies;
	CDC dc = GetDC();
	long fontHeight = -MulDiv(FONT_POINT_SIZE, dc.GetDeviceCaps(LOGPIXELSY), 72);

	CFont font;
	font.CreateFontW(fontHeight, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, FONT_FACE);
	CFontHandle oldFont = dc.SelectFont(font);

	long width = 0;
	long height = 0;
	CSize sz;

	// draw preedit string
	{
		wstring const& preedit = m_ctx.preedit.str;
		dc.GetTextExtent(preedit.c_str(), preedit.length(), &sz);
		width = max(width, sz.cx);
		height += sz.cy + SPACING;
	}

	// draw aux string
	{
		wstring const& aux = m_ctx.aux.str;
		dc.GetTextExtent(aux.c_str(), aux.length(), &sz);
		width = max(width, sz.cx);
		height += sz.cy + SPACING;
	}

	// draw candidates
	{
		vector<Text> const& candidates = m_ctx.cinfo.candies;
		for (size_t i = 0; i < candidates.size(); ++i)
		{
			wstring cand = (boost::wformat(L"%1%. %2%") % (i + 1) % candidates[i].str).str();
			dc.GetTextExtent(cand.c_str(), cand.length(), &sz);
			width = max(width, sz.cx);
			height += sz.cy + SPACING;
		}
	}

	//trim the last spacing
	height -= SPACING;

	width += 2 * X_MARGIN;
	height += 2 * Y_MARGIN;
	
	return CSize(width, height);
}

//更新界面
void WeaselPanel::_Refresh()
{
	CSize sz = _GetWindowSize();
	SetWindowPos( NULL, 0, 0, sz.cx, sz.cy, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	RedrawWindow();
}

LRESULT WeaselPanel::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& Handled)
{
	ATLASSERT(::IsWindow(m_hWnd));

	if(wParam != NULL)
	{
		CRect rect;
		GetClientRect(&rect);
		CMemoryDC dcMem((HDC)wParam, rect);
		DoPaint(dcMem.m_hDC);
	}
	else
	{
		CPaintDC dc(m_hWnd);
		CMemoryDC dcMem(dc.m_hDC, dc.m_ps.rcPaint);
		DoPaint(dcMem.m_hDC);
	}
	return 0;
}

//draw client area
void WeaselPanel::DoPaint(CDCHandle dc)
{
	CRect rc;
	GetClientRect(&rc);

	COLORREF bgColor = GetSysColor(COLOR_WINDOW);
	COLORREF fgColor = GetSysColor(COLOR_WINDOWTEXT);

	// background
	{
		CBrush brush;
		CRgn rgn;
		brush.CreateSolidBrush(bgColor);
		rgn.CreateRectRgnIndirect(&rc);
		dc.FillRgn(rgn, brush);
	}

	// black border
	{
		CPen pen;
		pen.CreatePen(PS_SOLID | PS_INSIDEFRAME, BORDER, fgColor);
		CPenHandle oldPen = dc.SelectPen(pen);
		dc.Rectangle(&rc);
		dc.SelectPen(oldPen);
	}

	long height = -MulDiv(FONT_POINT_SIZE, dc.GetDeviceCaps(LOGPIXELSY), 72);

	CFont font;
	font.CreateFontW(height, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, FONT_FACE);
	CFontHandle oldFont = dc.SelectFont(font);

	dc.SetTextColor(fgColor);
	dc.SetBkColor(bgColor);

	int xLeft = rc.left + X_MARGIN;
	int xRight = rc.right - X_MARGIN;
	int y = rc.top + Y_MARGIN;
	int yBottom = rc.bottom - Y_MARGIN;

	// draw preedit string
	{
		wstring const& preedit = m_ctx.preedit.str;
		vector<weasel::TextAttribute> const& attrs = m_ctx.preedit.attributes;
		CSize sz;
		dc.GetTextExtent(preedit.c_str(), preedit.length(), &sz);
		CRect rout(xLeft, y, xRight, y + sz.cy);
		dc.ExtTextOutW(xLeft, y, ETO_CLIPPED | ETO_OPAQUE, &rout, preedit.c_str(), preedit.length(), 0);
		for (size_t j = 0; j < attrs.size(); ++j)
		{
			if (attrs[j].type == weasel::HIGHLIGHTED)
			{
				weasel::TextRange const& range = attrs[j].range;
				CSize sz;
				dc.GetTextExtent(preedit.c_str(), range.start, &sz);
				int xx = xLeft + sz.cx;
				int yy = y - HIGHLIGHT_PADDING_TOP;
				dc.GetTextExtent(preedit.c_str() + range.start, range.end - range.start, &sz);
				sz.cy += HIGHLIGHT_PADDING_TOP + HIGHLIGHT_PADDING_BOTTOM;
				dc.BitBlt(xx, yy, sz.cx, sz.cy, dc.m_hDC, xx, yy, DSTINVERT);
			}
		}
		y += sz.cy;
	}
	y += SPACING;

	// draw aux string
	{
		wstring const& aux = m_ctx.aux.str;
		CSize sz;
		dc.GetTextExtent(aux.c_str(), aux.length(), &sz);
		CRect rout(xLeft, y, xRight, y + sz.cy);
		dc.ExtTextOutW(xLeft, y, ETO_CLIPPED | ETO_OPAQUE, &rout, aux.c_str(), aux.length(), 0);
		y += sz.cy;
	}
	y += SPACING;

	// draw candidates
	{
		vector<Text> const& candidates = m_ctx.cinfo.candies;
		for (size_t i = 0; i < candidates.size(); ++i)
		{
			if (y >= yBottom)
				break;
			wstring cand = (boost::wformat(L"%1%. %2%") % (i + 1) % candidates[i].str).str();
			CSize sz;
			dc.GetTextExtent(cand.c_str(), cand.length(), &sz);
			CRect rout(xLeft, y, xRight, y + sz.cy);
			dc.ExtTextOutW(xLeft, y, ETO_CLIPPED | ETO_OPAQUE, &rout, cand.c_str(), cand.length(), 0);
			if (i == m_ctx.cinfo.highlighted)
			{
				rout.top -= HIGHLIGHT_PADDING_TOP;
				rout.bottom += HIGHLIGHT_PADDING_BOTTOM;
				int width = rout.right - rout.left;
				int height = rout.bottom - rout.top;
				dc.BitBlt(rout.left, rout.top, width, height, dc, rout.left, rout.top, DSTINVERT);
			}
			y += sz.cy + SPACING;
		}
	}

	// TODO: draw other parts

	dc.SelectFont(oldFont);	
}

LRESULT WeaselPanel::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_Refresh();
	CenterWindow();
	return TRUE;
}

LRESULT WeaselPanel::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

void WeaselPanel::CloseDialog(int nVal)
{
	
}

void WeaselPanel::MoveTo(RECT const& rc)
{
	const int offset = 6;
	int x = rc.left;
	int y = rc.bottom + offset;
	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	RECT rcWindow;
	GetWindowRect(&rcWindow);
	int width = (rcWindow.right - rcWindow.left);
	int height = (rcWindow.bottom - rcWindow.top);
	// keep panel visible
	rcWorkArea.right -= width;
	rcWorkArea.bottom -= height;
	if (x > rcWorkArea.right)
		x = rcWorkArea.right;
	if (x < rcWorkArea.left)
		x = rcWorkArea.left;
	if (y > rcWorkArea.bottom)
		y = rc.top - offset - height;
	if (y > rcWorkArea.bottom)
		y = rcWorkArea.bottom;
	if (y < rcWorkArea.top)
		y = rcWorkArea.top;
	SetWindowPos(HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE|SWP_NOACTIVATE);
}
