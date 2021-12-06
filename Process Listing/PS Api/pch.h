#pragma once
#include <Windows.h>
#include <system_error>
#include <iostream>
#include <shellapi.h>

#pragma	comment(lib, "Shell32.lib")

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
/// Launch an elevated process with administator privilegesand exit the current process
/// </summary>
VOID SpawnElevatedProcess() {
	// Get current process image file path
	CHAR szFileName[MAX_PATH];
	if (!GetModuleFileNameA(NULL, szFileName, MAX_PATH)) {
		PrintError("GetModuleFileNameA()", TRUE);
	}

	// Craft the file execution information for ShellExecuteExA function
	SHELLEXECUTEINFOA si;
	si.cbSize = sizeof(SHELLEXECUTEINFOA);
	si.fMask = SEE_MASK_DEFAULT;
	si.hwnd = NULL;
	si.lpVerb = "runas"; // to show UAC window
	si.lpFile = szFileName;
	si.lpParameters = NULL;
	si.lpDirectory = NULL;
	si.nShow = SW_NORMAL;

	// Start the process with elevated UAC 
	if (!ShellExecuteExA(&si)) {
		PrintError("ShellExecuteExA()", TRUE);
	}

	// Exit the current process as it is no longer needed
	exit(1);
}

/// <summary>
/// Print the minimum possible details of the process
/// </summary>
/// <param name="pid">Process PID from EnumProcesses()</param>
VOID PrintProcessDetails(DWORD pid) {
	// Ignore if system idle process
	if (pid == 0) {
		return;
	}
	std::cout << "PID: " << pid << std::endl;
	
	// Open the proccess handle with full query information
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (hProc == NULL || hProc == INVALID_HANDLE_VALUE) {
		// Open the process handle with limited query information if the previous one is failed
		hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
		// Validate the process handle
		if (hProc == NULL || hProc == INVALID_HANDLE_VALUE) {
			PrintError("OpenProcess()");
			std::cout << "--------------------------------------------------------------------" << std::endl;
			return;
		}
	}

	
	BOOL bJob;
	DWORD dwSize = MAX_PATH, dwExitCode;
	CHAR szImageName[MAX_PATH];

	// Determine whether the current process is in a job or not
	if (!IsProcessInJob(hProc, NULL, &bJob)) {
		PrintError("IsProcessInJob()");
	} else {
		std::string s(bJob ? "Yes" : "No");
		std::cout << "Is Process in Job: " << s << std::endl;
	}
	
	// Get the image name of the process
	if (!QueryFullProcessImageNameA(hProc, PROCESS_NAME_NATIVE, szImageName, &dwSize)) {
		PrintError("GetExitCodeProcess()");
	} else {
		std::cout << "Process Image Name: " << szImageName << std::endl;
	}

	// Get the process exit status from its handle
	if (!GetExitCodeProcess(hProc, &dwExitCode)) {
		PrintError("GetExitCodeProcess()");
	} else if (dwExitCode == STILL_ACTIVE) {
		// If the exit code is STILL_ACTIVE, the process is currently running
		std::cout << "Exit Code: " << "Still Active" << std::endl;
	} else {
		std::cout << "Exit Code: " << dwExitCode << std::endl;
	}
	
	// Close handle and release resources
	CloseHandle(hProc);
	hProc = NULL;

	std::cout << "--------------------------------------------------------------------" << std::endl;
}