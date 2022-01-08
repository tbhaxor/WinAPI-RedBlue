#include "pch.h"

INT main(DWORD argc, PCHAR argv[]) {
	// Validate command line arguments
	if (argc < 4) {
		printf("Usage: %s <PID> <Base Address> <Length>\n", argv[0]);
		return 0x1;
	}

	// Add the debug privileges
	if (!AddSeDebugPrivileges()) {
		PrintError("AddSeDebugPrivileges()", TRUE);
	}
	
	// Perform argument conversions
	DWORD dwPID = atoi(argv[1]);
	DWORD dwBuffLen = atoi(argv[3]);
	LONGLONG addr;
	SIZE_T bytesRead;
	StrToInt64ExA(argv[2], STIF_SUPPORT_HEX, &addr);

	// Open process handle with desired access to read Virtual Memory
	HANDLE hProc = OpenProcess(PROCESS_VM_READ, FALSE, dwPID);
	if (hProc == INVALID_HANDLE_VALUE) {
		PrintError("OpenProcess()", TRUE);
	}

	std::cout << "Reading Address: " << argv[2] << " of PID=" << dwPID << std::endl;
	
	// Allocate the buffer for amount you want to read
	LPSTR lpBuffer = (LPSTR)VirtualAlloc(nullptr, dwBuffLen + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	// Read memory and store the data in the buffer
	if (!ReadProcessMemory(hProc, (LPCVOID)addr, (LPVOID)lpBuffer, 95, &bytesRead)) {
		PrintError("ReadProcessMemory()", FALSE);
	} else {
		std::cout << "Success! Buffer Read: " << lpBuffer << std::endl;
	}

	// Free the buffer
	VirtualFree(lpBuffer, 0x0, MEM_RELEASE);
	lpBuffer = nullptr;
}