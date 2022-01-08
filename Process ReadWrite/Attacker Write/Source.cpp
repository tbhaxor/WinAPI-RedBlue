#include "pch.h"

INT main(INT argc, PCHAR argv[]) {
	if (argc < 4) {
		printf("Usage: %s <PID> <Base Address> <Text>\n", argv[0]);
		return 0x1;
	}

	// Adding SeDebugPrivilege to the process
	// If the call is unsuccessfull, exit the program because it will fail during OpenProcess
	if (!AddSeDebugPrivileges()) {
		PrintError("AddSeDebugPrivileges()", TRUE);
	}

	// Handle argument conversions here
	DWORD dwPID = atoi(argv[1]);
	LONGLONG addr;
	SIZE_T dwRet;
	SIZE_T dwLen = strlen(argv[3]);
	StrToInt64ExA(argv[2], STIF_SUPPORT_HEX, &addr);
	
	// Open the process handle with VM_WRITE and VM_OPERATION permissions
	HANDLE hProc = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwPID);
	if (hProc == INVALID_HANDLE_VALUE) {
		PrintError("OpenProcess()", TRUE);
	}

	// Finally write process memory
	if (!WriteProcessMemory(hProc, (LPVOID)addr, (LPCVOID)argv[3], dwLen, &dwRet)) {
		PrintError("WriteProcessMemory()", FALSE);
	} else {
		std::cout << "Successfully tampered process memory\n";
	}

	return 0x0;
}