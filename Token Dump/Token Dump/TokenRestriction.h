#pragma once

#include "Utils.h"
#include "ErrorHandling.h"

/// <summary>
/// Print whether the token is every filtered or has some restrictions
/// </summary>
/// <param name="hTok">HANDLE of token</param>
VOID PrintTokenIsRestricted(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenHasRestrictions);
	DWORD dwRetLen;

	LPDWORD lpHasRestriction = (LPDWORD)VirtualAlloc(nullptr, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!GetTokenInformation(hTok, TokenHasRestrictions, (LPVOID)lpHasRestriction, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	if (*lpHasRestriction) {
		std::wcout << L"[+] Is Token Restricted: Yes\n";
	} else {
		std::wcout << L"[+] Is Token Restricted: No\n";
	}

	VirtualFree(lpHasRestriction, 0x0, MEM_RELEASE);
	lpHasRestriction = nullptr;
}