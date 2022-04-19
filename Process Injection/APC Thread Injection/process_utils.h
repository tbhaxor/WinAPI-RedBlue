#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <stack>

typedef std::stack<HANDLE*> THREAD_STACK;
typedef THREAD_STACK* PTHREAD_STACK;
typedef THREAD_STACK* LPTHREAD_STACK;

LPTHREAD_STACK GetProcessThreads(DWORD dwPID) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0x0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return nullptr;
	}

	THREADENTRY32 te;
	te.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hSnapshot, &te)) {
		CloseHandle(hSnapshot);
		return nullptr;
	}

	HANDLE hThread = NULL;

	

	LPTHREAD_STACK lpThreads = new THREAD_STACK();
	do {
		if (te.th32OwnerProcessID == dwPID) {
			hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, te.th32ThreadID);
			if (hThread != NULL) {
				lpThreads->push(&hThread);
			}
		}
	} while (Thread32Next(hSnapshot, &te));

	return lpThreads;
}
