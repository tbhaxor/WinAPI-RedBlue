#pragma once

#include "Utils.h"
#include "ErrorHandling.h"

/// <summary>
/// Print whether the token has UI access or not
/// </summary>
/// <param name="hTok">HANDLE of the token</param>
VOID PrintTokenUIAccess(HANDLE hTok) {
	// Get the token informaton length for TokenUIAccess
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenUIAccess);
	DWORD dwRetLen;

	// Get the information for TokenUIAccess
	LPDWORD lpUI = (LPDWORD)VirtualAlloc(nullptr, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!GetTokenInformation(hTok, TokenUIAccess, (LPVOID)lpUI, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	if (*lpUI) {
		std::wcout << "[+] UI Access: Yes\n";
	} else {
		std::wcout << "[+] UI Access: No\n";
	}

	// Release the memory
	VirtualFree(lpUI, 0x0, MEM_RELEASE);
	lpUI = nullptr;
}