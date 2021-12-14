#pragma once
#include <Windows.h>
#include <winternl.h>

#pragma comment(lib, "Ntdll.lib")

// Signature of NtQuerySystemInformation from winternl.h
EXTERN_C NTSTATUS NTAPI NtQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID               SystemInformation,
	IN ULONG                SystemInformationLength,
	OUT PULONG              ReturnLength OPTIONAL
);
