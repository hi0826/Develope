#pragma once
#include "SingleTone.h"
#include <fstream>

class CMapData :public CSingleTone<CMapData>
{
	int** Map;
	int Width;
	int Height;
public:
	bool  Initialize()
	{
		ifstream  in("Asset/Map/Map.geg");

		if (in.is_open())
		{
			in >> Width;
			in >> Height;

			Map = new int*[Height];
			for (int i = 0; i < Height; ++i) {
				Map[i] = new int[Width];
			}
			for (int y = 0; y < Height; ++y)
				for (int x = 0; x < Width; ++x)
					in >> Map[y][x];

			return true;
		}
		return false; 
	}

	void Close()
	{

		if (Map) {

			for (int y = 0; y < Height; ++y)
			{
				if (Map[y]) delete[] Map[y];
				Map[y] = NULL;
			}
			delete[] Map;
		}

	}
};

