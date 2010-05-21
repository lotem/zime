#include <windows.h>
#include <algorithm>

#include "rhymepanel.h"

static const WCHAR RHYME_PANEL_CLASS[] = L"RhymePanelClass";

static const int FONT_POINT_SIZE = 16;

static const int BORDER = 3;
static const int X_MARGIN = 18;
static const int Y_MARGIN = 12;
static const int SPACING = 4;

HINSTANCE RhymePanel::hModuleInstance = NULL;

std::map<HWND, RhymePanel*> RhymePanel::panelIndex;

RhymePanel::RhymePanel() : hwnd(0)
{
}

RhymePanel::~RhymePanel()
{
    if (hwnd)
        destroy();
}

void RhymePanel::setPreedit(std::wstring const& preedit)
{
    this->preedit = preedit;
}

std::wstring const& RhymePanel::getPreedit() const
{
    return preedit;
}

void RhymePanel::setAux(std::wstring const& aux)
{
    this->aux = aux;
}

std::wstring const& RhymePanel::getAux() const
{
    return aux;
}

std::vector<std::wstring>& RhymePanel::getCandidates()
{
    return candidates;
}

bool RhymePanel::registerClass(HINSTANCE hInst)
{
    hModuleInstance = hInst;
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_IME;
    wc.lpfnWndProc = (WNDPROC)RhymePanel::wndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = NULL;
    wc.lpszMenuName = (LPTSTR)NULL;
    wc.lpszClassName = RHYME_PANEL_CLASS;
    wc.hbrBackground = NULL;
    wc.hIconSm = NULL;
    if (!RegisterClassEx((LPWNDCLASSEX)&wc))
        return false;
    return true;
}

LRESULT RhymePanel::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        std::map<HWND, RhymePanel*>::iterator i = panelIndex.find(hwnd);
        if (i != panelIndex.end())
        {
            RhymePanel *p = i->second;
            p->onPaint(ps);
        }
        EndPaint(hwnd, &ps);
    }
    break;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

void RhymePanel::onPaint(PAINTSTRUCT& ps)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    HDC memDC = CreateCompatibleDC(ps.hdc);
    HBITMAP bitmap = CreateCompatibleBitmap(ps.hdc, rc.right, rc.bottom);
    HGDIOBJ oldBitmap = SelectObject(memDC, bitmap);

    COLORREF bgColor = GetSysColor(COLOR_WINDOW);
    COLORREF fgColor = GetSysColor(COLOR_WINDOWTEXT);

    // white background
    HBRUSH brush = CreateSolidBrush(bgColor);
    HRGN rgn = CreateRectRgnIndirect(&rc);
    FillRgn(memDC, rgn, brush);
    DeleteObject(brush);
    DeleteObject(rgn);

    // black border
    HPEN pen = CreatePen(PS_SOLID | PS_INSIDEFRAME, BORDER, fgColor);
    HPEN oldPen = (HPEN)SelectObject(memDC, pen);
    Rectangle(memDC, rc.left, rc.top, rc.right, rc.bottom);
    SelectObject(memDC, oldPen);
    DeleteObject(pen);

    long height = -MulDiv(FONT_POINT_SIZE, GetDeviceCaps(memDC, LOGPIXELSY), 72);
    HFONT font = CreateFont(height, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
    HFONT oldFont = (HFONT)SelectObject(memDC, font);

    SetTextColor(memDC, fgColor);
    SetBkColor(memDC, bgColor);

    SIZE sz;
    RECT rout;
    int xLeft = rc.left + X_MARGIN;
    int xRight = rc.right - X_MARGIN;
    int y = rc.top + Y_MARGIN;
    int yBottom = rc.bottom - Y_MARGIN;

    // TODO: draw preedit string
    ZeroMemory(&sz, sizeof(sz));
    GetTextExtentPoint32(memDC, preedit.c_str(), preedit.length(), &sz);
    SetRect(&rout, xLeft, y, xRight, y + sz.cy);
    ExtTextOut(memDC, xLeft, y, ETO_CLIPPED | ETO_OPAQUE, &rout, preedit.c_str(), preedit.length(), NULL);
    y += sz.cy + SPACING;

    // TODO: draw aux string
    ZeroMemory(&sz, sizeof(sz));
    GetTextExtentPoint32(memDC, aux.c_str(), aux.length(), &sz);
    SetRect(&rout, xLeft, y, xRight, y + sz.cy);
    ExtTextOut(memDC, xLeft, y, ETO_CLIPPED | ETO_OPAQUE, &rout, aux.c_str(), aux.length(), NULL);
    y += sz.cy + SPACING;

    // TODO: draw candidates
    WCHAR cand[100];
    for (size_t i = 0; i < candidates.size(); ++i)
    {
        snwprintf(cand, sizeof(cand), L"%d.  %s", (i + 1), candidates[i].c_str());
        ZeroMemory(&sz, sizeof(sz));
        GetTextExtentPoint32(memDC, cand, wcslen(cand), &sz);
        SetRect(&rout, xLeft, y, xRight, std::min(int(y + sz.cy), yBottom));
        ExtTextOut(memDC, xLeft, y, ETO_CLIPPED | ETO_OPAQUE, &rout, cand, wcslen(cand), NULL);
        y += sz.cy + SPACING;
        if (y >= yBottom)
            break;
    }

    SelectObject(memDC, oldFont);
    DeleteObject(font);

    // TODO: draw ZIME icon at the bottom right corner
    HICON icon = LoadIcon(NULL, IDI_INFORMATION);
    DrawIcon(memDC, xRight - 32, yBottom - 32, icon);

    BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top,
        (ps.rcPaint.right - ps.rcPaint.left), (ps.rcPaint.bottom - ps.rcPaint.top),
        memDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memDC);
}

bool RhymePanel::create(HWND hParent)
{
    if (hwnd)
        destroy();
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);

    hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE, RHYME_PANEL_CLASS, L"Rhyme Panel",
                          WS_POPUP | WS_CLIPCHILDREN | WS_DISABLED,
                          screenX / 4, screenY / 4, screenX / 2, screenY / 2, hParent, NULL, hModuleInstance, NULL);
    if (hwnd)
    {
        SetLayeredWindowAttributes(hwnd, 0, (255 * 90) / 100, LWA_ALPHA);

        panelIndex[hwnd] = this;
        return true;
    }
    return false;
}

void RhymePanel::destroy()
{
    if (hwnd)
    {
        DestroyWindow(hwnd);
        panelIndex.erase(hwnd);
    }
    hwnd = 0;
}

void RhymePanel::show()
{
    if (hwnd)
    {
        ShowWindow(hwnd, SW_SHOWNA);
        UpdateWindow(hwnd);
    }
}

void RhymePanel::hide()
{
    if (hwnd)
        ShowWindow(hwnd, SW_HIDE);
}
