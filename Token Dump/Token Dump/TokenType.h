#pragma once

#include "Utils.h"
#include "ErrorHandling.h"

/// <summary>
/// Print whether the token is primary or impersonated. If it is impersonated, then also print the level of its impersonation
/// </summary>
/// <param name="hTok">HANDLE of the token</param>
VOID PrintTokenType(HANDLE hTok) {
	DWORD dwTokLen = GetTokenInfoLength(hTok, TokenType);
	DWORD dwRetLen;

	PTOKEN_TYPE t = (PTOKEN_TYPE)VirtualAlloc(nullptr, dwTokLen, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	PSECURITY_IMPERSONATION_LEVEL il = (PSECURITY_IMPERSONATION_LEVEL)VirtualAlloc(nullptr, dwTokLen, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (!GetTokenInformation(hTok, TokenType, (LPVOID)t, dwTokLen, &dwRetLen)) {
		PrintError(L"GetTokenInformation()");
		return;
	}
	
	std::wcout << L"[+] Token Type: ";

	switch (*t) {
	case TokenPrimary:
		std::wcout << L"Primary\n";
		break;
	case TokenImpersonation:
		std::wcout << L"Impersonate\n";
		// If it's an impersonation token, then print the level of impersonation
		dwTokLen = GetTokenInfoLength(hTok, TokenImpersonationLevel);
		if (!GetTokenInformation(hTok, TokenImpersonationLevel, (LPVOID)il, dwTokLen, &dwRetLen)) {
			PrintError(L"GetTokenInformation()");
		} else {
			std::wcout << "[+] Impersonation Level: ";
			switch (*il) {
			case SecurityAnonymous:
				std::wcout << L"Anonymous - Cannot obtain identification information about the client\n";
				break;
			case SecurityIdentification:
				std::wcout << L"Identification - Can obtain information about the client, but not impersonate it\n";
				break;
			case SecurityImpersonation:
				std::wcout << L"Impersonation - Can impersonate the client's security context on its local system\n";
				break;
			case SecurityDelegation:
				std::wcout << L"Delegation - Can impersonate the client's security context on remote systems\n";
				break;
			default:
				std::wcout << L"N/A\n";
				break;
			}
		}
		break;
	default:
		std::wcout << L"N/A\n";
	}

	VirtualFree(t, 0x0, MEM_RELEASE);
	t = nullptr;
	VirtualFree(il, 0x0, MEM_RELEASE);
	il = nullptr;
}