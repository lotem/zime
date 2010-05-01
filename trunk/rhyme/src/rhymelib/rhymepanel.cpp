#include <windows.h>
#include <string>
#include "rhymepanel.h"

const char RHYME_PANEL_CLASS[] = "RhymePanelClass";

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
    HBITMAP memBitmap = CreateCompatibleBitmap(ps.hdc, rc.right, rc.bottom);
    HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);

    const std::string OUTPUT = "Abracadabra!";
    SetTextColor(memDC, GetSysColor(COLOR_WINDOWTEXT));
    SetBkColor(memDC, GetSysColor(COLOR_WINDOW));
    ExtTextOut(memDC, 0, 0, ETO_OPAQUE, &rc, OUTPUT.c_str(), OUTPUT.length(), NULL);

    BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top,
        (ps.rcPaint.right - ps.rcPaint.left), (ps.rcPaint.bottom - ps.rcPaint.top),
        memDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}

bool RhymePanel::create(HWND hParent)
{
    if (hwnd)
        destroy();
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);

    hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE, RHYME_PANEL_CLASS, "Rhyme Panel",
                          WS_POPUP | WS_CLIPCHILDREN | WS_DISABLED,
                          screenX / 3, screenY / 3, screenX / 3, screenY / 3, hParent, NULL, hModuleInstance, NULL);
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
