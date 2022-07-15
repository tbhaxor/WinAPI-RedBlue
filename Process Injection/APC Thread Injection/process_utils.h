#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <stack>

// custom typings
typedef std::stack<HANDLE*> THREAD_STACK;
typedef THREAD_STACK* PTHREAD_STACK;

/// <summary>
/// Get a vector of handles of the threads of the process using CreateToolhelp32Snapshot
/// </summary>
/// <param name="dwPID">PID of the onwer process</param>
/// <returns></returns>
PTHREAD_STACK GetProcessThreads(DWORD dwPID) {
	// create snapshot of all the threads
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0x0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return nullptr;
	}

	// init thread entry struct and get the first entry
	THREADENTRY32 te;
	te.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hSnapshot, &te)) {
		CloseHandle(hSnapshot);
		return nullptr;
	}

	HANDLE hThread = NULL;

	PTHREAD_STACK lpThreads = new THREAD_STACK();
	do {
		// if the thread owner id is of the process id provided in cli args
		// open thread handle and push ot the vector
		if (te.th32OwnerProcessID == dwPID) {
			hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, te.th32ThreadID);
			if (hThread != NULL) {
				lpThreads->push(&hThread);
			}
		}
	} while (Thread32Next(hSnapshot, &te));

	return lpThreads;
}
