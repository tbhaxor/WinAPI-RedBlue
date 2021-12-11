#pragma once

#include "Utils.h"
#include "ErrorHandling.h"

/// <summary>
/// Print the details of the User associated with the token user
/// </summary>
/// <param name="hTok"></param>
VOID PrintTokenUser(HANDLE hTok) {
	DWORD dwTokLength = GetTokenInfoLength(hTok, TokenUser);
	DWORD dwRetLen;
	
	PTOKEN_USER tu = (PTOKEN_USER)VirtualAlloc(NULL, dwTokLength, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (tu == nullptr) {
		PrintError(L"VirtualAlloc()");
		return;
	}
	std::wcout << L"[+] User Details" << std::endl;
	if (!GetTokenInformation(hTok, TokenUser, (LPVOID)tu, dwTokLength, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	LPWSTR lpSid = SIDSerialize(tu->User.Sid);
	if (lpSid == nullptr) {
		PrintError(L"SIDSerialize()");
	} else {
		std::wcout << L"\tSID: " << lpSid << std::endl;
	}
	LocalFree(lpSid);
	lpSid = nullptr;

	WCHAR wAcc[MAX_PATH], wDom[MAX_PATH];
	SID_NAME_USE eSidType;
	if (!GetDomainAccount(tu->User.Sid, wAcc, wDom, &eSidType)) {
		PrintError(L"GetDomainAccount()");
	} else {
		std::wcout << L"\tDomain\\Account (Type): " << wDom << L"\\" << wAcc << L" (" << GetSidType(eSidType) << L")" << std::endl;
	}

	VirtualFree(tu, 0x0, MEM_RELEASE);
	tu = nullptr;
}