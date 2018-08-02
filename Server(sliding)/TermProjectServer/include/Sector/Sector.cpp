#include "Sector.h"



CSector::CSector()
{

}


CSector::~CSector()
{

}

void CSector::Insert(int id)
{
	Stm.lock();
	SectorList.insert(id);
	Stm.unlock();
}

void CSector::Delete(int id)
{
	Stm.lock();
	SectorList.erase(id);
	Stm.unlock();
}
