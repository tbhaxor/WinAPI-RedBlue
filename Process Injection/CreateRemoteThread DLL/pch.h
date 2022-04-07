#pragma once

#include <Windows.h>
#include <system_error>
#include <iostream>

/// <summary>
/// Print the human-readable error message cause while execution of the function and exit if TRUE
/// </summary>
/// <param name="lpFunction">Function name caused error</param>
/// <param name="bExit">Whether to exit after printing error or not (TRUE/FALSE)</param>
VOID PrintError(LPCSTR lpFunction, BOOL bExit = FALSE) {
	DWORD dwErrorCode = GetLastError();

	std::cout << "[" << dwErrorCode << "] " << lpFunction << ": ";
	if (dwErrorCode == 0x0) {
		std::cout << "Undefined error\n";
	} else {
		std::cout << std::system_category().message(dwErrorCode) << std::endl;
	}

	if (bExit) {
		ExitProcess(1);
	}
}