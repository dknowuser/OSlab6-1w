//Марчук Л.Б. 5307
//Приложение-писатель
//Число страниц буферной памяти = 15
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

	//Получение дескриптора текущего процесса
	DWORD PID = GetCurrentProcessId();

	//Создаём журнальный файл
	std::string Path = "E:\\Temp\\WriterLog";

	char* chNumber = new char[20];
	chNumber = itoa(PID, chNumber, 10);
	std::string App1 = std::string(chNumber);
	Path = Path.append(App1);

	std::string App2 = ".txt";	
	Path = Path.append(App2);

	std::ofstream OutputFile;
	OutputFile.open(Path, std::ios::app);

	//Создание проецируемого файла
	HANDLE hdl = CreateFileA(chProjFile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (hdl == INVALID_HANDLE_VALUE)
	{
		std::cout << "Ошибка при создании проецируемого файла." << std::endl;
		std::cin.clear();
		std::cin.sync();
		getch();
		return;
	};
	std::cout << "Файловый дескриптор: " << hdl << std::endl << std::endl;
	OutputFile << timeGetTime() << " : PID - " << PID << " : Файловый дескриптор: " << hdl << std::endl << std::endl;

	//Получение размера страницы буферной памяти
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	DWORD PageSize = info.dwPageSize;

	//Создание проекции файла
	HANDLE hdlmap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, chMappingObject);

	if (hdlmap == NULL)
	{
		hdlmap = CreateFileMappingA(hdl, NULL, PAGE_READWRITE, NULL, PageNumber*PageSize, chMappingObject);
		if (hdlmap == NULL)
		{
			std::cout << "Ошибка при создании проецируемого файла." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};
	};
	std::cout << "Дескриптор объекта проекции файла: " << hdlmap << std::endl << std::endl;
	OutputFile << timeGetTime() << " : PID - " << PID << " : Дескриптор объекта проекции файла: " << hdlmap << std::endl << std::endl;

	//Проецирование фрагмента файла в память
	LPVOID MapViewAddr;
	if ((MapViewAddr = MapViewOfFile(hdlmap, FILE_MAP_ALL_ACCESS, NULL, NULL, NULL)) == NULL)
	{
		std::cout << "Ошибка при проецировании фрагмента файла в память." << std::endl;
		std::cin.clear();
		std::cin.sync();
		getch();
		return;
	};
	std::cout << "Базовый адрес объекта проецирования: " << MapViewAddr << std::endl << std::endl;
	OutputFile << timeGetTime() << " : PID - " << PID << " : Базовый адрес объекта проецирования: " << MapViewAddr << std::endl << std::endl;

	//Блокируем страницы буферной памяти
	if (!VirtualLock(MapViewAddr, PageNumber*PageSize))
	{
		std::cout << "Ошибка при блокировании страниц буферной памяти." << std::endl;
		std::cin.clear();
		std::cin.sync();
		getch();
		return;
	};

	//Пауза перед запуском
	std::cout << "Нажмите любую клавишу, чтобы продолжить." << std::endl;
	getch();

	//Создаём или открываем семафор
	HANDLE SemaphoreForWriters = CreateSemaphore(NULL, PageNumber, PageNumber, chSemaphoreWriters);

	if (SemaphoreForWriters == NULL)
	{
		std::cout << "Ошибка при создании или открытии семафора для писателей." << std::endl;
		std::cin.clear();
		std::cin.sync();
		getch();
		return;
	};

	std::cout << "Дескриптор семафора для писателей: " << SemaphoreForWriters << std::endl;
	OutputFile << timeGetTime() << " : PID - " << PID << " : Дескриптор семафора для писателей: " << SemaphoreForWriters << std::endl;

	//Массив мьютексов для записи (по мьютексу на страницу памяти)
	HANDLE MutexWriters[PageNumber];

	for (unsigned short i = 0; i < PageNumber; i++)
	{
		MutexWriters[i] = CreateMutex(NULL, FALSE, chMutexWriters[i]);

		if (MutexWriters[i] == NULL)
		{
			std::cout << "Ошибка при создании или открытии мьютекса для писателя." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		std::cout << "Дескриптор мьютекса для писателей: " << MutexWriters[i] << " для " << i << "-й страницы памяти." << std::endl;
		OutputFile << timeGetTime() << " : PID - " << PID << " : Дескриптор мьютекса для писателей: " << MutexWriters[i] << " для " << i << "-й страницы памяти." << std::endl;
	};

	std::cout << std::endl;

	//Массив мьютексов занятости страниц
	HANDLE MutexBuzy[PageNumber];

	for (unsigned short i = 0; i < PageNumber; i++)
	{
		MutexBuzy[i] = CreateMutex(NULL, FALSE, chMutexBuzy[i]);

		if (MutexBuzy[i] == NULL)
		{
			std::cout << "Ошибка при создании или открытии мьютекса занятости." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		std::cout << "Дескриптор мьютекса занятости: " << MutexBuzy[i] << " для " << i << "-й страницы памяти." << std::endl;
		OutputFile << timeGetTime() << " : PID - " << PID << " : Дескриптор мьютекса занятости: " << MutexBuzy[i] << " для " << i << "-й страницы памяти." << std::endl;
	};

	std::cout << std::endl;

	while (true)
	{
		OutputFile << timeGetTime() << " : PID - " << PID << " : Ожидание, когда число писателей станет меньше 15." << std::endl;

		//Ждём, когда значение счётчика семафора будет больше 0
		if (WaitForSingleObject(SemaphoreForWriters, INFINITE) == WAIT_FAILED)
		{
			std::cout << "Ошибка при ожидании семафора для писателей." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		OutputFile << timeGetTime() << " : PID - " << PID << " : Ожидание, когда появится область памяти, куда можно писать." << std::endl;

		//Ждём, пока появится область памяти, куда можно писать
		int WritePage = 0;
		if ((WritePage = WaitForMultipleObjects(PageNumber, MutexWriters, FALSE, INFINITE)) == WAIT_FAILED)
		{
			std::cout << "Ошибка при ожидании мьютекса для писателя." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		//Определяем, в какую страницу нужно писать
		WritePage -= WAIT_OBJECT_0;

		//Задаём длительность записи от 0,5 до 1,5 сек
		unsigned int WriteTime = 500 + rand() % 1000;

		//Получаем адрес начала региона, куда будем писать
		char* WriteRegion = (char*)MapViewAddr + (WritePage*PageSize);

		//Сколько байт записали
		unsigned int ByteCount = 0;

		OutputFile << timeGetTime() << " : PID - " << PID << " : Ожидание, когда оставшиеся читатели закончат чтение." << std::endl;

		//Ждём, когда оставшиеся читатели освободят страницу
		if (WaitForSingleObject(MutexBuzy[WritePage], INFINITE) == WAIT_FAILED)
		{
			std::cout << "Ошибка при ожидании мьютекса занятости." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};		

		OutputFile << timeGetTime() << " : PID - " << PID << " : Запись в страницу №" << WritePage << "." << std::endl;

		//Начало записи
		unsigned int StartTime = timeGetTime();

		//Пишем заданное время
		while ((timeGetTime() - StartTime) <= WriteTime)
		{
			if (ByteCount < PageSize)
			{
				//Пишем случайные символы
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

		OutputFile << timeGetTime() << " : PID - " << PID << " : Переход к освобождению." << std::endl;

		//Освобождаем мьютекс занятости
		if (!ReleaseMutex(MutexBuzy[WritePage]))
		{
			std::cout << "Ошибка при освобождении мьютекса занятости." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		//Освобождаем мьютекс для писателей
		if (!ReleaseMutex(MutexWriters[WritePage]))
		{
			std::cout << "Ошибка при освобождении мьютекса для писателей." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};

		//Освобождаем семафор
		if (!ReleaseSemaphore(SemaphoreForWriters, 1, NULL))
		{
			std::cout << "Ошибка при освобождении семафора для писателей." << std::endl;
			std::cin.clear();
			std::cin.sync();
			getch();
			return;
		};
	};

	getch();
}