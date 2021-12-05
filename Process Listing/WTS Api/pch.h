#pragma once
#include <Windows.h>
#include <system_error>
#include <iostream>
#include <shellapi.h>

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
	} else if(hTok == NULL || hTok == INVALID_HANDLE_VALUE) {
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
/// Get the serialized name of the type of SID from SID_NAME_USE value
/// </summary>
/// <param name="nUse">SID type value obtained from LookupAccountSidA()</param>
/// <returns>Serialized SID type</returns>
LPCSTR GetSidType(SID_NAME_USE nUse) {
	switch (nUse) {
	case SidTypeAlias:
		return "Alias";
	case SidTypeComputer:
		return "Computer";
	case SidTypeDeletedAccount:
		return "Deleted Account";
	case SidTypeDomain:
		return "Domain";
	case SidTypeGroup:
		return "Group";
	case SidTypeInvalid:
		return "Invalid";
	case SidTypeLabel:
		return "Label";
	case SidTypeLogonSession:
		return "Logon Session";
	case SidTypeUnknown:
		return "Unknown";
	case SidTypeUser:
		return "User";
	case SidTypeWellKnownGroup:
		return "Well Known Group";
	default:
		return "N/A";
	}
}