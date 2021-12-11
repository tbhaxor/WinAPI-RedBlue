#pragma once

#include "Utils.h"
#include "ErrorHandling.h"

/// <summary>
/// Get the token elevation details of the token process. This will be different for different process
/// </summary>
/// <param name="hTok">HANDLE of the token</param>
VOID PrintTokenElevation(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenElevation);
	DWORD dwRetLen;

	PTOKEN_ELEVATION te = (PTOKEN_ELEVATION)VirtualAlloc(nullptr, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!GetTokenInformation(hTok, TokenElevation, (LPVOID)te, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	std::wcout << L"[+] Token Elevation" << std::endl;
	if (te->TokenIsElevated) {
		// If the process is elevated, then get the elevation type
		std::wcout << "\tStatus: Elevated" << std::endl;
		dwTokLen = GetTokenInfoLength(hTok, TokenElevationType);
		
		PTOKEN_ELEVATION_TYPE t = (PTOKEN_ELEVATION_TYPE)VirtualAlloc(nullptr, dwTokLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!GetTokenInformation(hTok, TokenElevationType, (LPVOID)t, dwTokLen, &dwRetLen)) {
			PrintError(L"GetTokenInformation()");
		} else {
			switch (*t) {
			case TokenElevationTypeDefault:
				std::wcout << "\tType: Default - The token does not have a linked token\n";
				break;
			case TokenElevationTypeFull:
				std::wcout << "\tType: Full - The token is an elevated token\n";
				break;
			case TokenElevationTypeLimited:
				std::wcout << "\tType: Limited - The token is a limited token\n";
				break;
			default:
				break;
			}
		}
		VirtualFree(t, 0x0, MEM_RELEASE);
		t = nullptr;
	} else {
		std::wcout << "\tStatus: Not Elevated" << std::endl;
	}

	VirtualFree(te, 0x0, MEM_RELEASE);
	te = nullptr;
}