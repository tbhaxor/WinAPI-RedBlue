#pragma once
#include "ErrorHandling.h"
#include <TlHelp32.h>
#include <memoryapi.h>

/// <summary>
/// Print the basic details of the process.
/// </summary>
/// <param name="dwPID">Process ID</param>
VOID PrintBasicProcessDetails(DWORD dwPID) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0x0);

	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(PROCESSENTRY32W);
	DWORD dwPPID, dwThreads;
	PWCHAR szExeFile = (PWCHAR)VirtualAlloc(NULL, 260, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (!Process32FirstW(hSnap, &pe)) {
		PrintError(L"Process32FirstW()");
	} else {
		do {
			if (pe.th32ProcessID == dwPID) {
				dwPPID = pe.th32ParentProcessID;
				dwThreads = pe.cntThreads;
				szExeFile = pe.szExeFile;
				break;
			}
		} while (Process32NextW(hSnap, &pe));
	}


	std::wcout << L"[+] Basic Process Details" << std::endl;
	std::wcout << L"\tProcess ID: " << dwPID << std::endl;
	std::wcout << L"\tParent Process ID: " << dwPPID << std::endl;
	std::wcout << L"\tProcess Name: " << szExeFile << std::endl;

	VirtualFree(szExeFile, 260, MEM_RELEASE);
	szExeFile = NULL;
}