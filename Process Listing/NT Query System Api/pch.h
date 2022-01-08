#pragma once
#include <Windows.h>
#include <winternl.h>

#pragma comment(lib, "Ntdll.lib")

// Signature of NtQuerySystemInformation from winternl.h
// Placeholders IN and OUT are defined to nothing, this is just for developers to know how the parameters will be used
EXTERN_C NTSTATUS NTAPI NtQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID               SystemInformation,
	IN ULONG                SystemInformationLength,
	OUT PULONG              ReturnLength OPTIONAL
);
