#pragma once
#include "ErrorHandling.h"
#include "Utils.h"
#include "ProcessDetails.h"
#include <iomanip>

/// <summary>
/// Print the privileges associated with an access token and determines whether the privileges are enabled.
/// </summary>
/// <param name="hTok">HANDLE of token</param>
VOID PrintTokenPrivilege(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenPrivileges);
	DWORD dwRetLen;

	PTOKEN_PRIVILEGES tp = (PTOKEN_PRIVILEGES)malloc(dwTokLen); // (NULL, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	
	if (!GetTokenInformation(hTok, TokenPrivileges, (LPVOID)tp, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	std::wcout << L"[+] Token Privileges" << std::endl;
	
	for (DWORD c = 0; c < tp->PrivilegeCount; c++) {
		LPWSTR lpName = (LPWSTR)VirtualAlloc(NULL, 1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		LPWSTR lpDisplayName = (LPWSTR)VirtualAlloc(NULL, 1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		DWORD dwName, dwLangId, dwDisplayName;
		dwName = dwDisplayName = 1000;

		std::wcout << "\t[" << std::setw(2) << c + 1 << L"] ";
		
		// Get the name of the privilege from LUID
		if (!LookupPrivilegeNameW(NULL, &tp->Privileges[c].Luid, lpName, &dwName)) {
			PrintError(L"LookupPrivilegeNameW()");
			continue;
		}
		std::wcout << L"Name: " << lpName << std::endl;
		
		// Get the description / display for the privilege by its name
		if (!LookupPrivilegeDisplayNameW(NULL, lpName, lpDisplayName, &dwDisplayName, &dwLangId)) {
			PrintError(L"LookupPrivilegeNameW()");
			continue;
		}
		std::wcout << L"\t     Description: " << lpDisplayName << std::endl;

		switch (tp->Privileges[c].Attributes) {
		case SE_PRIVILEGE_ENABLED:
			std::wcout << L"\t     Status: Enabled\n";
			break;
		case SE_PRIVILEGE_ENABLED_BY_DEFAULT:
			std::wcout << L"\t     Status: Enabled by Default\n";
			break;
		case SE_PRIVILEGE_ENABLED | SE_PRIVILEGE_ENABLED_BY_DEFAULT:
			std::wcout << L"\t     Status: Enabled by Default\n";
			break;
		case SE_PRIVILEGE_REMOVED:
			std::wcout << L"\t     Status: Removed\n";
			break;
		case SE_PRIVILEGE_USED_FOR_ACCESS:
			std::wcout << L"\t     Status: Used for Access\n";
			break;
		case 0x0:
			std::wcout << L"\t     Status: Disabled\n";
			break;
		default:
			std::wcout << L"\t     Status: N/A\n";
		}

		VirtualFree(lpName, 0x0, MEM_RELEASE);
		VirtualFree(lpDisplayName, 0x0, MEM_RELEASE);
		lpName = nullptr;
		lpDisplayName = nullptr;
	}
		
	VirtualFree(tp, 0x0, MEM_RELEASE);
	tp = nullptr;
}