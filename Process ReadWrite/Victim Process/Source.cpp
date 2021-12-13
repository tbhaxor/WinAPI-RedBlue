#include <Windows.h>
#include <stdio.h>

INT main(VOID) {
	char* ch = (char*)malloc(20);

	// Print process details and base address of the data
	printf("Process ID: %d\n", GetCurrentProcessId());
	printf("Enter text: ");
	scanf_s("%19s", ch, 20);
	printf("Base Address: 0x%p\n\n", ch);
	
	// Print the text before and after tamper
	printf("Text Before Tamper: %s\n", ch);
	system("pause");
	printf("Text After Tamper: %s\n", ch);
}