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

//���½���
void WeaselPanel::_Refresh()
{
	_ResizeWindow();
	_RepositionWindow();
	RedrawWindow();
}

void WeaselPanel::_HighlightText(CDCHandle dc, CRect rc, COLORREF color, DWORD dwRop)
{
	COLORREF crMask = 0xFFFFFF & ~color;
	rc.InflateRect(0, HIGHLIGHT_PADDING);
	CMemoryDC memDC(dc, rc);
	CBrush brush;
	brush.CreateSolidBrush(crMask);
	CBrush oldBrush = memDC.SelectBrush(brush);
	CPen pen;
	pen.CreatePen(PS_SOLID, 0, crMask);
	CPen oldPen = memDC.SelectPen(pen);
	CPoint ptRoundCorner(ROUND_CORNER, ROUND_CORNER);
	memDC.RoundRect(rc, ptRoundCorner);
	memDC.SelectBrush(oldBrush);
	memDC.SelectPen(oldPen);
	memDC.BitBlt(
		rc.left, rc.top,
		rc.Width(), rc.Height(), 
		dc,
		rc.left, rc.top, 
		dwRop);
}

bool WeaselPanel::_DrawText(Text const& text, CDCHandle dc, CRect const& rc, int& y)
{
	bool drawn = false;
	wstring const& t = text.str;
	if (!t.empty())
	{
		vector<weasel::TextAttribute> const& attrs = text.attributes;
		CSize szText;
		dc.GetTextExtent(t.c_str(), t.length(), &szText);
		CRect rcText(rc.left, y, rc.right, y + szText.cy);
		dc.ExtTextOutW(rc.left, y, ETO_CLIPPED | ETO_OPAQUE, &rcText, t.c_str(), t.length(), 0);
		for (size_t j = 0; j < attrs.size(); ++j)
		{
			if (attrs[j].type == weasel::HIGHLIGHTED)
			{
				weasel::TextRange const& range = attrs[j].range;
				CSize selStart;
				dc.GetTextExtent(t.c_str(), range.start, &selStart);
				CSize selEnd;
				dc.GetTextExtent(t.c_str(), range.end, &selEnd);
				CRect rcHilited(
					rc.left + selStart.cx, 
					y, 
					rc.left + selEnd.cx, 
					y + szText.cy
				);
				_HighlightText(dc, rcHilited, HIGHLIGHTED_TEXT_COLOR, SRCERASE);
			}
		}
		y += szText.cy;
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
		wstring t = (boost::wformat(L" %1%. %2%") % (i + 1) % candies[i].str).str();
		CSize szText;
		dc.GetTextExtent(t.c_str(), t.length(), &szText);
		CRect rcText(rc.left, y, rc.right, y + szText.cy);
		dc.ExtTextOutW(rc.left, y, ETO_CLIPPED | ETO_OPAQUE, &rcText, t.c_str(), t.length(), 0);
		if (i == cinfo.highlighted)
		{
			_HighlightText(dc, rcText, HIGHLIGHTED_CAND_COLOR, SRCINVERT);
		}
		y += szText.cy;
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