#pragma once

#include "Utils.h"
#include "ErrorHandling.h"

/// <summary>
/// Get the token mandatory policy for the process
/// </summary>
/// <param name="hTok">HANDLE to the token</param>
VOID PrintTokenPolicy(HANDLE hTok) {
	// Get the size of the token information
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenMandatoryPolicy);
	DWORD dwRetLen;
	
	// Get the token information for TokenMandatoryPolicy
	PTOKEN_MANDATORY_POLICY tp  = (PTOKEN_MANDATORY_POLICY)VirtualAlloc(nullptr, dwTokLen, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!GetTokenInformation(hTok, TokenMandatoryPolicy, (LPVOID)tp, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}

	std::wcout << "[+] Token Policy: ";
	switch (tp->Policy) {
	case TOKEN_MANDATORY_POLICY_OFF:
		std::wcout << "No policy is enforced\n";
		break;
	case TOKEN_MANDATORY_POLICY_NO_WRITE_UP:
		std::wcout << "Can not write to objects that have a greater mandatory integrity level\n";
		break;
	case TOKEN_MANDATORY_POLICY_NEW_PROCESS_MIN:
		std::wcout << "Integrity level is lesser than of the parent-process integrity level and the executable-file integrity level\n";
		break;
	case TOKEN_MANDATORY_POLICY_VALID_MASK:
		std::wcout << "Both of NO_WRITE_UP and NEW_PROCESS_MIN flags are set\n";
		break;
	default:
		std::wcout << "N/A\n";
		break;
	}

	// Release the memory
	VirtualFree(tp, 0x0, MEM_RELEASE);
	tp = nullptr;
}