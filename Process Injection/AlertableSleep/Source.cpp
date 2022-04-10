#include <Windows.h>
#include <iostream>

INT main(INT argc, LPSTR argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " SLEEP MILLISEC\n";
		return 0x1;
	}

	DWORD dwMilliSecs = static_cast<DWORD>(atoll(argv[1]));
	
	std::cout << GetCurrentProcessId() << " process entering alertable state for " << argv[1] << " ms\n";
	
	// TRUE is used to notify that while sleep, set this thread in alertable state
	SleepEx(dwMilliSecs, TRUE);

	return 0x0;
}