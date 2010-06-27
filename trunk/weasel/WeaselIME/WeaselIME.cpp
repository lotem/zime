// WeaselIME.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "WeaselIME.h"


BOOL WINAPI ImeInquire(IMEINFO* lpIMEInfo, LPWSTR lpszUIClass, DWORD dwSystemInfoFlags)
{
	if (!lpIMEInfo || !lpszUIClass)
		return FALSE;

	// TODO:
	//dwSystemInfoFlags;

	// TODO:
	wcscpy(lpszUIClass, L"WeaselUIClass");

	lpIMEInfo->dwPrivateDataSize = 0;
	lpIMEInfo->fdwProperty = IME_PROP_UNICODE | IME_PROP_SPECIAL_UI;
	lpIMEInfo->fdwConversionCaps = IME_CMODE_FULLSHAPE | IME_CMODE_NATIVE;
	lpIMEInfo->fdwSentenceCaps = IME_SMODE_NONE;
	lpIMEInfo->fdwUICaps = UI_CAP_2700;
	lpIMEInfo->fdwSCSCaps = 0;
	lpIMEInfo->fdwSelectCaps = SELECT_CAP_CONVERSION;

	return TRUE;
}

BOOL WINAPI ImeConfigure(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData)
{
	//
	return TRUE;
}

DWORD WINAPI  ImeConversionList(HIMC hIMC,LPCTSTR lpSource,LPCANDIDATELIST lpCandList,DWORD dwBufLen,UINT uFlag)
{
	//
	return TRUE;
}
BOOL WINAPI  ImeDestroy(UINT uForce)
{
	//
	return TRUE;
}
LRESULT WINAPI  ImeEscape(HIMC hIMC,UINT uSubFunc,LPVOID lpData)
{
	//
	return TRUE;
}
BOOL WINAPI  ImeProcessKey(HIMC hIMC,UINT vKey,LPARAM lKeyData,CONST LPBYTE lpbKeyState)
{
	BOOL bEaten = FALSE;
	// TODO:
	
	return bEaten;
}
BOOL WINAPI  ImeSelect(HIMC hIMC, BOOL fSelect)
{
	// TODO:

	return TRUE;
}

BOOL WINAPI  ImeSetActiveContext(HIMC hIMC,BOOL fFlag)
{
	if (hIMC)
	{	
		if (fFlag)
		{
			LPINPUTCONTEXT lpIMC=ImmLockIMC(hIMC);
			if(lpIMC)
			{
				if(!(lpIMC->fdwInit & INIT_COMPFORM))
				{
					lpIMC->cfCompForm.dwStyle = CFS_DEFAULT;
					GetCursorPos(&lpIMC->cfCompForm.ptCurrentPos);
					ScreenToClient(lpIMC->hWnd, &lpIMC->cfCompForm.ptCurrentPos);
					lpIMC->fdwInit |= INIT_COMPFORM;
				}
				ImmUnlockIMC(hIMC);
			}
		}

		// TODO:
                
	}

	return TRUE;
}

UINT WINAPI  ImeToAsciiEx (UINT uVKey,UINT uScanCode,CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT fuState,HIMC hIMC)
{
	// TODO:

	return 0;
}

BOOL WINAPI  NotifyIME(HIMC hIMC,DWORD dwAction,DWORD dwIndex,DWORD dwValue)
{
	//
	return TRUE;
}
BOOL WINAPI  ImeRegisterWord(LPCTSTR lpRead, DWORD dw, LPCTSTR lpStr)
{
	//
	return FALSE;
}

BOOL WINAPI  ImeUnregisterWord(LPCTSTR lpRead, DWORD dw, LPCTSTR lpStr)
{
	//
	return FALSE;
}

UINT WINAPI  ImeGetRegisterWordStyle(UINT nItem, LPSTYLEBUF lp)
{
	//
	return 0;
}

UINT WINAPI  ImeEnumRegisterWord(REGISTERWORDENUMPROC lpfn, LPCTSTR lpRead, DWORD dw, LPCTSTR lpStr, LPVOID lpData)
{
	//
	return 0;
}

BOOL WINAPI  ImeSetCompositionString(HIMC hIMC, DWORD dwIndex, LPCVOID lpComp, DWORD dwComp, LPCVOID lpRead, DWORD dwRead)
{
	//
	return FALSE;
}

