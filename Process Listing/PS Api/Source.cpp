#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include "pch.h"

int main(VOID) {
	// Add debug privileges to the current process if available
	if (!AddSeDebugPrivileges()) {
		// Spawn a new process with administrator UAC and exit the current process
		SpawnElevatedProcess();
	}


	// Enumerate processes usign EnumProcess function
	DWORD processes[1024], dwNeeded;
	if (!EnumProcesses(processes, sizeof(processes), &dwNeeded)) {
		PrintError("EnumProcesses()", TRUE);
	}
	DWORD dwCount = dwNeeded / sizeof(DWORD);
	
	// Iterate over processes id array and PrintProcessDetails()
	for (DWORD i = 0;i < dwCount;i++) {
		PrintProcessDetails(processes[i]);
	}

	return 0;
}