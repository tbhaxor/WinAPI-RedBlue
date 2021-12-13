#include "pch.h"

INT main(INT argc, PCHAR argv[]) {
	if (argc < 4) {
		printf("Usage: %s <PID> <Base Address> <Text>\n", argv[0]);
		return 0x1;
	}

	if (!AddSeDebugPrivileges()) {
		PrintError("AddSeDebugPrivileges()", TRUE);
	}

	DWORD dwPID = atoi(argv[1]);
	LONGLONG addr;
	SIZE_T dwRet;
	SIZE_T dwLen = strlen(argv[3]);
	StrToInt64ExA(argv[2], STIF_SUPPORT_HEX, &addr);
	
	CHAR buffer[100];
	memset(buffer, 0x0, 100);
	sprintf(buffer, "%s", argv[2]);

	HANDLE hProc = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwPID);
	if (hProc == INVALID_HANDLE_VALUE) {
		PrintError("OpenProcess()", TRUE);
	}

	if (!WriteProcessMemory(hProc, (LPVOID)addr, (LPCVOID)buffer, dwLen, &dwRet)) {
		PrintError("WriteProcessMemory()", TRUE);
	} else {
		printf("Successfully tampered process memory\n");
	}
}