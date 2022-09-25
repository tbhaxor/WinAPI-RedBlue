#include "pch.h"
#include "process_utils.h"

INT main(INT argc, CHAR** argv) {
	if (argc < 3) {
		std::cerr << "usage: " << argv[0] << " PID DLL_PATH\n";
		return 0x1;
	}

	std::cout << "Process ID: " << argv[1] << std::endl;
	DWORD dwPID = atoi(argv[1]);

	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwPID);
	if (hProcess == nullptr) {
		PrintError("OpenProcess()", TRUE);
	}

	HANDLE hThread = GetFirstThead(dwPID);
	if (hThread == NULL) {
		PrintError("GetFirstThead()", TRUE);
	}

	if (!DoInjection(hProcess, hThread, argv[2])) {
		PrintError("DoInjection()", TRUE);
	}

	std::cout << "Injected!\n";

	return 0x0;
}