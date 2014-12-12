#ifndef PhoneBase
#define PhoneBase
#define LinkedList_h
#define HashTable_h

#include "stdafx.h"
#include <list>
#include <string>
#include <iostream>
#include <locale>
#include <conio.h>
#include <sstream>
#include <Windows.h>
using namespace std;

namespace BaseDll
{
	struct Item
	{
		std::string key;
		std::string info;
		Item * next;
	};

	class LinkedList
	{
	private:
		Item * head;

	public:

		LinkedList();

		void insertItem(Item * newItem);

		Item * getItem(string itemKey);
		std::string LinkedList::getItemsInStringFormat(string itemKey);

		~LinkedList();
	};

	class HashTable
	{
	private:
		LinkedList * array;

		int length;
		std::list <std::string> listRecord;


	public:
		HashTable(int tableLength = 13);
		
		void insertItem(Item * newItem);

		Item * getItemByKey(string itemKey);

		std::string HashTable::getStringByKey(string itemKey);

		int hash(string itemKey);

		~HashTable();

	};
	extern HashTable *hashTablePhone;
	extern HashTable *hashTableName;
	extern HashTable *hashTableAddress;
	extern HANDLE mutex;
	extern HANDLE file;
	extern HANDLE fileMapping;

	bool createIndex(HashTable * hashTable, int index, char * filemem);
	extern "C" __declspec(dllexport)void createBase();
	extern "C" __declspec(dllexport) std::string find(std::string str, int field);
	extern "C" __declspec(dllexport) void closeResources();

	const int USE_PHONE = 0;
	const int USE_ADDRESS = 4;
	const int USE_NAME = 1;
	const int MAX_MAP_SIZE = 0x00400000;
}
#endif
