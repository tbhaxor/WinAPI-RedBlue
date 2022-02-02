#include "pch.h"

INT wmain(DWORD argc, PCWSTR* argv) {
	if (argc < 2) {
		std::wcout << L"Usage: \"" << argv[0] << L"\" <PID>" << std::endl;
		return  0x1;
	}
	DWORD dwPID = _wtol(argv[1]);
	
	// Try adding SeDebugPrivileges or continue unprivileged process
	if (!AddSeDebugPrivileges()) {
		PrintError(L"AddSeDebugPrivileges()");
	}


	// Open process token with QUERY_INFORMATION
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
	if (hProc == NULL || hProc == INVALID_HANDLE_VALUE) {
		// In case of protected process, open token to query limited information
		hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwPID);
		if (hProc == NULL || hProc == INVALID_HANDLE_VALUE) {
			PrintError(L"OpenProcess()", FALSE);
		}
	}

	// Print basic process details
	PrintBasicProcessDetails(dwPID);

	// Open the process token with the process handle to query basic information and its source
	HANDLE hTok;
	if (!OpenProcessToken(hProc, TOKEN_QUERY | TOKEN_QUERY_SOURCE, &hTok)) {
		PrintError(L"OpenProcessToken()", TRUE);
	}

	// Print all the token information
	PrintTokenSessionId(hTok);
	PrintTokenStatistics(hTok);
	PrintTokenUser(hTok);
	PrintTokenGroup(hTok);
	PrintTokenPrivilege(hTok);
	PrintTokenOwner(hTok);
	PrintTokenPrimaryGroup(hTok);
	PrintTokenSource(hTok);
	PrintTokenType(hTok);
	PrintTokenElevation(hTok);
	PrintTokenIsRestricted(hTok);
	PrintTokenVirtualization(hTok);
	PrintTokenUIAccess(hTok);
	PrintTokenPolicy(hTok);
	PrintTokenSandboxInert(hTok);

	// Close handlers and release memory
	CloseHandle(hProc);
	CloseHandle(hTok);
	hProc = nullptr;
	hTok = nullptr;
}