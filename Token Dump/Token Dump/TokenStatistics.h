#pragma once

#include "Utils.h"
#include "ErrorHandling.h"
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

/// <summary>
/// 
/// </summary>
/// <param name="hTok"></param>
VOID PrintTokenStatistics(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenStatistics);
	DWORD dwRetLen;

	PTOKEN_STATISTICS pStats = (PTOKEN_STATISTICS)VirtualAlloc(nullptr, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!GetTokenInformation(hTok, TokenStatistics, (LPVOID)pStats, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()", FALSE);
	} else {
		
		std::wcout << L"[+] Token Statistics:\n";
		std::wcout << L"\tToken ID: 0x" << std::hex << std::uppercase << pStats->TokenId.HighPart << L"-" << pStats->TokenId.LowPart << std::nouppercase << std::dec << std::endl;
		std::wcout << L"\tAuthentication ID: 0x" << std::hex << std::uppercase << pStats->AuthenticationId.HighPart << L":" << pStats->AuthenticationId.LowPart << std::nouppercase  << std::dec << std::endl;
		
		LPWSTR lpDynamicCharged = (LPWSTR)VirtualAlloc(nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		LPWSTR lpDynamicAvailable = (LPWSTR)VirtualAlloc(nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		LPWSTR lpDynamicTotal = (LPWSTR)VirtualAlloc(nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		StrFormatByteSize(pStats->DynamicCharged, lpDynamicCharged, MAX_PATH);
		StrFormatByteSize(pStats->DynamicAvailable, lpDynamicAvailable, MAX_PATH);
		StrFormatByteSize(pStats->DynamicCharged + pStats->DynamicCharged, lpDynamicTotal, MAX_PATH);

		std::wcout << L"\tMemory Usage: " << lpDynamicCharged << L" / " << lpDynamicTotal << L"\t(" << lpDynamicAvailable << L" left)" << std::endl;
		std::wcout << L"\tTotal Groups Count: " << pStats->GroupCount << std::endl;
		std::wcout << L"\tTotal Privileges Count: " << pStats->PrivilegeCount << std::endl;
		std::wcout << L"\tModified ID: 0x" << std::hex << std::uppercase << pStats->ModifiedId.HighPart << L"-" << pStats->ModifiedId.LowPart << std::nouppercase << std::dec << std::endl;
		
		VirtualFree(lpDynamicAvailable, 0x0, MEM_RELEASE);
		VirtualFree(lpDynamicCharged, 0x0, MEM_RELEASE);
		VirtualFree(lpDynamicTotal, 0x0, MEM_RELEASE);
		lpDynamicAvailable = lpDynamicCharged = lpDynamicTotal = nullptr;
	}

	VirtualFree(pStats, 0x0, MEM_RELEASE);
	pStats = nullptr;
}