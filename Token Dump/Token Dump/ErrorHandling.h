#pragma once
#include <Windows.h>
#include <errhandlingapi.h>
#include <iostream>
#include <system_error>

/// <summary>
/// Format the error message and print the human-readable message from GetLastError()
/// </summary>
/// <param name="lpFunctionName">Name of the function</param>
/// <param name="bExit">Whether to exit the program after displaying error message of not. By default it is FALSE</param>
VOID PrintError(LPCWSTR lpFunctionName, BOOL bExit = FALSE) {
	// Get the latest error id
	DWORD ErrId = GetLastError();
	std::wcout << L"[ERR:" << ErrId << L"] " << lpFunctionName  << L": ";

	// Pring the error message based on the response
	if (ErrId == 0) {
		std::wcout << L"Something went wrong";
	} else {
		std::cout << std::system_category().message(ErrId);
	}

	std::wcout << std::endl;

	// If exit flag is set, quit application with error return code
	if (bExit) {
		exit(1);
	}
}