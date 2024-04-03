#pragma warning(disable:4996)
#include <iostream>
#include <windows.h>

using namespace std;

//���ܿ� ���� �� ������ �����. ���⿡���� �ܼ��� �ֿܼ� ���
LONG ExpFilter(EXCEPTION_POINTERS* pEx)  //Filter
{
	PEXCEPTION_RECORD pER = pEx->ExceptionRecord;
	char szOut[512];

	int nLen = printf(szOut, "Code = %x, Address=&p",
		pER->ExceptionCode, pER->ExceptionAddress);

	if (pER->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		printf(szOut + nLen, "\nAttempt to %s data at address &p",
			pER->ExceptionInformation[0] ? "write" : "read",
			pER->ExceptionInformation[1]);
	}

	cout << szOut << endl;

	return EXCEPTION_EXECUTE_HANDLER;
}

DWORD WINAPI ThreadProc(PVOID pParam)  //������ �Լ�
{
	PCSTR pszTest = (PCSTR)pParam;

	PINT pnVal = NULL;  //�ǵ������� ���ܸ� �߻���Ű�� ���� NULL�� �ʱ�ȭ�� ������ ���� ����

	PSTR pBuff = new char[strlen(pszTest) + 1];  //�޸𸮸� �Ҵ��ϰ� �Ű������� ���� ���� ���ڿ��� �����Ѵ�. ������ ���� �ÿ� �����Ǿ�� �� ���ҽ���. 
	strcpy(pBuff, pszTest);  //�Ҵ�� �޸𸮸� ���� �߻��ÿ��� ���� �������� �����ֱ� ���� �߰� �ڵ�.
	                         

	__try
	{
		cout << ">>>>> Thread " << GetCurrentThreadId() << endl;
		cout << "  copied data: " << pBuff << endl;

		*pnVal = strlen(pszTest); //�� �������� �ǵ������� ���ܸ� �߻���Ų��. NULL�� �ʱ�ȭ�� ������ ������ ���� �����Ѵ�.

		delete[] pBuff;
	}

	__except (ExpFilter(GetExceptionInformation()))  //���ܸ� ��� �� ������ �м��ϱ� ���� GetExceptionInformation ��ũ�θ� ExpFilter �Ű������� �Ѱ��ش�.
	{
		DWORD dwExCode = GetExceptionCode();
		printf("==== Exception occurred in thread &d, code= %08x\n",
			GetCurrentThreadId(), dwExCode);

		delete[] pBuff;  //�Ҵ��ߴ� �޸𸮸� �����Ѵ�. ���� ó���� ���� ��� �޸𸮸� ������ ��ȸ�� ���� ���Ѵ�.

		return dwExCode;  //���� �ڵ带 ������ ���� �ڵ�� �����Ѵ�.
	}
	
	return 0;

}
void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;

	PCSTR pszTest = "Thread Test...";
	DWORD dwThrID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, (PVOID)pszTest, 0, &dwThrID);

	getchar();
	WaitForSingleObject(hThread, INFINITE);

	DWORD dwExitCode = 0;
	GetExitCodeThread(hThread, &dwExitCode);
	printf("...Sub thread %d terminated with ExitCode 0x%08X\n",
		dwThrID, dwExitCode);

	CloseHandle(hThread);  //���ܰ� �߻������� �����忡�� ���ܸ� ó���ϰ� �����尡 ���������� ����Ǿ�����, ���� �ڵ带 ���� ���� �����忡�� ������ ������ �ĺ�
	                       //���� �Լ��� ������ ���� ����
}