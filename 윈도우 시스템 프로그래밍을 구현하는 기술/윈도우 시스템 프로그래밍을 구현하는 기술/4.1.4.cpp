#include <iostream>
#include <Windows.h>
#include <tchar.h>

using namespace std;

#define Share_File _T("C:\\temp\\LockFileTest.txt")
#define Line_Len 40

DWORD WINAPI WriteProc(PVOID pParam)
{
	for (int j = 0; j < 26; j++)
	{
		char ch = j + 'a';
		HANDLE hFile = CreateFile
		(
			Share_File, GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_ALWAYS, 0, NULL
		);

		OVERLAPPED ov;
		memset(&ov, 0, sizeof(ov));
		ov.Offset = j + Line_Len;
		SetFilePointer(hFile, ov.Offset, NULL, FILE_BEGIN);

		LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, Line_Len, 0, &ov);
		{
			DWORD dwBytesWrote;
			for (DWORD i = 0; i < Line_Len; i++)
			{
				WriteFile(hFile, &ch, 1, &dwBytesWrote, NULL);
				FlushFileBuffers(hFile);
			}
		}
		UnlockFileEx(hFile, 0, Line_Len, 0, &ov);

		CloseHandle(hFile);
	}
	return 0;
}

DWORD WINAPI ReadProc(PVOID pParam)
{
	char szRead[83];
	for (int j = 0; j < 26; j++)
	{
		Sleep(10);
		HANDLE hFile = CreateFile
		(
			Share_File, GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_ALWAYS, 0, NULL
		);

		OVERLAPPED ov;
		memset(&ov, 0, sizeof(ov));
		ov.Offset = j * Line_Len;
		SetFilePointer(hFile, ov.Offset, NULL, FILE_BEGIN);

		LockFileEx(hFile, 0, 0, Line_Len, 0, &ov);
		{
			char ch = '?';
			DWORD dwBytesRead, pos = 0;
			for (; pos < Line_Len; pos++)
			{
				ReadFile(hFile, &ch, 1, &dwBytesRead, NULL);
				szRead[pos] = ch;
			}
			szRead[pos] = 0;
		}
		UnlockFileEx(hFile, 0, Line_Len, 0, &ov);

		CloseHandle(hFile);
		printf("%s\n", szRead);
	}
	return 0;
}

#define Thread_cnt 2
void _tmain(void)
{
	HANDLE hFile = CreateFile
	(
		Share_File, GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, 0, NULL
	);
	for (int i = 0; i < Line_Len; i++)
	{
		char ch = '-';
		DWORD dwBytesWrote;
		WriteFile(hFile, &ch, 1, &dwBytesWrote, NULL);
	}
	CloseHandle(hFile);

	HANDLE arhThreads[Thread_cnt];

	for (int i = 0; i < Thread_cnt; i++)
	{
		LPTHREAD_START_ROUTINE pfnThr = ((i == 0) ? WriteProc : ReadProc);
		DWORD dwThrId = 0;
		arhThreads[i] = CreateThread(NULL, 0, pfnThr, NULL, 0, &dwThrId);
	}

	getchar();
	WaitForMultipleObjects(Thread_cnt, arhThreads, TRUE, INFINITE);
	for (int i = 0; i < Thread_cnt; i++)
	{
		CloseHandle(arhThreads[i]);
	}
}