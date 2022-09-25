#include "process_utils.h"


HANDLE GetFirstThead(DWORD dwPID) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0x0);
	HANDLE hThread = NULL;

	THREADENTRY32 te{};
	te.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hSnap, &te)) {
		CloseHandle(hSnap);
		return hThread;
	}

	do {
		if (te.th32OwnerProcessID == dwPID) {
			// SET_CONTEXT is used to change the values of the registers
			// GET_CONTEXT is used to retrieve the initial values of the registers
			// SUSPEND and RESUME are required because instruction pointer can not be changed for running thread
			hThread = OpenThread(THREAD_SET_CONTEXT | THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
			if (hThread != NULL) {
				break;
			}
		}
	} while (Thread32Next(hSnap, &te));

	CloseHandle(hSnap);
	return hThread;
}


BOOL DoInjection(HANDLE hProcess, HANDLE hThread, LPCSTR lpDllPath) {
#ifdef _WIN64
	BYTE code[] = {
		// sub rsp, 28h
		0x48, 0x83, 0xec, 0x28,
		// mov [rsp + 18], rax
		0x48, 0x89, 0x44, 0x24, 0x18,
		// mov [rsp + 10h], rcx
		0x48, 0x89, 0x4c, 0x24, 0x10,
		// mov rcx, 11111111111111111h
		0x48, 0xb9, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		// mov rax, 22222222222222222h
		0x48, 0xb8, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
		// call rax
		0xff, 0xd0,
		// mov rcx, [rsp + 10h]
		0x48, 0x8b, 0x4c, 0x24, 0x10,
		// mov rax, [rsp + 18h]
		0x48, 0x8b, 0x44, 0x24, 0x18,
		// add rsp, 28h
		0x48, 0x83, 0xc4, 0x28,
		// mov r11, 333333333333333333h
		0x49, 0xbb, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
		// jmp r11
		0x41, 0xff, 0xe3
	};
#else
	BYTE code[] = {
			0x60,
			0x68, 0x11, 0x11, 0x11, 0x11,
			0xb8, 0x22, 0x22, 0x22, 0x22,
			0xff, 0xd0,
			0x61,
			0x68, 0x33, 0x33, 0x33, 0x33,
			0xc3
	};
#endif
	if (SuspendThread(hThread) == -1) {
		return FALSE;
	}

	LPVOID lpBuffer = VirtualAllocEx(
		hProcess,
		nullptr,
		PAGE_SIZE,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READWRITE
	);
	if (lpBuffer == nullptr) {
		ResumeThread(hThread);
		return FALSE;
	}

	CONTEXT ctx{};
	ctx.ContextFlags = CONTEXT_ALL;

	if (!GetThreadContext(hThread, &ctx)) {
		ResumeThread(hThread);
		return FALSE;
	}

	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	if (hKernel32 == NULL) {
		ResumeThread(hThread);
		return FALSE;
	}

	LPVOID lpLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");
	if (lpLoadLibraryA == NULL) {
		ResumeThread(hThread);
		return FALSE;
	}

#ifdef _WIN64
	* (LPVOID*)(code + 0x10) = (LPVOID)((CHAR*)lpBuffer + (PAGE_SIZE / 2));
	*(LPVOID*)(code + 0x1a) = lpLoadLibraryA;
	*(PLONGLONG)(code + 0x34) = ctx.Rip;
#else
	* (LPVOID*)(code + 2) = (LPVOID)((CHAR*)lpBuffer + (PAGE_SIZE / 2));
	*(LPVOID*)(code + 7) = lpLoadLibraryA;
	*(PUINT)(code + 0xf) = ctx.Eip;
#endif

	if (!WriteProcessMemory(
		hProcess,
		lpBuffer,
		code,
		sizeof(code),
		nullptr
	)) {
		ResumeThread(hThread);
		return FALSE;
	}

	if (!WriteProcessMemory(
		hProcess,
		(CHAR*)lpBuffer + (PAGE_SIZE / 2),
		lpDllPath,
		strlen(lpDllPath),
		nullptr
	)) {
		ResumeThread(hThread);
		return FALSE;
	}

#ifdef _WIN64
	ctx.Rip = (ULONGLONG)lpBuffer;
#else
	ctx.Eip = (DWORD)lpBuffer;
#endif

	if (!SetThreadContext(hThread, &ctx)) {
		ResumeThread(hThread);
		return FALSE;
	}

	ResumeThread(hThread);
	return TRUE;
}
