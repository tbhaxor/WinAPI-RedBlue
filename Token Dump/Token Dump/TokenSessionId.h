#pragma once


#include "Utils.h"
#include "ErrorHandling.h"

/// <summary>
/// Get the information of the login session associated with the token
/// </summary>
/// <param name="hTok">HANDLE of the token</param>
VOID PrintTokenSessionId(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenSessionId);
	DWORD dwRetLen;
	
	LPDWORD lpSessionId = (LPDWORD)VirtualAlloc(nullptr, dwTokLen, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	if (!GetTokenInformation(hTok, TokenSessionId, lpSessionId, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()", FALSE);
	} else {
		std::wcout << L"[+] Logon Session ID: " << *lpSessionId << std::endl;
	}

	VirtualFree(lpSessionId, 0x0, MEM_RELEASE);
	lpSessionId = nullptr;
}