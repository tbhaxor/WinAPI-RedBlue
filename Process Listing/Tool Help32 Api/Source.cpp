#include "pch.h"
#include <TlHelp32.h>


int wmain() {
	// Add the SeDebugPrivilege if available or continue unprivilege process
	if (!AddSeDebugPrivileges()) {
		PrintError("AddSeDebugPrivileges()");
	}

	// Get snapshot of all the processes
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0x0);
		if (hSnapShot == NULL || hSnapShot == INVALID_HANDLE_VALUE) {
		PrintError("CreateToolhelp32Snapshot()", TRUE);
	}
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapShot, &pe);
	
	// Iterate over all the processes and get the next entry from Process32Next
	do {
		std::wcout << L"PID: " << pe.th32ProcessID << L"\tParent PID: " << pe.th32ParentProcessID << std::endl;
		std::wcout << L"# Threads: " << pe.cntThreads  << L"\tProcess Name: " << pe.szExeFile << std::endl;
			
		// Print list of all the threads
		ListProcessThreads(pe.th32ProcessID);

		// Print list of all the modules 
		ListProcessModules(pe.th32ProcessID);
		
		std::wcout << L"------------------------------------------------------" << std::endl;
	} while (Process32Next(hSnapShot, &pe));
	
	// Close handle and release memory
	CloseHandle(hSnapShot);
	hSnapShot = NULL;

	return 0;
}