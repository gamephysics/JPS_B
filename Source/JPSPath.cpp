#include "Base.h"

//--------------------------------------------------------------------
// Class: Search()
// Desc	: 
//--------------------------------------------------------------------
bool JPSPath::Search(Int32 _sX, Int32 _sY, Int32 _eX, Int32 _eY, std::list<JPSCoord>& ResultNodes)
{
	//===================================================================
	// CHECK
	//===================================================================

	if ((_sX < 0 || _sX >= m_GridWidth) ||
		(_sY < 0 || _sY >= m_GridHeight) ||
		(_sX == _eX && _sY == _eY) ||
		(m_fiieldCollision == nullptr))
	{
		return false;
	}

	//===================================================================
	// PREPARE
	//===================================================================
	m_EndPos.m_x = _eX;
	m_EndPos.m_y = _eY;
	ResultNodes.clear();
	m_OpenList.Clear();	// Ready the openlist, closed list and node container for search
	m_Searched.Clear();

	//================================================
	// START  : Insert the start nodes into the openList
	//================================================
	auto spStartNode = std::make_shared<JPSNode>();
	if (!spStartNode)	
		throw std::bad_alloc();
	spStartNode->Set(nullptr, JPSCoord(_sX, _sY), m_EndPos, 8);
	//================================================

	m_OpenList.InsertNewone(spStartNode);
	m_Searched.SetAt(spStartNode->m_Pos.m_x, spStartNode->m_Pos.m_y, true);

	//===================================================================
	// LOOP
	//===================================================================
	// Keep iterating over openlist until a solution is found or list is empty
	while (m_OpenList.Count())
	{
		auto  spCurrNode = m_OpenList.PopMax();
		Int32 Directions = _ForcedNeighbours(spCurrNode->m_Pos, spCurrNode->m_CardinalDir) | _NaturalNeighbours(spCurrNode->m_CardinalDir);

		for (Int32 dir = 0; dir < 8; dir++)
		{
			if ((1 << dir) & Directions)
			{
				auto CoordinateNewC = _JumpNew(spCurrNode->m_Pos, dir);
				if (!CoordinateNewC.IsEmpty())
				{
					//================================================
					// ARRIVED
					//================================================
					if (CoordinateNewC == m_EndPos)
					{
						ResultNodes.push_front(m_EndPos);

						// RECURSIVE TO START POINT
						auto  spTraceNode = spCurrNode;
						Int32 CurDir = _GetCoordinateDir(m_EndPos, spCurrNode->m_Pos);
						while (spTraceNode)
						{
							Int32 NextDir = 0;
							if (spTraceNode->m_spParent)	NextDir = _GetCoordinateDir(spTraceNode->m_Pos, spTraceNode->m_spParent->m_Pos);
							if (CurDir != NextDir)			ResultNodes.push_front(spTraceNode->m_Pos);
							spTraceNode = spTraceNode->m_spParent;
							CurDir = NextDir;
						}
						return true;
					}

					//================================================
					// KEEP SEARCH
					//================================================
					auto newNode = std::make_shared<JPSNode>();
					if (!newNode)
						throw std::bad_alloc();
					newNode->Set(spCurrNode, CoordinateNewC, m_EndPos, dir);
					//================================================

					// Insert or update node into the openList
					if (!m_Searched.IsSet(CoordinateNewC.m_x, CoordinateNewC.m_y))
					{
						m_OpenList.InsertNewone(newNode);
						m_Searched.SetAt(CoordinateNewC.m_x, CoordinateNewC.m_y, true);
					}
					else
					{
						m_OpenList.InsertSmaller(newNode);
					}
				}
			}
		}
	}

	return false;
}


