//������ �.�. 5307
//����������-��������
//����� ������� �������� ������ = 15
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <math.h>
#include <fstream>
#include <string>

#pragma comment(lib, "Winmm.lib")

const unsigned short PageNumber = 15;

void main(void)
{
	char* chProjFile = "E:\\Temp\\new.txt";
	char* chMappingObject = "My_mapping_object";
	char* chSemaphoreWriters = "Semaphore_For_Writers";
	char* chMutexWriters[PageNumber] = {"WMutex A", "WMutex B", "WMutex C", "WMutex D", "WMutex E", "WMutex F", "WMutex G", "WMutex H", "WMutex I", "WMutex J", "WMutex K", "WMutex L", "WMutex M", "WMutex N", "WMutex O"};
	char* chMutexBuzy[PageNumber] = { "BMutex A", "BMutex B", "BMutex C", "BMutex D", "BMutex E", "BMutex F", "BMutex G", "BMutex H", "BMutex I", "BMutex J", "BMutex K", "BMutex L", "BMutex M", "BMutex N", "BMutex O" };

	setlocale(LC_ALL, "Russian");

	//��������� ����������� �������� ��������
	DWORD PID = GetCurrentProcessId();

	//������ ���������� ����
	std::string Path = "E:\\Temp\\WriterLog";

	char* chNumber = new char[20];
	chNumber = itoa(PID, chNumber, 10);
	std::string App1 = std::string(chNumber);
	Path = Path.append(App1);

	std::string App2 = ".txt";	
	Path = Path.append(App2);

	std::ofstream OutputFile;
	OutputFile.open(Path, std::ios::app);

	//�������� ������������� �����
	HANDLE hdl = CreateFileA(chProjFile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (hdl == INVALID_HANDLE_VALUE)
	{
		std::cout << "������ ��� �������� ������������� �����." << std::endl;
		std::cin.clear();
		std::cin.sync();
		getch();
		return;
	};
	std::cout << "�������� ����������: " << hdl << std::endl << std::endl;
	OutputFile << timeGetTime() << " : PID - " << PID << " : �������� ����������: " << hdl << std::endl << std::endl;

	//��������� ������� �������� �������� ������
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	DWORD PageSize = info.dwPageSize;

	//�������� �������� �����
	HANDLE hdlmap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, chMappingObject);

	if (hdlmap == NULL)
	{
		hdlmap = CreateFileMappingA(hdl, NULL, PAGE_READWRITE, NULL, PageNumber*PageSize, chMappingObject);
		if (hdlmap == NULL)
		{
			std::cout << "������ ��� �������� ������������� �����." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};
	};
	std::cout << "���������� ������� �������� �����: " << hdlmap << std::endl << std::endl;
	OutputFile << timeGetTime() << " : PID - " << PID << " : ���������� ������� �������� �����: " << hdlmap << std::endl << std::endl;

	//������������� ��������� ����� � ������
	LPVOID MapViewAddr;
	if ((MapViewAddr = MapViewOfFile(hdlmap, FILE_MAP_ALL_ACCESS, NULL, NULL, NULL)) == NULL)
	{
		std::cout << "������ ��� ������������� ��������� ����� � ������." << std::endl;
		std::cin.clear();
		std::cin.sync();
		getch();
		return;
	};
	std::cout << "������� ����� ������� �������������: " << MapViewAddr << std::endl << std::endl;
	OutputFile << timeGetTime() << " : PID - " << PID << " : ������� ����� ������� �������������: " << MapViewAddr << std::endl << std::endl;

	//��������� �������� �������� ������
	if (!VirtualLock(MapViewAddr, PageNumber*PageSize))
	{
		std::cout << "������ ��� ������������ ������� �������� ������." << std::endl;
		std::cin.clear();
		std::cin.sync();
		getch();
		return;
	};

	//����� ����� ��������
	std::cout << "������� ����� �������, ����� ����������." << std::endl;
	getch();

	//������ ��� ��������� �������
	HANDLE SemaphoreForWriters = CreateSemaphore(NULL, PageNumber, PageNumber, chSemaphoreWriters);

	if (SemaphoreForWriters == NULL)
	{
		std::cout << "������ ��� �������� ��� �������� �������� ��� ���������." << std::endl;
		std::cin.clear();
		std::cin.sync();
		getch();
		return;
	};

	std::cout << "���������� �������� ��� ���������: " << SemaphoreForWriters << std::endl;
	OutputFile << timeGetTime() << " : PID - " << PID << " : ���������� �������� ��� ���������: " << SemaphoreForWriters << std::endl;

	//������ ��������� ��� ������ (�� �������� �� �������� ������)
	HANDLE MutexWriters[PageNumber];

	for (unsigned short i = 0; i < PageNumber; i++)
	{
		MutexWriters[i] = CreateMutex(NULL, FALSE, chMutexWriters[i]);

		if (MutexWriters[i] == NULL)
		{
			std::cout << "������ ��� �������� ��� �������� �������� ��� ��������." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		std::cout << "���������� �������� ��� ���������: " << MutexWriters[i] << " ��� " << i << "-� �������� ������." << std::endl;
		OutputFile << timeGetTime() << " : PID - " << PID << " : ���������� �������� ��� ���������: " << MutexWriters[i] << " ��� " << i << "-� �������� ������." << std::endl;
	};

	std::cout << std::endl;

	//������ ��������� ��������� �������
	HANDLE MutexBuzy[PageNumber];

	for (unsigned short i = 0; i < PageNumber; i++)
	{
		MutexBuzy[i] = CreateMutex(NULL, FALSE, chMutexBuzy[i]);

		if (MutexBuzy[i] == NULL)
		{
			std::cout << "������ ��� �������� ��� �������� �������� ���������." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		std::cout << "���������� �������� ���������: " << MutexBuzy[i] << " ��� " << i << "-� �������� ������." << std::endl;
		OutputFile << timeGetTime() << " : PID - " << PID << " : ���������� �������� ���������: " << MutexBuzy[i] << " ��� " << i << "-� �������� ������." << std::endl;
	};

	std::cout << std::endl;

	while (true)
	{
		OutputFile << timeGetTime() << " : PID - " << PID << " : ��������, ����� ����� ��������� ������ ������ 15." << std::endl;

		//���, ����� �������� �������� �������� ����� ������ 0
		if (WaitForSingleObject(SemaphoreForWriters, INFINITE) == WAIT_FAILED)
		{
			std::cout << "������ ��� �������� �������� ��� ���������." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		OutputFile << timeGetTime() << " : PID - " << PID << " : ��������, ����� �������� ������� ������, ���� ����� ������." << std::endl;

		//���, ���� �������� ������� ������, ���� ����� ������
		int WritePage = 0;
		if ((WritePage = WaitForMultipleObjects(PageNumber, MutexWriters, FALSE, INFINITE)) == WAIT_FAILED)
		{
			std::cout << "������ ��� �������� �������� ��� ��������." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		//����������, � ����� �������� ����� ������
		WritePage -= WAIT_OBJECT_0;

		//����� ������������ ������ �� 0,5 �� 1,5 ���
		unsigned int WriteTime = 500 + rand() % 1000;

		//�������� ����� ������ �������, ���� ����� ������
		char* WriteRegion = (char*)MapViewAddr + (WritePage*PageSize);

		//������� ���� ��������
		unsigned int ByteCount = 0;

		OutputFile << timeGetTime() << " : PID - " << PID << " : ��������, ����� ���������� �������� �������� ������." << std::endl;

		//���, ����� ���������� �������� ��������� ��������
		if (WaitForSingleObject(MutexBuzy[WritePage], INFINITE) == WAIT_FAILED)
		{
			std::cout << "������ ��� �������� �������� ���������." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};		

		OutputFile << timeGetTime() << " : PID - " << PID << " : ������ � �������� �" << WritePage << "." << std::endl;

		//������ ������
		unsigned int StartTime = timeGetTime();

		//����� �������� �����
		while ((timeGetTime() - StartTime) <= WriteTime)
		{
			if (ByteCount < PageSize)
			{
				//����� ��������� �������
				*WriteRegion = (char)(65 + rand() % 20);
				WriteRegion++;
				ByteCount++;
			}
			else
			{
				ByteCount = 0;
				WriteRegion = (char*)MapViewAddr + (WritePage*PageSize);
			};
		};

		OutputFile << timeGetTime() << " : PID - " << PID << " : ������� � ������������." << std::endl;

		//����������� ������� ���������
		if (!ReleaseMutex(MutexBuzy[WritePage]))
		{
			std::cout << "������ ��� ������������ �������� ���������." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		//����������� ������� ��� ���������
		if (!ReleaseMutex(MutexWriters[WritePage]))
		{
			std::cout << "������ ��� ������������ �������� ��� ���������." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		//����������� �������
		if (!ReleaseSemaphore(SemaphoreForWriters, 1, NULL))
		{
			std::cout << "������ ��� ������������ �������� ��� ���������." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};
	};

	getch();
}