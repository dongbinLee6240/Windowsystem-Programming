#include <iostream>
#include <tchar.h>
#include <Windows.h>

using namespace std;

void _tmain()
{
	HANDLE hDevice = CreateFile
	(
		_T("\\\\.\\PhsicaDrive0"),
		GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL
	);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		cout << "*** CreateFile Error : " << GetLastError() << endl;
		return;
	}

	PARTITION_INFORMATION pi;
	DWORD dwRetCnt = 0;
	BOOL bIsOK = DeviceIoControl
	(
		hDevice,
		IOCTL_DISK_GET_PARTITION_INFO,
		NULL, 0,
		&pi, sizeof(pi), &dwRetCnt, NULL
	);
	if (bIsOK)
	{
		cout << "Volume size    ="
			<< pi.PartitionLength.QuadPart << " bytes" << endl;
	}
	else
	{
		cout << " *** DISK_GET_PARTITION_INFO Error : " << GetLastError() << endl;
	}

	DISK_GEOMETRY dg;
	dwRetCnt = 0;
	bIsOK = DeviceIoControl
	(
		hDevice,
		IOCTL_DISK_GET_DRIVE_GEOMETRY,
		NULL, 0,
		&dg, sizeof(dg), &dwRetCnt, NULL
	);

	if (bIsOK)
	{
		cout << "Cylinders       = " << dg.Cylinders.QuadPart << endl;
		cout << "Tracks/cylinder = " << dg.SectorsPerTrack << endl;
		cout << "Bytes/sector      = " << dg.BytesPerSector << endl;
	}
	else
	{
		cout << " *** DISK_GET_DRIVE_GEOMETRY Error : " << GetLastError() << endl;
	}
	CloseHandle(hDevice);
	
	cout << endl << "Press any key to exit...." << endl;
	getchar();

}