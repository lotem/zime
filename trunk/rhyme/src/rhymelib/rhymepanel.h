#ifndef RHYMEPANEL_H
#define RHYMEPANEL_H

#include <map>
#include <string>
#include <vector>

class RhymePanel
{
    public:
        RhymePanel();
        virtual ~RhymePanel();
        bool create(HWND hParent);
        void destroy();
        void show();
        void hide();
        void setPreedit(std::wstring const& preedit);
        std::wstring const& getPreedit() const;
        void setAux(std::wstring const& aux);
        std::wstring const& getAux() const;
        std::vector<std::wstring>& getCandidates();
        static bool registerClass(HINSTANCE hInst);
    protected:
    private:
        void onPaint(PAINTSTRUCT& ps);
        static LRESULT wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
        static HINSTANCE hModuleInstance;
        static std::map<HWND, RhymePanel*> panelIndex;
        HWND hwnd;
        std::wstring preedit;
        std::wstring aux;
        std::vector<std::wstring> candidates;
};

#endif // RHYMEPANEL_H
