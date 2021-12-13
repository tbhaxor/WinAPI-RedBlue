#include "pch.h"

INT main(DWORD argc, PCHAR argv[]) {
	// Validate command line arguments
	if (argc < 3) {
		printf("Usage: %s <PID> <Base Address>\n", argv[0]);
		return 0x1;
	}

	// Add the debug privileges
	if (!AddSeDebugPrivileges()) {
		PrintError("AddSeDebugPrivileges()", TRUE);
	}
	
	// Perform argument conversions
	DWORD  dwPID = atoi(argv[1]);
	LONGLONG addr;
	CHAR buffer[100];
	SIZE_T bytesRead;
	StrToInt64ExA(argv[2], STIF_SUPPORT_HEX, &addr);

	// Open process handle with desired access to read Virtual Memory
	HANDLE hProc = OpenProcess(PROCESS_VM_READ, FALSE, dwPID);
	if (hProc == INVALID_HANDLE_VALUE) {
		PrintError("OpenProcess()", TRUE);
	}

	printf("Reading Address: 0x%I64X of PID %d\n", addr, dwPID);
	memset(buffer, 0x0, 100);

	// Read memory and store the data in the buffer
	if (!ReadProcessMemory(hProc, (LPCVOID)addr, (LPVOID)buffer, 95, &bytesRead)) {
		PrintError("ReadProcessMemory()", TRUE);
	} else {
		printf("Success! Read: %s\n", buffer);
	}
}