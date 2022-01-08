#pragma once
#include <Windows.h>
#include <Shlwapi.h>
#include <winternl.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ntdll.lib")
#pragma comment(lib, "Shlwapi.lib")

// NTSTATUS return codes specifically for NtQuerySystemInformation 
constexpr unsigned int STATUS_SUCCESS = 0x0;
constexpr unsigned int STATUS_INFO_LENGTH_MISMATCH = 0x0C0000004;

// Signature of NtQuerySystemInformation from winternl.h
// Placeholders IN and OUT are defined to nothing, this is just for developers to know how the parameters will be used
EXTERN_C NTSTATUS NTAPI NtQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID               SystemInformation,
	IN ULONG                SystemInformationLength,
	OUT PULONG              ReturnLength OPTIONAL
);

template <typename T>
LPCWSTR GetHumanReadableSize(T size) {
	LPCWSTR lpSize = (LPCWSTR)VirtualAlloc(nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	
	if (StrFormatByteSizeW((LONGLONG)size, (PWSTR)lpSize, MAX_PATH) == NULL) {
		VirtualFree((LPVOID)lpSize, 0x0, MEM_RELEASE);
		lpSize = nullptr;
	}

	return lpSize;
}