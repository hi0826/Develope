#pragma once
//SECTOR ���͸� ������ �������� ������ 20 20 
#include "../Include.h"
class CSector 
{
public:
	unordered_set<int>      SectorList; 
    mutex                   Stm;
public:
	CSector();
	~CSector();
public:
	void      Insert(int id);
	void      Delete(int id);
};

