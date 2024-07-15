#include <iostream>
#include <tchar.h>
#include <Windows.h>
#define BUFF_SIZE 65536
using namespace std;

void _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		cout << "Uasge : FileCopySync SourceFile" << endl;
		return;
	}

	HANDLE hSrcFile = CreateFile
	(
		argv[1], GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL
	);

	if (hSrcFile == INVALID_HANDLE_VALUE)
	{
		cout << argv[1] << " open failed, code :" << GetLastError() << endl;
		return;
	}

	LARGE_INTEGER llSrcSize;
	GetFileSizeEx(hSrcFile, &llSrcSize);

	LARGE_INTEGER llDstSize;
	DWORD dwPad = (llDstSize.QuadPart % BUFF_SIZE);
	if (dwPad > 0)
		llDstSize.QuadPart = llDstSize.QuadPart + (BUFF_SIZE - dwPad);

	HANDLE hDstFile = CreateFile
	(
		argv[2], GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_FLAG_NO_BUFFERING, NULL
	);

	if (hDstFile == INVALID_HANDLE_VALUE)
	{
		cout << argv[2] << "open failed, code : " << GetLastError() << endl;
		return;
	}
	//������ �� �� ���� ������ ������ ������ ������ �̷�����Ƿ�, 
	// �ӵ� ������ ���ؼ� �̸� Ÿ�� ������ ũ�⸦ ���� ũ�� ������ ���ĵ� �ҽ� ũ���
	// �����ϰ� ����
	SetFilePointerEx(hDstFile, llDstSize, NULL, FILE_BEGIN);
	SetEndOfFile(hDstFile);
	SetFilePointer(hDstFile, 0, NULL, FILE_BEGIN);

	PBYTE pBuff = (PBYTE)VirtualAlloc(NULL, BUFF_SIZE, MEM_COMMIT, PAGE_READWRITE);

	DWORD dwErrCode = 0;
	LONGLONG llCopiedSize = 0;
	while (llCopiedSize < llDstSize.QuadPart)
	{
		DWORD dwReadyBytes = 0;
		BOOL dIsOK = ReadFile
		(
			hSrcFile, pBuff, BUFF_SIZE, &dwReadyBytes, NULL
		);

		if (!dIsOK)
		{
			dwErrCode = GetLastError();
			break;
		}

		DWORD dwWroteBytes = 0;
		dIsOK = WriteFile
		(
			hDstFile, pBuff, BUFF_SIZE, &dwWroteBytes, NULL
		);
		if (!dIsOK)
		{
			dwErrCode = GetLastError();
			break;
		}

		llCopiedSize += dwWroteBytes;
		printf("Copy bytes : %164d, %d%%\n", llCopiedSize,
			(100 * llCopiedSize) / llDstSize.QuadPart);
	}
	VirtualFree(pBuff, 0, MEM_RELEASE);

	CloseHandle(hSrcFile);

	if (dwErrCode == ERROR_SUCCESS)
	{
		hDstFile = ReOpenFile(hDstFile, GENERIC_WRITE,
			FILE_SHARE_WRITE | FILE_SHARE_READ, 0);
		SetFilePointerEx(hDstFile, llSrcSize, NULL, FILE_BEGIN);
		SetEndOfFile(hDstFile);
	}
	else
		cout << endl << "Error occurred in file copying, code=" << dwErrCode << endl;

	CloseHandle(hDstFile);

}