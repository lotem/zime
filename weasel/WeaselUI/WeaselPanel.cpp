#include "stdafx.h"
#include "WeaselPanel.h"
#include <WeaselCommon.h>

using namespace weasel;

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

void WeaselPanel::_ResizeWindow()
{
	CDC dc = GetDC();
	long fontHeight = -MulDiv(GetFontPoint(), dc.GetDeviceCaps(LOGPIXELSY), 72);

	CFont font;
	font.CreateFontW(fontHeight, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, GetFontFace());
	CFontHandle oldFont = dc.SelectFont(font);

	long width = 0;
	long height = 0;
	CSize sz;

	// draw preedit string
	wstring const& preedit = m_ctx.preedit.str;
	if (!preedit.empty())
	{
		dc.GetTextExtent(preedit.c_str(), preedit.length(), &sz);
		width = max(width, sz.cx);
		height += sz.cy + SPACING;
	}

	// draw aux string
	wstring const& aux = m_ctx.aux.str;
	if (!aux.empty())
	{
		dc.GetTextExtent(aux.c_str(), aux.length(), &sz);
		width = max(width, sz.cx);
		height += sz.cy + SPACING;
	}

	// draw candidates
	vector<Text> const& candidates = m_ctx.cinfo.candies;
	for (size_t i = 0; i < candidates.size(); ++i, height += CAND_SPACING)
	{
		wstring cand = (boost::wformat(L"%1%. %2%") % (i + 1) % candidates[i].str).str();
		dc.GetTextExtent(cand.c_str(), cand.length(), &sz);
		width = max(width, sz.cx);
		height += sz.cy;
	}
	if (!candidates.empty())
		height += SPACING;

	//trim the last spacing
	if (height > 0)
		height -= SPACING;

	width += 2 * MARGIN_X;
	height += 2 * MARGIN_Y;

	width = max(width, MIN_WIDTH);
	height = max(height, MIN_HEIGHT);
	
	SetWindowPos( NULL, 0, 0, width, height, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
}

//更新界面
void WeaselPanel::_Refresh()
{
	_ResizeWindow();
	_RepositionWindow();
	RedrawWindow();
}

bool WeaselPanel::_DrawText(Text const& text, CDCHandle dc, CRect const& rc, int& y)
{
	bool drawn = false;
	wstring const& t = text.str;
	if (!t.empty())
	{
		vector<weasel::TextAttribute> const& attrs = text.attributes;
		CSize sz;
		dc.GetTextExtent(t.c_str(), t.length(), &sz);
		CRect rout(rc.left, y, rc.right, y + sz.cy);
		dc.ExtTextOutW(rc.left, y, ETO_CLIPPED | ETO_OPAQUE, &rout, t.c_str(), t.length(), 0);
		for (size_t j = 0; j < attrs.size(); ++j)
		{
			if (attrs[j].type == weasel::HIGHLIGHTED)
			{
				weasel::TextRange const& range = attrs[j].range;
				CSize szout;
				dc.GetTextExtent(t.c_str(), range.start, &szout);
				CPoint p(rc.left + szout.cx, y);
				dc.GetTextExtent(t.c_str() + range.start, range.end - range.start, &szout);
				p.y -= HIGHLIGHT_PADDING_TOP;
				szout.cy += HIGHLIGHT_PADDING_TOP;
				szout.cy += HIGHLIGHT_PADDING_BOTTOM;
				//if (range.end == t.length())
				//	szout.cx = xRight - p.x;
				dc.BitBlt(p.x, p.y, szout.cx, szout.cy, dc.m_hDC, p.x, p.y, DSTINVERT);
			}
		}
		y += sz.cy;
		drawn = true;
	}
	return drawn;
}

bool WeaselPanel::_DrawCandidates(CandidateInfo const& cinfo, CDCHandle dc, CRect const& rc, int& y)
{
	bool drawn = false;
	vector<Text> const& candies = cinfo.candies;
	for (size_t i = 0; i < candies.size(); ++i, y += CAND_SPACING)
	{
		if (y >= rc.bottom)
			break;
		wstring t = (boost::wformat(L"%1%. %2%") % (i + 1) % candies[i].str).str();
		CSize sz;
		dc.GetTextExtent(t.c_str(), t.length(), &sz);
		CRect rout(rc.left, y, rc.right, y + sz.cy);
		dc.ExtTextOutW(rc.left, y, ETO_CLIPPED | ETO_OPAQUE, &rout, t.c_str(), t.length(), 0);
		if (i == cinfo.highlighted)
		{
			rout.InflateRect(0, HIGHLIGHT_PADDING_TOP, 
				             0, HIGHLIGHT_PADDING_BOTTOM);
			CSize szout = rout.Size();
			dc.BitBlt(rout.left, rout.top, szout.cx, szout.cy, dc, rout.left, rout.top, DSTINVERT);
		}
		y += sz.cy;
		drawn = true;
	}
	return drawn;
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

	long height = -MulDiv(GetFontPoint(), dc.GetDeviceCaps(LOGPIXELSY), 72);

	CFont font;
	font.CreateFontW(height, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, GetFontFace());
	CFontHandle oldFont = dc.SelectFont(font);

	dc.SetTextColor(fgColor);
	dc.SetBkColor(bgColor);

	rc.DeflateRect(MARGIN_X, MARGIN_Y);

	int y = rc.top;

	// draw preedit string
	if (_DrawText(m_ctx.preedit, dc, rc, y))
		y += SPACING;

	// draw aux string
	if (_DrawText(m_ctx.aux, dc, rc, y))
		y += SPACING;

	// draw candidates
	if (_DrawCandidates(m_ctx.cinfo, dc, rc, y))
		y += SPACING;

	// TODO: draw other parts

	if (y > rc.top)
		y -= SPACING;

	dc.SelectFont(oldFont);	
}

LRESULT WeaselPanel::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_Refresh();
	//CenterWindow();
	GetWindowRect(&m_inputPos);
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
	m_inputPos = rc;
	m_inputPos.InflateRect(0, offset);
	_RepositionWindow();
}

void WeaselPanel::_RepositionWindow()
{
	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	RECT rcWindow;
	GetWindowRect(&rcWindow);
	int width = (rcWindow.right - rcWindow.left);
	int height = (rcWindow.bottom - rcWindow.top);
	// keep panel visible
	rcWorkArea.right -= width;
	rcWorkArea.bottom -= height;
	int x = m_inputPos.left;
	int y = m_inputPos.bottom;
	if (x > rcWorkArea.right)
		x = rcWorkArea.right;
	if (x < rcWorkArea.left)
		x = rcWorkArea.left;
	// show panel above the input focus if we're around the bottom
	if (y > rcWorkArea.bottom)
		y = m_inputPos.top - height;
	if (y > rcWorkArea.bottom)
		y = rcWorkArea.bottom;
	if (y < rcWorkArea.top)
		y = rcWorkArea.top;
	// memorize adjusted position (to avoid window bouncing on height change)
	m_inputPos.bottom = y;
	SetWindowPos(HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE|SWP_NOACTIVATE);
}