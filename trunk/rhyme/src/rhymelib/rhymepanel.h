#ifndef RHYMEPANEL_H
#define RHYMEPANEL_H

#include <map>

class RhymePanel
{
    public:
        RhymePanel();
        virtual ~RhymePanel();
        bool create(HWND hParent);
        void destroy();
        void show();
        void hide();
        //update(...);
        void onPaint(PAINTSTRUCT& ps);
        static bool registerClass(HINSTANCE hInst);
    protected:
    private:
        static LRESULT wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
        static HINSTANCE hModuleInstance;
        static std::map<HWND, RhymePanel*> panelIndex;
        HWND hwnd;
};

#endif // RHYMEPANEL_H
