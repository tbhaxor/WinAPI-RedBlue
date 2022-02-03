#pragma once

#include "Utils.h"
#include "ErrorHandling.h"

/// <summary>
/// Print the details of source of this token
/// </summary>
/// <param name="hTok">HANDLE of token</param>
VOID PrintTokenSource(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenSource);
	DWORD dwRetLen;

	PTOKEN_SOURCE ts = (PTOKEN_SOURCE)VirtualAlloc(nullptr, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!GetTokenInformation(hTok, TokenSource, (LPVOID)ts, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	std::cout << "[+] Token Source " << std::endl;
	std::cout << "\tSource Name: " << ts->SourceName << std::endl;
	std::cout << "\tSource ID: " << std::hex << std::uppercase << ts->SourceIdentifier.HighPart << "-" << ts->SourceIdentifier.LowPart << std::nouppercase << std::dec << std::endl;

	VirtualFree(ts, 0x0, MEM_RELEASE);
	ts = nullptr;
}