// OSISPWin32.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PhoneBaseDll.h"

using namespace BaseDll;



HashTable::HashTable(int tableLength)
{
	if (tableLength <= 0) tableLength = 100;
	array = new LinkedList[tableLength];
	length = tableLength;
}

int HashTable::hash(string itemKey)
{
	int value = 0;
	for (int i = 0; i < itemKey.length(); i++)
		value += itemKey[i];
	return (itemKey.length() * value) % length;
}

void HashTable::insertItem(Item * newItem)
{
	int index = hash(newItem->key);
	array[index].insertItem(newItem);
}


/*Item * HashTable::getItemByKey(string itemKey)
{
int index = hash(itemKey);
return array[index].getItem(itemKey);
}*/

std::string HashTable::getStringByKey(string itemKey)
{
	int index = hash(itemKey);
	return array[index].getItemsInStringFormat(itemKey);
}

HashTable::~HashTable()
{
	delete[] array;
}


LinkedList::LinkedList()
{
	head = new Item;
	head->next = NULL;
}

void LinkedList::insertItem(Item * newItem)
{
	if (!head->next)
	{
		head->next = newItem;
		return;
	}
	Item * p = head;
	Item * q = head;
	while (q)
	{
		p = q;
		q = p->next;
	}
	p->next = newItem;
	newItem->next = NULL;
}

Item * LinkedList::getItem(string itemKey)
{
	Item * p = head;
	Item * q = head;
	while (q)
	{
		p = q;
		if ((p != head) && (strcmp(p->key.c_str(), itemKey.c_str()) == 0))
			return p;
		q = p->next;
	}
	return NULL;
}

std::string LinkedList::getItemsInStringFormat(string itemKey)
{
	Item * p = head;
	Item * q = head;
	std::string str = "";
	while (q)
	{
		p = q;
		if ((p != head) && (strcmp(p->key.c_str(), itemKey.c_str()) == 0))
		{
			str += p->info + '\n';
		}
		q = p->next;
	}
	return str;
}

LinkedList::~LinkedList()
{
	Item * p = head;
	Item * q = head;
	while (q)
	{
		p = q;
		q = p->next;
		if (q) delete p;
	}
}
HashTable *BaseDll::hashTablePhone;
HashTable *BaseDll::hashTableName;
HashTable *BaseDll::hashTableAddress;

HANDLE BaseDll::mutex;
HANDLE BaseDll::file;
HANDLE BaseDll::fileMapping;
int mapSize;
extern "C" __declspec(dllexport) void BaseDll::createBase()
{
	mutex = CreateMutex(NULL, FALSE, L"Mutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"Mutex");
		WaitForSingleObject(mutex, INFINITE);
		fileMapping = OpenFileMapping(FILE_MAP_READ, FALSE, L"Base");
	}
	else
	{
		file = CreateFile(L"telbase.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		fileMapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, L"Base");
		CloseHandle(file);
	}
	ReleaseMutex(mutex);
}

extern "C" __declspec(dllexport) std::string find(std::string str, int field)
{
	HashTable *hashTable = NULL;
	char* filemem;
	DWORD fileSize = GetFileSize(file, NULL);
	int nIterations = 0;

	int offset = 0;
	std::string resultString;
	std::string analyzedString;
	bool isEndOfRecords = true;
	if (fileSize % MAX_MAP_SIZE != 0)
	{
		nIterations = fileSize / MAX_MAP_SIZE + 1;
	}
	else
	{
		nIterations /= MAX_MAP_SIZE;
	}
	for (int i = 0; i < nIterations; i++)
	{
		if (i != nIterations - 1)
		{
			mapSize = MAX_MAP_SIZE;
		}
		else
		{
			mapSize = fileSize - offset;
		}
		filemem = (char*)MapViewOfFile(fileMapping, FILE_MAP_READ, 0, offset, mapSize);
		if (isEndOfRecords)
		{
			hashTable = new HashTable(255);
			switch (field)
			{
			case USE_PHONE:
				isEndOfRecords = createIndex(hashTable, USE_PHONE, filemem);
				break;
			case USE_NAME:
				isEndOfRecords = createIndex(hashTable, USE_NAME, filemem);
				break;
			case USE_ADDRESS:
				isEndOfRecords = createIndex(hashTable, USE_ADDRESS, filemem);
				break;
			}
			resultString += hashTable->getStringByKey(str);
			delete hashTable;
		}
		UnmapViewOfFile(filemem);
		offset += MAX_MAP_SIZE;
	}
	return strcmp(resultString.c_str(), "") ? resultString : "Ничего не найдено.";
}

bool BaseDll::createIndex(HashTable * hashTable, int index, char * filemem)
{
	int size = mapSize;
	std::string currentString;
	std::string key;
	int j;
	bool result = true;
	for (int i = 0; i < size - 1; i++)
	{
		currentString += filemem[i];
		if (filemem[i] == '\n' && (i != size - 1))
		{
			j = 0;
			if (currentString.find("endrecords") == 0){
				result = false;
				break;
			}
			for (int k = 0; k < index; k++)
			{
				while (currentString[j] != ';')
				{
					j++;
				}
				j++;
			}
			while (currentString[j] != ';')
			{
				key += currentString[j];
				j++;
			}
			Item * newItem = new Item{ key, currentString, NULL };
			hashTable->insertItem(newItem);
			currentString = "";
			key = "";
		}
	}
	return result;
}
extern "C" __declspec(dllexport) void closeResources(){
	CloseHandle(mutex);
	CloseHandle(fileMapping);
}

