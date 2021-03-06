#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

/// <summary>
/// Simple thread function to call the sleepex
/// </summary>
VOID HelloThread(VOID) {
	_tprintf(_T("[%d] Thread is going to sleep for 5000 millisecs\n"), GetCurrentThreadId());

	// SleepEx will either be completed when a single APC is executed or it gets timed out
	if (SleepEx(5000, TRUE) != WAIT_IO_COMPLETION) {
		_tprintf(_T("[%d] Timed outs\n"), GetLastError());
	}

	_tprintf(_T("[%d] Resuming thread\n"), GetCurrentThreadId());

}

/// <summary>
/// Callback function that will be run from the thread apc queue
/// </summary>
/// <param name="dThread">Parameter to hold thread of the APC</param>
VOID Callback(ULONG_PTR dThread) {
	_tprintf(_T("[%d APC] Callback invoked!\n"), (DWORD)dThread);
}

INT _tmain(INT argc, TCHAR argv[]) {

	// create a thread with HelloThread routine
	DWORD dThreadId;
	HANDLE hThread = CreateThread(NULL, 0x0, (LPTHREAD_START_ROUTINE)HelloThread, NULL, 0x0, &dThreadId);
	if (hThread == NULL) {
		_tprintf(_T("[%d] Thread creation failed!\n"), GetLastError());
		return 0x1;
	}

	_tprintf(_T("Continue main function, about add callback to apc queue!\n"));

	Sleep(4999); // increase time (same as sleep of the thread) and the thread sleepex will timeout

	// Add the callback apc to the hThread
	if (!QueueUserAPC((PAPCFUNC)Callback, hThread, (ULONG_PTR)dThreadId)) {
		_tprintf(_T("[%d] APC not called\n"), GetLastError());
		CloseHandle(hThread);
		return 0x1;
	}


	// Free resources and exit
	CloseHandle(hThread);
	hThread = NULL;

	return 0x0;
}