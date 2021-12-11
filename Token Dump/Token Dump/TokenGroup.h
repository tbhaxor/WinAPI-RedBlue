#pragma once
#include "ErrorHandling.h"
#include "Utils.h"
#include <iomanip>

/// <summary>
/// Print the list of the groups
/// </summary>
/// <param name="hTok"></param>
VOID PrintTokenGroup(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenGroups);
	DWORD dwRetLen;
	
	PTOKEN_GROUPS tg = (PTOKEN_GROUPS)VirtualAlloc(NULL, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	
	if (!GetTokenInformation(hTok, TokenGroups, (LPVOID)tg, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	std::wcout << L"[+] Groups" << std::endl;
	for (DWORD c = 0;c < tg->GroupCount;c++) {
		std::wcout << L"\t[" << std::setw(2) << c + 1 << L"] ";
		
		// Get the SID string for SID pointer
		LPWSTR lpSid = SIDSerialize(tg->Groups[c].Sid);
		if (lpSid == nullptr) {
			PrintError(L"SIDSerialize()");
		} else {
			std::wcout << L"SID: " << lpSid << std::endl;
		}
		LocalFree(lpSid);
		lpSid = nullptr;

		// Get the account name, domain name and its type from the SID value
		WCHAR wAcc[MAX_PATH], wDom[MAX_PATH];
		SID_NAME_USE eSidType;
		if (!GetDomainAccount(tg->Groups[c].Sid, wAcc, wDom, &eSidType)) {
			PrintError(L"GetDomainAccount()");
		} else {
			std::wcout << L"\t     Domain\\Account (Type): " << wDom << L"\\" << wAcc << L" (" << GetSidType(eSidType) << L")" << std::endl;
		}
	}

	VirtualFree(tg, 0x0, MEM_RELEASE);
	tg = nullptr;
}