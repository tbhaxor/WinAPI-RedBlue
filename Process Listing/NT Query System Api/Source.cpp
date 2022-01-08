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
		std::wcout << L"PID: "                        << HandleToUlong(p->UniqueProcessId) 
				   << L"\t\t\t\tSession ID: "         << p->SessionId 
			       << L"\t\t\tImage Name: "           << (p->ImageName.Buffer ? p->ImageName.Buffer : L"")     << std::endl;
		
		std::wcout << L"# Handles: "                  << p->HandleCount 
			       << L"\t\t\t\t# Threads: "          << p->NumberOfThreads                                    << std::endl;
		
		std::wcout << L"Virtual Size: "               << GetHumanReadableSize(p->VirtualSize) 
				   << L"\t\t\tPeak Virtual Size: "    << GetHumanReadableSize(p->PeakVirtualSize)              << std::endl;
		
		std::wcout << L"Pagefile Usage: "             << GetHumanReadableSize(p->PagefileUsage) 
			       << L"\t\t\tPeak Pagefile Usage: "  << GetHumanReadableSize(p->PeakPagefileUsage)            << std::endl;
		
		std::wcout << L"Working Set Size: "           << GetHumanReadableSize(p->WorkingSetSize) 
			       << L"\t\tPeak Working Set Size: "  << GetHumanReadableSize(p->PeakWorkingSetSize)           << std::endl;
		
		std::wcout << L"Quota Non-Paged Pool Usage: " << GetHumanReadableSize(p->QuotaNonPagedPoolUsage) 
			       << L"\tQuota Paged Pool Usage: "   << GetHumanReadableSize(p->QuotaPagedPoolUsage)          << std::endl;
		
		std::wcout << L"-------------------------------------------------------------------------------------" << std::endl;

		// Jump to next entry
		p = (PSYSTEM_PROCESS_INFORMATION)((PBYTE)p + p->NextEntryOffset);
	} while (p->NextEntryOffset != 0);
	
	return 0x0;
}