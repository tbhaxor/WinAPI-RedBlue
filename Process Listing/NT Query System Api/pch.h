#pragma once
#include <Windows.h>
#include <winternl.h>
#include <Shlwapi.h>
#include <iostream>

#pragma comment(lib, "Ntdll.lib")
#pragma comment(lib, "Shlwapi.lib")

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