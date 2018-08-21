#pragma once
#include "../include/SingleTone.h"
#include"../include/stdafx.h"
class CMapData  :public CSingleTone<CMapData>
{
public:
	std::unordered_map<BYTE, std::vector<BoundingOrientedBox*>> MapBoundingBoxes;
	int Stage1[20][20];
	int Stage2[20][20];
	int Stage3[20][20];
	BYTE Width;
	BYTE Height;
public:
	CMapData();
	~CMapData();

public: 
	bool Initialize();
	void CreateMapBoundingBox();
	void Close();
};

