#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

constexpr SIZE_T PAGE_SIZE = 1 << 12;

/// <summary>
/// Get the first accesible thread for the process
/// </summary>
/// <param name="dwPID">Process ID</param>
/// <returns>Valid thread handle or NULL</returns>
HANDLE GetFirstThead(DWORD dwPID);

/// <summary>
/// </summary>
/// <param name="hProcess"></param>
/// <param name="hThread"></param>
/// <param name="pDllPath"></param>
/// <returns>TRUE if the injection is successful</returns>

BOOL DoInjection(HANDLE hProcess, HANDLE hThread, LPCSTR lpDllPath);