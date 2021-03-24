#include "Base.h"

#include <iostream>
#include <chrono>
#include <time.h>
#include <stdarg.h>



int main()
{
	auto spCollision = std::make_shared<JPSCollision>();
	if (!spCollision)
		throw std::bad_alloc();

	//====================================================
	// CREATE MAP 
	//====================================================
	Int32 GridWidth = 300, GridHeight = 300;
	spCollision->Create(GridWidth, GridHeight);

	srand(32);

	// Start, End Position (시작점, 도착점)
	Int32 Sx = 0, Sy = 0;
	Int32 Ex = GridWidth - 1, Ey = GridHeight - 1;

	// Make a Maze (충돌 미로를 만든다.)
	for (Int32 y = 0; y < GridHeight; ++y)
	{
		for (Int32 x = 0; x < GridWidth; ++x)
		{
			// Don't Mark Collision at Start, End Position (시작,도착점은 충돌처리 하지 않는다.)
			if (x == Sx && y == Sy)	continue;
			if (x == Ex && y == Ey) continue;

			// Randomly Mark Collision at Position (랜덤하게 충돌처리를 한다.)
			if (rand() % 50 == 0) spCollision->SetAt(x, y);
		}
	}

	//====================================================
	// Find PATH
	//====================================================
	std::list<JPSCoord> ResultNodes;	// Result for JPS

	JPSPath	jps;
	// SET
	jps.Init(spCollision);
	// SEARCH
	jps.Search(Sx, Sy, Ex, Ey, ResultNodes);
	// STRAIGHT PATH 
	//jps.PullingString(ResultNodes);
	//====================================================
	// SAVE RESULT MAP TO FILE FOR DEBUG
	//====================================================
	std::string results(GridHeight * (GridWidth + 1) + 1, ' ');

	// Mark Collision With '@' ('@' 로 충돌맵을 표시합니다.)
	for (Int32 y = 0; y < GridHeight; y++)
	{
		for (Int32 x = 0; x < GridWidth; x++)
		{
			results[(GridHeight - 1 - y) * (GridWidth + 1) + x] = !spCollision->IsCollision(x, y) ? ' ' : '@';
		}
		results[(GridHeight - 1 - y) * (GridWidth + 1) + GridWidth] = '\n';
	}

	// Mark Path Nodes With '#' ('#'로 찾은 경로를 표시합니다.)
	if (ResultNodes.size() > 0)
	{
		auto iterS = ResultNodes.begin();
		auto iterE = ResultNodes.end();
		auto iterP = iterS;	++iterS;
		for (; iterS != iterE; iterP = iterS, ++iterS)
		{
			auto& PreCoord = (*iterP);
			auto& CurCoord = (*iterS);

			Int32 x = PreCoord.m_x;
			Int32 y = PreCoord.m_y;
			Int32 dx = core::clamp<Int32>(CurCoord.m_x - PreCoord.m_x, -1, 1);
			Int32 dy = core::clamp<Int32>(CurCoord.m_y - PreCoord.m_y, -1, 1);

			for (Int32 v = y, u = x; ; v += dy, u += dx)
			{
				results[(GridHeight - 1 - v) * (GridWidth + 1) + u] = '#';

				if (u == CurCoord.m_x && v == CurCoord.m_y)
					break;

				Int32 deltax = core::clamp<Int32>(CurCoord.m_x - u, -1, 1);
				Int32 deltay = core::clamp<Int32>(CurCoord.m_y - v, -1, 1);
				if (deltax != dx || deltay != dy)
				{
					std::cout << "INVALID NODE\n";
					break;
				}
			}
			results[(GridHeight - 1 - CurCoord.m_y) * (GridWidth + 1) + CurCoord.m_x] = '#';
		}

		// Mark Start & End Position ('S', 'E' 로 시작점 도착점을 표시합니다.)
		auto	iterStart	= ResultNodes.begin();
		auto	iterEnd		= ResultNodes.rbegin();
		auto&	startCoord	= (*iterStart);
		auto&	endCoord	= (*iterEnd);
		results[(GridHeight - 1 - startCoord.m_y) * (GridWidth + 1) + startCoord.m_x] = 'S';
		results[(GridHeight - 1 - endCoord.m_y) * (GridWidth + 1) + endCoord.m_x]	  = 'E';
	}

	//====================================================
	// SAVE FILE
	//====================================================
	if(results.size() > 0)
	{
		FILE* pFile = fopen("result_jps(b).txt", "wt");
		if (pFile != NULL)
		{
			fwrite(results.c_str(), sizeof(char), results.size(), pFile);
			fclose(pFile);
		}
	}
}
