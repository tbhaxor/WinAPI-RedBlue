#include "pch.h"
#include <Psapi.h>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

// Include the PSAPI library based on the version
#if PSAPI_VERSION == 0x1
	#pragma comment(lib, "Psapi.lib")
#else
	#pragma comment(lib, "Kernel32.lib")
#endif 


int main(VOID) {
	// Force add SeDebugPrivilege to the process's token
	if (!AddSeDebugPrivileges()) {
		SpawnElevatedProcess();
	}

	/* 
		When the process count is equal to the dwLimit, then there may be other processes failed to fit in the buffer because of low memory.
		Enumerate the processes until all of them fits in the buffer by doubling its size again and again.
	*/
	DWORD dwNeeded, dwLimit = 0x50, dwCount = 0x50;
	PDWORD lpProcesses = nullptr;
	while (dwCount == dwLimit) {
		VirtualFree(lpProcesses, NULL, MEM_RELEASE);
		
		if (lpProcesses != nullptr) dwLimit <<= 1;
		lpProcesses = (PDWORD)VirtualAlloc(nullptr, sizeof(DWORD) * dwLimit, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		// Enumerate processes and populate into the buffer
		if (!EnumProcesses(lpProcesses, sizeof(DWORD) * dwLimit, &dwNeeded)) {
			VirtualFree(lpProcesses, NULL, MEM_RELEASE);
			PrintError("EnumProcesses()", TRUE);
		}

		dwCount = dwNeeded / sizeof(DWORD);
	}

	std::cout << "Total Processes: " << dwCount << std::endl;

	/*
		Print the process detail
	*/
	for (DWORD i = 0;i < dwCount;i++) {
		DWORD dwPID = *(lpProcesses + i); 
		if (dwPID == 0x0) continue;
		std::cout << "Process ID: " << dwPID;
		
		// Open process with information query access rights
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
		if (hProcess == NULL) {
			std::cout << std::endl;
			PrintError("OpenProcess()");
			std::cout << "-------------------------------------------------\n";
			continue;
		}

		// Print the base name of the process
		LPSTR lpBaseName = (LPSTR)VirtualAlloc(nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!GetModuleBaseNameA(hProcess, NULL, lpBaseName, MAX_PATH)) {
			std::cout << std::endl;
			PrintError("GetModuleBaseNameA()");
		} else {
			std::cout << "\tProcess Name: " << lpBaseName << std::endl;
		}
		VirtualFree(lpBaseName, 0x0, MEM_RELEASE);
		lpBaseName = nullptr;

		// Print the complete image path of the process
		// Image is basically the executable file which is being loaded by the loader
		LPSTR lpImageName = (LPSTR)VirtualAlloc(nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!GetProcessImageFileNameA(hProcess, lpImageName, MAX_PATH)) {
			std::cout << std::endl;
			PrintError("GetProcessImageFileNameA()");
		
		} else {
			std::cout << "Process Image: " << lpImageName << std::endl;
		}
		VirtualFree(lpImageName, 0x0, MEM_RELEASE);
		lpImageName = nullptr;

		// Enumerate the process modules
		dwLimit = 0x50, dwCount = 0x50;
		HMODULE* lpModules = nullptr; 
		BOOL bModSuccess = TRUE;
		std::cout << "Modules List: ";
		while (dwLimit == dwCount) {
			VirtualFree(lpModules, 0x0, MEM_RELEASE);

			if (lpModules != nullptr) dwLimit <<= 1;
			lpModules = (HMODULE*)VirtualAlloc(nullptr, sizeof(HMODULE) * dwLimit, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			
			if (!EnumProcessModulesEx(hProcess, lpModules, sizeof(HMODULE) * dwLimit, &dwNeeded, LIST_MODULES_ALL)) {
				PrintError("EnumProcessModulesEx()");
				bModSuccess = FALSE;
				break;
			}

			dwCount = dwNeeded / sizeof(HMODULE);
		}

		if (bModSuccess) {
			std::cout << dwCount << " modules found" << std::endl;
			for (DWORD j = 0;j < dwCount;j++) {
				HMODULE hMod = *(lpModules + j);
				std::cout << "\tOrdinal Number: " << j;
				
				// Print the base name of the module
				LPSTR lpBaseName = (LPSTR)VirtualAlloc(nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				if (!GetModuleBaseNameA(hProcess, hMod, lpBaseName, MAX_PATH)) {
					std::cout << std::endl;
					PrintError("GetModuleBaseNameA()");
				} else {
					std::cout << "\tName: " << lpBaseName << std::endl;
				}
				VirtualFree(lpBaseName, 0x0, MEM_RELEASE);
				lpBaseName = nullptr;

				// Print the file path of the module
				LPSTR lpFileName = (LPSTR)VirtualAlloc(nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				if (!GetModuleFileNameExA(hProcess, hMod, lpFileName, MAX_PATH)) {
					PrintError("GetModuleFileNameExA()");
				} else {
					std::cout << "\tFile Name: " << lpFileName << std::endl;
				}
				VirtualFree(lpFileName, MAX_PATH, MEM_RELEASE);
				lpFileName = nullptr;
				
				// Print the module related information
				LPMODULEINFO lpModInfo = (LPMODULEINFO)VirtualAlloc(nullptr, sizeof(MODULEINFO), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				if (!GetModuleInformation(hProcess, hMod, lpModInfo, sizeof(MODULEINFO))) {
					PrintError("GetModuleInformation()");
				} else {
					PSTR lpSize = (PSTR)VirtualAlloc(nullptr, 100, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					StrFormatByteSizeA(lpModInfo->SizeOfImage, lpSize, 100);

					std::cout << "\tEntry Point: 0x" << std::hex << lpModInfo->EntryPoint
						<< "\t\tBase of DLL: 0x" << lpModInfo->lpBaseOfDll
						<< "\t\tSize of Image: " << std::dec << lpSize << std::endl;

					VirtualFree(lpSize, 0x0, MEM_RELEASE);
					lpSize = nullptr;
				}
				VirtualFree(lpModInfo, 0x0, MEM_RELEASE);
				lpModInfo = nullptr;

				std::cout << "\t----------------------\n";
			}
		}
		VirtualFree(lpModules, 0x0, MEM_RELEASE);
		lpModules = nullptr;

		std::cout << "-------------------------------------------------\n";
	}


	VirtualFree(lpProcesses, NULL, MEM_RELEASE);
	lpProcesses = nullptr;
	return 0;
}