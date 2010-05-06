#ifndef RHYMECLIENT_H
#define RHYMECLIENT_H

#include <string>

struct RhymeMessage
{
    bool result;
};

class RhymeClient
{
    public:
        RhymeClient();
        virtual ~RhymeClient();
        bool checkService();
        void startSession(std::wstring const& schema);
        void endSession();
        RhymeMessage processKeyEvent(long keyCode, long mask);
    protected:
    private:
};

#endif // RHYMECLIENT_H
