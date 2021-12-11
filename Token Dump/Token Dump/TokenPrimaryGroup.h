#pragma once
#include "ErrorHandling.h"
#include "Utils.h"

/// <summary>
/// Print the details of the primary group or the security descriptors of new objects.
/// </summary>
/// <param name="hTok">Handle of token</param>
VOID PrintTokenPrimaryGroup(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenPrimaryGroup);
	DWORD dwRetLen;

	PTOKEN_PRIMARY_GROUP tp = (PTOKEN_PRIMARY_GROUP)VirtualAlloc(NULL, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!GetTokenInformation(hTok, TokenPrimaryGroup, (LPVOID)tp, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}
	
	std::wcout << "[+] Token Primary Group: " << std::endl;

	LPWSTR lpSid = SIDSerialize(tp->PrimaryGroup);
	if (lpSid == nullptr) {
		PrintError(L"SIDSerialize()");
	} else {
		std::wcout << L"\tSID: " << lpSid << std::endl;
	}
	LocalFree(lpSid);
	lpSid = nullptr;

	WCHAR wAcc[MAX_PATH], wDom[MAX_PATH];
	SID_NAME_USE eSidType;
	if (!GetDomainAccount(tp->PrimaryGroup, wAcc, wDom, &eSidType)) {
		PrintError(L"GetDomainAccount()");
	} else {
		std::wcout << L"\tDomain\\Account (Type): " << wDom << L"\\" << wAcc << L" (" << GetSidType(eSidType) << L")" << std::endl;
	}

	VirtualFree(tp, 0x0, MEM_RELEASE);
	tp = nullptr;
}