#include "pch.h"

INT main(INT argc, LPSTR argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " PID /path/to/dll\n";
		return 0x1;
	}

	DWORD dwPID = atoll(argv[1]);
	
	// Permission PROCESS_VM_WRITE | PROCESS_VM_OPERATION are specifically for VirtualAlloc and WriteProcessMemory for the DLL Path
	// PROCESS_CREATE_THREAD is used for CreateRemoteThread function to work.
	HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD, FALSE, dwPID);
	if (hProcess == NULL) {
		PrintError("OpenProcess()", TRUE);
		return 0x1;
	}

	LPVOID lpBaseAddress = VirtualAllocEx(hProcess, nullptr, 1 << 12, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (lpBaseAddress == nullptr) {
		PrintError("VirtualAllocEx()", TRUE);
		return 0x0;
	}

	if (!WriteProcessMemory(hProcess, lpBaseAddress, (LPCVOID)argv[2], strlen(argv[2]), nullptr)) {
		PrintError("WriteProcessMemory()", TRUE);
	}
	
	// Kernel32 is ASLRed while booting and is then address is shared by all the processes
	HMODULE hKernel32 = GetModuleHandleA("Kernel32");
	if (hKernel32 == NULL) {
		VirtualFreeEx(hProcess, lpBaseAddress, 0x0, MEM_RELEASE);
		lpBaseAddress = nullptr;

		CloseHandle(hProcess);
		hProcess = NULL;

		PrintError("GetModuleHandleA()", TRUE);
		return 0x0;
	}
	FARPROC pLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");

	// Inherit the security attributes from the parent process (helps in getting privileges of that) with default stack size
	// Creation flag 0x0 means that thread will be started immediately and no thread ID is returned
	HANDLE hThread = CreateRemoteThread(hProcess, nullptr, NULL, (LPTHREAD_START_ROUTINE)pLoadLibraryA, lpBaseAddress, NULL, nullptr);
	if (hThread == NULL) {
		VirtualFreeEx(hProcess, lpBaseAddress, 0x0, MEM_RELEASE);
		lpBaseAddress = nullptr;

		CloseHandle(hProcess);
		hProcess = NULL;

		PrintError("CreateRemoteThread()", TRUE);
		return 0x0;
	}

	std::cout << "Injected DLL\n";
	

	CloseHandle(hProcess);
	CloseHandle(hThread);
	hProcess = hThread = nullptr;

	return 0x0;
}