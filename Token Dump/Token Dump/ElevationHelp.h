#pragma once
#include "ErrorHandling.h"
#include <Windows.h>
#include <string>

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
		PrintError(L"OpenProcess()");
		return FALSE;
	}

	// Get the token handle with query information and adjust privileges access
	HANDLE hTok = NULL;
	if (!OpenProcessToken(hProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hTok)) {
		PrintError(L"OpenProcessToken()");
		return FALSE;
	} else if (hTok == NULL || hTok == INVALID_HANDLE_VALUE) {
		PrintError(L"OpenProcessToken()");
		return FALSE;
	}

	// Get the value of SeDebugPrivilege from text
	LUID pDebugPriv;
	if (!LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &pDebugPriv)) {
		PrintError(L"LookupPrivilegeValueA()");
		return FALSE;
	}

	// Adjust token privilege 
	TOKEN_PRIVILEGES tokPrivs;
	tokPrivs.PrivilegeCount = 1;
	tokPrivs.Privileges[0].Luid = pDebugPriv;
	tokPrivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hTok, FALSE, &tokPrivs, sizeof(tokPrivs), NULL, NULL)) {
		PrintError(L"AdjustTokenPrivileges()");
		return FALSE;
	}

	// Query token privileges to confirm whether 
	BOOL bRes;
	PRIVILEGE_SET tokPrivSet;
	tokPrivSet.Control = PRIVILEGE_SET_ALL_NECESSARY;
	tokPrivSet.PrivilegeCount = 1;
	tokPrivSet.Privilege[0].Luid = pDebugPriv;
	if (!PrivilegeCheck(hTok, &tokPrivSet, &bRes)) {
		PrintError(L"PrivilegeCheck()");
		return FALSE;
	}

	CloseHandle(hProc);
	CloseHandle(hTok);
	hProc = NULL;
	hTok = NULL;

	return bRes;
}

/// <summary>
/// Launch an elevated process with administator privilegesand exit the current 
/// </summary>
/// <param name="dwPID">Process ID of the application to parse in elevated prompt</param>
VOID SpawnElevatedProcess(DWORD dwPID) {
	// Get current process image file path
	WCHAR szFileName[MAX_PATH];
	if (!GetModuleFileNameW(NULL, szFileName, MAX_PATH)) {
		PrintError(L"GetModuleFileNameW()", TRUE);
	}

	// Craft the file execution information for ShellExecuteExA function
	SHELLEXECUTEINFOW si;
	si.cbSize = sizeof(SHELLEXECUTEINFOW);
	si.fMask = SEE_MASK_DEFAULT;
	si.hwnd = NULL;
	si.lpVerb = L"runas"; // to show UAC window
	si.lpFile = szFileName;
	si.lpParameters = { std::to_wstring(dwPID).c_str() };
	si.lpDirectory = NULL;
	si.nShow = SW_NORMAL;

	// Start the process with elevated UAC 
	if (!ShellExecuteExW(&si)) {
		PrintError(L"ShellExecuteExW()", TRUE);
	}

	// Exit the current process as it is no longer needed
	exit(1);
}