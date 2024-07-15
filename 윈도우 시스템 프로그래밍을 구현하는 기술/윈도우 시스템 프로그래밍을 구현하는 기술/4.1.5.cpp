#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <tchar.h>
#include <Windows.h>

using namespace std;

void _tmain()
{
	HANDLE hStdOut = CreateFile(_T("CONOUT$"),
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL
	);

	HANDLE hStdIn= CreateFile(_T("CONIN$"),
		GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

	char szIn[256], szOut[256];
	while (true)
	{
		DWORD dwTrBytes = 0;
		ReadFile(hStdIn, szIn, sizeof(szIn), &dwTrBytes, NULL);

		szIn[dwTrBytes] = 0;
		if (_stricmp(szIn, "quit\r\n") == 0)
			break;

		int len = sprintf(szOut, "<== Echo : %s", szIn);
		WriteFile(hStdOut, szOut, (DWORD)len, &dwTrBytes, NULL);
	}

	CloseHandle(hStdIn);
	CloseHandle(hStdOut);
}