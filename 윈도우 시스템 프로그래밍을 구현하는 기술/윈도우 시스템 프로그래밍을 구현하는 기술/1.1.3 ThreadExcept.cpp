#include <iostream>
#include <windows.h>

using namespace std;

//���ܿ� ���� �� ������ �����. ���⿡���� �ܼ��� �ֿܼ� ���
LONG ExpFilter(EXCEPTION_POINTERS* pEx)
{
	PEXCEPTION_RECORD pER = pEx->ExceptionRecord;
	char szOut[512];

	int nLen = sprintf(szOut, "Code = %x, Address=&p",
		pER->ExceptionCode, pER->ExceptionAddress);

	if (pER->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		sprintf(szOut + nLen, "\nAttempt to %s data at address &p",
			pER->ExceptionInformation[0] ? "write" : "read",
			pER->ExceptionInformation[1]);
	}

	cout << szOut << endl;

	return EXCEPTION_EXECUTE_HANDLER;
}
int main()
{

}