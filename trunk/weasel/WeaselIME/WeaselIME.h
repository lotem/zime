// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WEASELIME_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WEASELIME_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WEASELIME_EXPORTS
#define WEASELIME_API __declspec(dllexport)
#else
#define WEASELIME_API __declspec(dllimport)
#endif

// This class is exported from the WeaselIME.dll
class WEASELIME_API CWeaselIME {
public:
	CWeaselIME(void);
	// TODO: add your methods here.
};

extern WEASELIME_API int nWeaselIME;

WEASELIME_API int fnWeaselIME(void);
