#include <stdio.h>

#include "utils.hpp"

void main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("\tUsage : LoadLibraryInjector.exe [EXE_NAME] [DLL_PATH]\n");
		return;
	}


	uint64_t PID = get_pid_by_name(argv[1]);
	if (PID)
		printf("[+] Found process %s with pid %d\n", argv[1], PID);
	else
		{printf("[-] Failed to find process %s pid \n", argv[1]); return;};
	

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess)
		printf("[+] Opened handle 0x%X to process\n", hProcess);
	else
		{printf("[-] Failed to open handle to process\n"); return;};


	void* allocated_memory = VirtualAllocEx(hProcess, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (allocated_memory)
		printf("[+] Allocated memory in remote process at 0x%p\n", allocated_memory);
	else
		{printf("[-] Failed to allocate memory in remote process\n"); return;};


	BOOL bWSuccess = WriteProcessMemory(hProcess, allocated_memory, &(*argv[2]), MAX_PATH, 0);
	if (bWSuccess)
		printf("[+] Wrote dll path in remote process\n");
	else
		{printf("[-] Failed to write dll path in remote process\n"); return;}


	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocated_memory, 0, 0);
	if (hThread)
		printf("[+] Created thread with handle 0x%X \n", hThread);
	else
		{printf("[-] Failed to create remote thread\n"); return;};

	WaitForSingleObject(hThread, INFINITE);

	BOOL bVSuccess = VirtualFreeEx(hProcess, allocated_memory, MAX_PATH, MEM_RELEASE);
	if (bVSuccess)
		printf("[+] Freeing allocated memory\n");
	else
		printf("[-] Failed to free memory\n"); 

	CloseHandle(hProcess);
	return;

	

}
