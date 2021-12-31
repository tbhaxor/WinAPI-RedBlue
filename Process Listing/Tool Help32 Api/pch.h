#pragma once
#include <Windows.h>
#include <system_error>
#include <iostream>
#include <shellapi.h>
#include <TlHelp32.h>

#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Shell32.lib")

/// <summary>
/// Format the error message and print the human-readable message from GetLastError()
/// </summary>
/// <param name="Function">Name of the function</param>
/// <param name="Exit">Whether to exit the program after displaying error message of not. By default it is FALSE</param>
VOID PrintError(LPCSTR Function, BOOL Exit = FALSE) {
	// Get the latest error id
	DWORD ErrId = GetLastError();
	std::cout << "[ERR:" << ErrId << "] " << Function << ": ";

	// Pring the error message based on the response
	if (ErrId == 0) {
		std::cout << "Something went wrong";
	} else {
		std::cout << std::system_category().message(ErrId);
	}

	std::cout << std::endl;

	// If exit flag is set, quit application with error return code
	if (Exit) {
		exit(1);
	}
}

/// <summary>
/// Launch an elevated process with administator privilegesand exit the current process
/// </summary>
VOID SpawnElevatedProcess() {
	// Get current process image file path
	CHAR szFileName[MAX_PATH];
	if (!GetModuleFileNameA(nullptr, szFileName, MAX_PATH)) {
		PrintError("GetModuleFileNameA()", TRUE);
	}

	// Craft the file execution information for ShellExecuteExA function
	SHELLEXECUTEINFOA si;
	si.cbSize = sizeof(SHELLEXECUTEINFOA);
	si.fMask = SEE_MASK_DEFAULT;
	si.hwnd = nullptr;
	si.lpVerb = "runas"; // to show UAC window
	si.lpFile = szFileName;
	si.lpParameters = nullptr;
	si.lpDirectory = nullptr;
	si.nShow = SW_NORMAL;

	// Start the process with elevated UAC 
	if (!ShellExecuteExA(&si)) {
		PrintError("ShellExecuteExA()", TRUE);
	}

	// Exit the current process as it is no longer needed
	exit(1);
}

/// <summary>
/// Add "SeDebugPrivilege" to the current running process by adjusting token privileges and verify the details.
/// NOTE: The verification process is needed because AdjustTokenPrivileges function succeeds even if the privilege is not added
/// </summary>
/// <returns>TRUE if privilege is added and exists in the query result, otherwise false (also if any function is failed)</returns>
BOOL AddSeDebugPrivileges() {
	// Get the current process handle
	DWORD dwPid = GetCurrentProcessId();
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
	if (hProc == NULL || hProc == INVALID_HANDLE_VALUE) {
		PrintError("OpenProcess()");
		return FALSE;
	}

	// Get the token handle with query information and adjust privileges access
	HANDLE hTok = NULL;
	if (!OpenProcessToken(hProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hTok)) {
		PrintError("OpenProcessToken()");
		return FALSE;
	} else if (hTok == NULL || hTok == INVALID_HANDLE_VALUE) {
		PrintError("OpenProcessToken()");
		return FALSE;
	}

	// Get the value of SeDebugPrivilege from text
	LUID pDebugPriv;
	if (!LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &pDebugPriv)) {
		PrintError("LookupPrivilegeValueA()");
		return FALSE;
	}

	// Adjust token privilege 
	TOKEN_PRIVILEGES tokPrivs;
	tokPrivs.PrivilegeCount = 1;
	tokPrivs.Privileges[0].Luid = pDebugPriv;
	tokPrivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hTok, FALSE, &tokPrivs, sizeof(tokPrivs), NULL, NULL)) {
		PrintError("AdjustTokenPrivileges()");
		return FALSE;
	}

	// Query token privileges to confirm whether 
	BOOL bRes;
	PRIVILEGE_SET tokPrivSet;
	tokPrivSet.Control = PRIVILEGE_SET_ALL_NECESSARY;
	tokPrivSet.PrivilegeCount = 1;
	tokPrivSet.Privilege[0].Luid = pDebugPriv;
	if (!PrivilegeCheck(hTok, &tokPrivSet, &bRes)) {
		PrintError("PrivilegeCheck()");
		return FALSE;
	}

	CloseHandle(hProc);
	CloseHandle(hTok);
	hProc = NULL;
	hTok = NULL;

	return bRes;
}

/// <summary>
/// Snapshot all threads. Filter and print the entries matches the owner process ID = dwPID
/// </summary>
/// <param name="dwPID">Process ID for filtering threads</param>
VOID ListProcessThreads(DWORD dwPID) {
	// Create and validate snapshot
	// If the handle is invalid or null, return the function immediately
	HANDLE hThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0x0);
	if (hThread == NULL || hThread == INVALID_HANDLE_VALUE) {
		PrintError("CreateToolhelp32Snapshot()");
		return;
	}

	THREADENTRY32 te;
	te.dwSize = sizeof(THREADENTRY32);
	

	// Check whether the entry exists. If it does, print all the details of the threads
	if (!Thread32First(hThread, &te)) {
		PrintError("Thread32First()");
	} else {
		std::wcout << L"Threads: " << std::endl;

		do {
			if (te.th32OwnerProcessID == dwPID) {
				std::wcout << L"\tTID: " << te.th32ThreadID;

				// If the current thread id is same as the process id, then it is a main thread
				if (te.th32ThreadID == dwPID) {
					std::wcout << L" (Main)";
				}
				std::wcout << std::endl;

				// Open thread handle with maximun query information
				HANDLE h = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te.th32ThreadID);
				if (h == NULL || h == INVALID_HANDLE_VALUE) {
					// If the initial access failed, try opening thread handle with limited information
					// Validate the new handle value
					h = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, te.th32ThreadID);
					if (h == NULL || h == INVALID_HANDLE_VALUE) {
						PrintError("OpenThread()");
						continue;
					}
				}

				// Get the thread exit code
				DWORD dwExitCode;
				if (!GetExitCodeThread(h, &dwExitCode)) {
					PrintError("GetExitCodeThread()");
				} else if (dwExitCode == STILL_ACTIVE) {
					std::wcout << L"\tExit Status: Still Running" << std::endl;
				} else {
					std::wcout << L"\tExit Status: " << dwExitCode << std::endl;
				}

				// Get the thread priority and print the serialized type
				std::wcout << L"\tPriority: ";
				switch (GetThreadPriority(h)) {
				case THREAD_PRIORITY_ABOVE_NORMAL:
					std::wcout << L"Above Normal";
					break;
				case THREAD_PRIORITY_BELOW_NORMAL:
					std::wcout << L"Below Normal";
					break;
				case THREAD_PRIORITY_ERROR_RETURN:
					std::wcout << L"Error";
					break;
				case THREAD_PRIORITY_HIGHEST:
					std::wcout << L"Highest";
					break;
				case THREAD_PRIORITY_IDLE:
					std::wcout << L"Idle";
					break;
				case THREAD_PRIORITY_LOWEST:
					std::wcout << L"Lowest";
					break;
				case THREAD_PRIORITY_NORMAL:
					std::wcout << L"Normal";
					break;
				case THREAD_PRIORITY_TIME_CRITICAL:
					std::wcout << L"Time Critical";
					break;
				default:
					std::wcout << L"N/A";
				}

				std::wcout << "\n\t-------------------------------------------------------------\n";

			}
		} while (Thread32Next(hThread, &te));
	}

	// Close handle and release memory
	CloseHandle(hThread);
	hThread = NULL;
}

/// <summary>
/// Create snapshot of all the modules for the process by its ID
/// </summary>
/// <param name="dwPID">Process ID for taking snap shot of the processes</param>
VOID ListProcessModules(DWORD dwPID) {
	// Create and validate snapshot
	// If the handle is invalid or null, return the function immediately
	HANDLE hModules = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwPID);
	if (hModules == NULL || hModules == INVALID_HANDLE_VALUE) {
		PrintError("CreateToolhelp32Snapshot()");
		return;
	}

	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);
	
	// Check whether the entry exists. If it does, print all the details of the modules
	if (!Module32First(hModules, &me)) {
		PrintError("Module32First()");
	} else {
		std::wcout << L"Modules: " << std::endl;

		do {
			std::wcout << L"\tMID: " << me.th32ModuleID << L"\tName: " << me.szModule << "\tExecutable: " << me.szExePath << std::endl;
			std::wcout << L"\t# Global Reference: " << me.GlblcntUsage << "\t# Process Reference" << me.ProccntUsage << std::endl;
			std::wcout << L"\tBase Address: " << std::hex << me.modBaseAddr << "\tBase Size: " << me.modBaseSize << std::endl;
			std::wcout << "\t-------------------------------------------------------------\n";
		} while (Module32Next(hModules, &me));
	}

	// Close the snapshot handle and release memory
	CloseHandle(hModules);
	hModules = NULL;
}