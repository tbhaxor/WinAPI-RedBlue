#include "pch.h"
#include <iostream>
#include <string>

INT main(VOID) {
	DWORD dwRet;
	PSYSTEM_PROCESS_INFORMATION p = nullptr;
	
	// Get the buffer size
	NtQuerySystemInformation(SystemProcessInformation, (PVOID)p, (ULONG)0x0, (PULONG)&dwRet);
	free(p);
	
	p = nullptr;

	// Add some extra to the buffer
	DWORD dwSize = dwRet + (1 << 18);
	p = (PSYSTEM_PROCESS_INFORMATION)malloc(dwSize);

	// Get information of the processes
	if (NtQuerySystemInformation(SystemProcessInformation, (PVOID)p, (ULONG)dwSize, (PULONG)&dwRet) != 0) {
		std::cout << "Error fetching details" << std::endl;
		free(p);
		p = nullptr;
		return 0x1;
	}

	do {
		// Print process details
		std::cout << "PID: " << HandleToUlong(p->UniqueProcessId) << "\tSession ID: " << p->SessionId << "\tImage Name: ";
		std::wcout << (p->ImageName.Buffer ? p->ImageName.Buffer : L"") << std::endl;
		std::cout << "# Handles: " << p->HandleCount << "\t\t\t# Threads: " << p->NumberOfThreads << std::endl;
		std::cout << "Virtual Size: "  << p->VirtualSize << "\tPeak Virtual Size: " << p->PeakVirtualSize << std::endl;
		std::cout << "Pagefile Usage: " << p->PagefileUsage << "\tPeak Pagefile Usage: " << p->PeakPagefileUsage << std::endl;
		std::cout << "Working Set Size: " << p->WorkingSetSize << "\tPeak Working Set Size: " << p->PeakWorkingSetSize << std::endl;
		std::cout << "Quota Non-Paged Pool Usage: " << p->QuotaNonPagedPoolUsage << "\tQuota Paged Pool Usage: " << p->QuotaPagedPoolUsage << std::endl;
		std::cout << "-------------------------------------------------------------------------------------" << std::endl;

		// Jump to next entry
		p = (PSYSTEM_PROCESS_INFORMATION)((PBYTE)p + p->NextEntryOffset);
	} while (p->NextEntryOffset != 0);
	
	return 0x0;
}