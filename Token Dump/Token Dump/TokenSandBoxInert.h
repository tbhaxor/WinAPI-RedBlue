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
	
	DWORD dwSandBoxInert;
	if (!GetTokenInformation(hTok, TokenSandBoxInert, &dwSandBoxInert, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()", FALSE);
	} else if (dwSandBoxInert) {
		std::wcout << L"[+] Sandbox Inert: Exists in Token\n";
	} else {
		std::wcout << L"[+] Sandbox Inert: Does not exists in Token\n";
	}
}