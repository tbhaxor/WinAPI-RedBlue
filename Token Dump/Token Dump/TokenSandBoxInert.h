#pragma once

#include "Utils.h"
#include "ErrorHandling.h"

/// <summary>
/// Get the information for whether or not sandboxing of processes is disabled or enabled
/// </summary>
/// <param name="hTok">HANDLE of the token</param>
VOID PrintTokenSandboxInert(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenSandBoxInert);
	DWORD dwRetLen;
	
	LPDWORD lpSandBoxInert = (LPDWORD)VirtualAlloc(nullptr, dwTokLen, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (!GetTokenInformation(hTok, TokenSandBoxInert, lpSandBoxInert, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()", FALSE);
	} else if (*lpSandBoxInert) {
		std::wcout << L"[+] Sandbox Inert: Exists in Token\n";
	} else {
		std::wcout << L"[+] Sandbox Inert: Does not exists in Token\n";
	}

	VirtualFree(lpSandBoxInert, 0x0, MEM_RELEASE);
	lpSandBoxInert = nullptr;
}