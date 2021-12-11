#pragma once

#include "Utils.h"
#include "ErrorHandling.h"

/// <summary>
/// Get check whether the token virtualized is allowed / enabled or not
/// </summary>
/// <param name="hTok">HANDLE of the token</param>
VOID PrintTokenVirtualization(HANDLE hTok) {
	DWORD dwTokLen, dwRetLen;

	// Get the information length of TokenVirtualizationAllowed
	dwTokLen = GetTokenInfoLength(hTok, TokenVirtualizationAllowed);
	
	// Get the information of TokenVirtualizationAllowed and store in lpVA
	LPDWORD lpVA = (LPDWORD)VirtualAlloc(nullptr, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!GetTokenInformation(hTok, TokenVirtualizationAllowed, (LPVOID)lpVA, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	std::wcout << L"[+] Token Virtualization" << std::endl;
	if (*lpVA) {
		std::wcout << "\tIs Allowed: Yes\n";
	} else {
		std::wcout << "\tIs Allowed: No\n";

	}

	// Get the information length of TokenVirtualizationEnabled
	dwTokLen = GetTokenInfoLength(hTok, TokenVirtualizationEnabled);

	// Get the information for the TokenVirtualizationEnabled
	LPDWORD lpVE = (LPDWORD)VirtualAlloc(nullptr, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!GetTokenInformation(hTok, TokenVirtualizationEnabled, (LPVOID)lpVA, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	if (*lpVA) {
		std::wcout << "\tIs Enabled: Yes\n";
	} else {
		std::wcout << "\tIs Enabled: No\n";

	}

	// Release the memory
	VirtualFree(lpVA, 0x0, MEM_RELEASE);
	VirtualFree(lpVE, 0x0, MEM_RELEASE);
	lpVA = nullptr;
	lpVE = nullptr;
}