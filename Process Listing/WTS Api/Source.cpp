#include <Windows.h>
#include <iostream>
#include <WtsApi32.h>
#include <system_error>
#include <sddl.h>
#include <iomanip>
#include "pch.h"

// instruction for the linker
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Advapi32.lib")

INT main(VOID) {
	// Add and confirm the debug privileges before doing anything
	if (!AddSeDebugPrivileges()) {
		// Try to evelate the process with administrator permissions with UAC prompt and exit
		SpawnElevatedProcess();
	}

	// Craft parameters for WTSEnumerateProcessesExA()
	DWORD dwLevel = 0x1;
	PWTS_PROCESS_INFO_EXA pProcesses = nullptr;
	DWORD dwProcessCount = 0x0;

	// Get the pointer processes informations for the current server handle and any user session
	// This will allow you to get a list of processes runing not only by current user but of all user including NT AUTHORITY\SYSTEM
	if (!WTSEnumerateProcessesExA(
		WTS_CURRENT_SERVER_HANDLE,
		&dwLevel,
		WTS_ANY_SESSION,
		(LPSTR*)&pProcesses,
		&dwProcessCount
	)) {
		PrintError("WTSEnumerateProcessesExA()", TRUE);
	}

	std::cout << "Total processes found: " << dwProcessCount - 1 << std::endl;

	for (DWORD c = 0; c < dwProcessCount; c++) {
		// Skip "System Idle" process as it is reserved
		// https://stackoverflow.com/a/3232487/10362396
		if ((pProcesses + c)->ProcessId == 0) {
			continue;
		}
		std::cout << "PID: " << (pProcesses + c)->ProcessId << "\t\tProcess Name: " << (pProcesses + c)->pProcessName << std::endl;
		std::cout << "# Handles: " << (pProcesses + c)->HandleCount << "\t\t# Threads: " << (pProcesses + c)->NumberOfThreads << std::endl;
		
		// Convert the SID to the string
		LPSTR lpSID = nullptr;
		if (!ConvertSidToStringSidA((pProcesses + c)->pUserSid, &lpSID)) {
			std::cout << "Account SID: " << "N/A";
		} else {
			std::cout << "Account SID: " << lpSID;
		}

		// Get the domain name and user account from SID
		CHAR szAccountName[MAX_PATH], szDomainName[MAX_PATH];
		DWORD dwMaxPathAccount = MAX_PATH;
		DWORD dwMaxPathDomain = MAX_PATH;
		SID_NAME_USE nUse;
		
		// Get the account and domain information from the SID of the process
		if (!LookupAccountSidA(
			nullptr,
			(pProcesses + c)->pUserSid,
			szAccountName,
			&dwMaxPathAccount,
			szDomainName,
			&dwMaxPathDomain,
			&nUse
		)) {
			std::cout << "\tDomain\\Account (Type): " << "N/A (N/A)" << std::endl;
		} else {
			std::cout << "\tDomain\\Account (Type): " << szDomainName << "\\" << szAccountName << " (" << GetSidType(nUse) << ")\n";
		}

		std::cout << "---------------------------------------------------------------------" << std::endl;
	}

	WTSFreeMemoryExA(WTSTypeProcessInfoLevel1, (PVOID)pProcesses, dwProcessCount);
	pProcesses = nullptr;

	system("pause");
	return 0;
}