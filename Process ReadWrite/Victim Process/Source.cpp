#include <Windows.h>
#include <iostream>

INT main(DWORD argc, PCHAR argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <text>" << std::endl;
		return 0x1;
	}
	SIZE_T txtLen = strlen(argv[1]);

	std::cout << "Process ID: " << GetCurrentProcessId() << std::endl;
	
	LPSTR lpBuff = (LPSTR)VirtualAlloc(nullptr, txtLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	memcpy(lpBuff, argv[1], txtLen);

	std::cout << "=====================================\n";
	std::cout << "       Variables before Tamper       \n";
	std::cout << "=====================================\n\n";
	std::cout << "Argument address: 0x" << std::hex << (PVOID)argv[1] << "\t\t\tArgument Value: " << argv[1] << std::endl;
	std::cout << "Buffer address:   0x" << std::hex << (PVOID)lpBuff << "\t\t\tBuffer Value: " << lpBuff << std::endl;
	std::cout << "Length of Chars: " << std::dec << txtLen << std::endl;

	system("pause");

	std::cout << "=====================================\n";
	std::cout << "       Variables after Tamper        \n";
	std::cout << "=====================================\n\n";
	std::cout << "Argument address: 0x" << std::hex << (PVOID)argv[1] << "\t\t\tArgument Value: " << argv[1] << std::endl;
	std::cout << "Buffer address: 0x" << std::hex << (PVOID)lpBuff << "\t\t\tBuffer Value: " << lpBuff << std::endl;
}