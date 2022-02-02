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
	
	DWORD dwSessionId;
	if (!GetTokenInformation(hTok, TokenSessionId, &dwSessionId, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()", FALSE);
	} else {
		std::wcout << L"[+] Logon Session ID: " << dwSessionId << std::endl;
	}
}