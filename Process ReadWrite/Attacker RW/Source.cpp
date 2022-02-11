#include "pch.h"

int main(DWORD argc, LPCSTR argv[]) {
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " <PID> <BASE ADDRESS> <TEXT> [<PAGES>]\n";
		return 0x1;
	}
	
	DWORD dwPages = argc == 5 ? atoll(argv[4]) : 1; // number of pages to fetch
	DWORD dwPID = atoll(argv[1]);

	// convert the hex address to long long (decimal base)
	LONGLONG llBaseAddr;
	if (!StrToInt64ExA(argv[2], STIF_SUPPORT_HEX, &llBaseAddr)) {
		PrintError("StrToInt64ExA()", TRUE);
	}

	// get the process handle with VM read/write permissions
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwPID);
	if (hProcess == NULL) {
		PrintError("OpenProcess()", TRUE);
	}

	// read the buffer from the remote process and store in the local buffer
	LPVOID lpReadBuffer = (LPVOID)VirtualAlloc(nullptr, PAGE_SIZE * dwPages, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (lpReadBuffer == nullptr) {
		PrintError("VirtualAlloc()", TRUE);
	}
	
	if (!ReadProcessMemory(hProcess, (LPVOID)llBaseAddr, lpReadBuffer, PAGE_SIZE * dwPages, nullptr)) {
		PrintError("ReadProcessMemory()", TRUE);
	} else {
		std::cout << "Read Buffer: " << (LPSTR)lpReadBuffer << std::endl;
	}

	// write into process memory
	if (!WriteProcessMemory(hProcess, (LPVOID)llBaseAddr, argv[3], strlen(argv[3]) + 1, nullptr)) {
		PrintError("WriteProcessMemory()", TRUE);
	}

	std::cout << "Memory tamper succeeded!" << std::endl;
}