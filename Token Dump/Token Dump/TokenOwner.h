#pragma once
#include "ErrorHandling.h"
#include "Utils.h"

/// <summary>
/// The process can be started by another user from the image owned by different user. The TOKEN_OWNER will provide the details of new objects created by the user running the process
/// </summary>
/// <param name="hTok">HANDLE of token</param>
VOID PrintTokenOwner(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenOwner);
	DWORD dwRetLen;

	PTOKEN_OWNER to = (PTOKEN_OWNER)VirtualAlloc(NULL, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!GetTokenInformation(hTok, TokenOwner, (LPVOID)to, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	std::wcout << "[+] Token Owner: " << std::endl;

	LPWSTR lpSid = SIDSerialize(to->Owner);
	if (lpSid == nullptr) {
		PrintError(L"SIDSerialize()");
	} else {
		std::wcout << L"\tSID: " << lpSid << std::endl;
	}
	LocalFree(lpSid);
	lpSid = nullptr;

	WCHAR wAcc[MAX_PATH], wDom[MAX_PATH];
	SID_NAME_USE eSidType;
	if (!GetDomainAccount(to->Owner, wAcc, wDom, &eSidType)) {
		PrintError(L"GetDomainAccount()");
	} else {
		std::wcout << L"\tDomain\\Account (Type): " << wDom << L"\\" << wAcc << L" (" << GetSidType(eSidType) << L")" << std::endl;
	}

	VirtualFree(to, 0x0, MEM_RELEASE);
	to = nullptr;
}