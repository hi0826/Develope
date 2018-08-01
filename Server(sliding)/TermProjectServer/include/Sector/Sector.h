#pragma once
//SECTOR 섹터를 적당한 간격으로 나누자 20 20 
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

