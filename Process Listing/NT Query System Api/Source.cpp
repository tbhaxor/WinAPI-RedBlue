#include "pch.h"

INT main(VOID) {
	DWORD dwRet;
	DWORD dwSize = 0x0;
	NTSTATUS dwStatus = STATUS_INFO_LENGTH_MISMATCH;
	PSYSTEM_PROCESS_INFORMATION p = nullptr;
	

	while (TRUE) {
		if (p != nullptr) VirtualFree(p, 0x0, MEM_RELEASE);
	
		// try to get the information details
		p = (PSYSTEM_PROCESS_INFORMATION)VirtualAlloc(nullptr, dwSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		dwStatus = NtQuerySystemInformation(SystemProcessInformation, (PVOID)p, (ULONG)dwSize, &dwRet);
		
		// if success, break the loop and proceed to printing details
		// if different error than information length mismatch, exit prorgram with error message
		if (dwStatus == STATUS_SUCCESS) { break; }
		else if (dwStatus != STATUS_INFO_LENGTH_MISMATCH) {
			VirtualFree(p, 0x0, MEM_RELEASE);
			p = nullptr;
			std::cout << "Error fetching details" << std::endl;
			return 0x1;
		}
		
		// use the dwRet value and add extra 8kb buffer size
		// this will become handy when new processes are created while processing this loop
		dwSize = dwRet + (2 << 12);
	}

	// Print process details
	do {
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
	
	// Free the process buffer
	VirtualFree(p, 0x0, MEM_RELEASE);
	p = nullptr;

	return 0x0;
}