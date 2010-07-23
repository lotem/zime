// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "WeaselIME.h"

#include <cstdlib>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	WeaselIME::SetModuleInstance(hModule);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			HRESULT hr = WeaselIME::RegisterUIClass();
			if (FAILED(hr))
				return FALSE;
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		WeaselIME::Cleanup();
		WeaselIME::UnregisterUIClass();
		break;
	}
	return TRUE;
}

BOOL _CopyFile(const wstring& src, const wstring& dest)
{
	BOOL ret = CopyFile(src.c_str(), dest.c_str(), FALSE);
	if (!ret)
	{
		wstring old = (boost::wformat(L"%1%.old.%2%") % dest % rand()).str();
		if (MoveFileEx(dest.c_str(), old.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))
		{
			MoveFileEx(old.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
		}
		ret = CopyFile(src.c_str(), dest.c_str(), FALSE);
	}
	return ret;
}

BOOL _DeleteFile(const wstring& file)
{
	BOOL ret = DeleteFile(file.c_str());
	if (!ret)
	{
		ret = MoveFileEx(file.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
	}
	return ret;
}

//
// rundll32
//

extern "C" __declspec(dllexport)
void RegisterIME(HWND hWnd, HINSTANCE hInstance, LPWSTR lpszCmdLine, int nCmdShow)
{
	WCHAR modulePath[MAX_PATH];
	GetModuleFileName(WeaselIME::GetModuleInstance(), modulePath, _countof(modulePath));
	wstring srcPath = modulePath;
	WCHAR sysPath[MAX_PATH];
	ExpandEnvironmentStrings(L"%SystemRoot%\\system32\\", sysPath, _countof(sysPath));
	wstring destPath = sysPath;
	destPath += WeaselIME::GetIMEFileName();
	if (!_CopyFile(srcPath, destPath))
	{
		MessageBox(hWnd, destPath.c_str(), L"安裝失敗", MB_ICONERROR | MB_OK);
		return;
	}
	HKL hkl = ImmInstallIME(destPath.c_str(), WeaselIME::GetIMEName());
	if (!hkl)
	{
		DWORD dwErr = GetLastError();
		WCHAR msg[100];
		wsprintf(msg, L"ImmInstallIME %x", dwErr);
		MessageBox(hWnd, msg, L"安裝失敗", MB_ICONERROR | MB_OK);
		return;
	}
	MessageBox(hWnd, L":)", L"安裝完成", MB_OK);
}

extern "C" __declspec(dllexport)
void UnregisterIME(HWND hWnd, HINSTANCE hInstance, LPWSTR lpszCmdLine, int nCmdShow)
{
	HKEY hKey;
	LSTATUS ret = RegOpenKey(HKEY_LOCAL_MACHINE, L"", &hKey);
	if (ret != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		MessageBox(hWnd, L"註冊表操作錯誤", L"卸載失敗", MB_ICONERROR | MB_OK);
		return;
	}

	for (int i = 0; true; ++i)
	{
		WCHAR subKey[16];
		ret = RegEnumKey(hKey, i, subKey, _countof(subKey));
		if (ret != ERROR_SUCCESS)
			break;
		if (wcscmp(subKey + 4, L"0404") == 0 || wcscmp(subKey + 4, L"0804") == 0)
		{
			HKEY hSubKey;
			ret = RegOpenKey(hKey, subKey, &hSubKey);
			if (ret != ERROR_SUCCESS)
				continue;
			WCHAR imeFile[32];
			DWORD len = sizeof(imeFile);
			DWORD type = 0;
			ret = RegQueryValueEx(hSubKey, L"Ime File", NULL, &type, (LPBYTE)imeFile, &len);
			RegCloseKey(hSubKey);
			if (ret != ERROR_SUCCESS)
				continue;
			if (_wcsicmp(imeFile, WeaselIME::GetIMEFileName()) == 0)
			{
				ret = RegDeleteKey(hKey, subKey);
				if (ret != ERROR_SUCCESS)
					continue;
			}
		}
	}

	WCHAR sysPath[MAX_PATH];
	ExpandEnvironmentStrings(L"%SystemRoot%\\system32\\", sysPath, _countof(sysPath));
	wstring file = sysPath;
	file += WeaselIME::GetIMEFileName();
	if (!_DeleteFile(file))
	{
		MessageBox(hWnd, file.c_str(), L"卸載失敗", MB_ICONERROR | MB_OK);
		return;
	}
	MessageBox(hWnd, L":)", L"卸載完成", MB_OK);
}
