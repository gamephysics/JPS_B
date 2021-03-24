#pragma once

//--------------------------------------------------------------------
// Class: JPSPath
// Desc : JPS (B)
//--------------------------------------------------------------------
class JPSPath
{
private:
	//Directions
	// N, NE, E, SE, S, SW, W, NW , StartPosition
	const Int32					NO_DIRECTION = 8;

	// Priority Queue (Opened List)
	JPSHeap						m_OpenList;

	// searched (Closed List)
	BitArray<Int64>				m_Searched;

	// Goal node 
	JPSCoord					m_EndPos;

	//-------- Field Collision 에서 얻어오는 함수 -----------
	JPSCollision::SharedPtr		m_fiieldCollision;
	Int32						m_GridWidth = 0, m_GridHeight = 0;
	//------------------------------------------------------ 
	
public:
	JPSPath()  = default;
	~JPSPath() = default;

	void Init(JPSCollision::SharedPtr fiieldCollision)
	{
		m_fiieldCollision	= fiieldCollision;
		if (nullptr == m_fiieldCollision) return;

		m_GridWidth			= fiieldCollision->GetWidth();
		m_GridHeight		= fiieldCollision->GetHeight();
		m_Searched.Create(m_GridWidth, m_GridHeight);
	}
	void Destroy()
	{
		m_fiieldCollision	= nullptr;
		m_GridWidth			= 0;
		m_GridHeight		= 0;
		m_Searched.Clear();
	}

	bool Search(Int32 _sX, Int32 _sY, Int32 _eX, Int32 _eY, std::list<JPSCoord>& ResultNodes);
	
private:
	//--------------------------------------------------------------------
	// Code	: IsPassable()
	// Desc	: 해당 좌표가 이동가능한지 검사
	//--------------------------------------------------------------------
	inline bool _IsPassable(const JPSCoord &c)
	{
		return !m_fiieldCollision->IsCollision(c.m_x, c.m_y);
	}

	//--------------------------------------------------------------------
	// Code	: GetNorthEndPointReOpenBB()
	// Desc	: EndPointReOpen 을 pair 로 얻어오는 함수들
	//--------------------------------------------------------------------
	std::pair<Int32, Int32> _GetNorthEndPointReOpenBB(Int32 x, Int32 y)
	{
		if (x < 0 || x >= m_GridWidth)
			return std::pair<Int32, Int32>(-1, -1);

		if (m_fiieldCollision->IsCollision(x, y))
		{
			// 충돌지점에 있으면 다음 오픈지점만 돌려줍니다.
			auto openpos = m_fiieldCollision->GetOpenValue(x, y, false, false);
			return std::pair<Int32, Int32>(openpos, openpos);
		}
		else
		{
			// 오픈지점의 마지막 위치와 다음 오픈지점을 돌려줍니다.
			auto closepos = m_fiieldCollision->GetCloseValue(x, y, false, false);
			auto openpos  = m_fiieldCollision->GetOpenValue(x, closepos, false, false);
			return std::pair<Int32, Int32>(closepos + 1, openpos);
		}
	}

	std::pair<Int32, Int32> _GetSouthEndPointReOpenBB(Int32 x, Int32 y)
	{
		if (x < 0 || x >= m_GridWidth)
			return std::pair<Int32, Int32>(m_GridHeight, m_GridHeight);

		if (m_fiieldCollision->IsCollision(x, y))
		{
			// 충돌지점에 있으면 다음 오픈지점만 돌려줍니다.
			auto openpos = m_fiieldCollision->GetOpenValue(x, y, false, true);
			return std::pair<Int32, Int32>(openpos, openpos);
		}
		else
		{
			// 오픈지점의 마지막 위치와 다음 오픈지점을 돌려줍니다.
			auto closepos = m_fiieldCollision->GetCloseValue(x, y, false, true);
			auto openpos = m_fiieldCollision->GetOpenValue(x, closepos, false, true);
			return std::pair<Int32, Int32>(closepos - 1, openpos);
		}
	}

	std::pair<Int32, Int32> _GetEastEndPointReOpenBB(Int32 x, Int32 y)
	{
		if (y < 0 || y >= m_GridHeight)
			return std::pair<Int32, Int32>(m_GridWidth, m_GridWidth);

		if (m_fiieldCollision->IsCollision(x, y))
		{
			// 충돌지점에 있으면 다음 오픈지점만 돌려줍니다.
			auto openpos = m_fiieldCollision->GetOpenValue(x, y, true, true);
			return std::pair<Int32, Int32>(openpos, openpos);
		}
		else
		{
			// 오픈지점의 마지막 위치와 다음 오픈지점을 돌려줍니다.
			auto closepos = m_fiieldCollision->GetCloseValue(x, y, true, true);
			auto openpos = m_fiieldCollision->GetOpenValue(closepos, y, true, true);
			return std::pair<Int32, Int32>(closepos - 1, openpos);
		}
	}

	std::pair<Int32, Int32> _GetWestEndPointReOpenBB(Int32 x, Int32 y)
	{
		if (y < 0 || y >= m_GridHeight)
			return std::pair<Int32, Int32>(-1, -1);

		if (m_fiieldCollision->IsCollision(x, y))
		{
			// 충돌지점에 있으면 다음 오픈지점만 돌려줍니다.
			auto openpos = m_fiieldCollision->GetOpenValue(x, y, true, false);
			return std::pair<Int32, Int32>(openpos, openpos);
		}
		else
		{
			// 오픈지점의 마지막 위치와 다음 오픈지점을 돌려줍니다.
			auto closepos = m_fiieldCollision->GetCloseValue(x, y, true, false);
			auto openpos = m_fiieldCollision->GetOpenValue(closepos, y, true, false);
			return std::pair<Int32, Int32>(closepos + 1, openpos);
		}
	}

	//--------------------------------------------------------------------
	JPSCoord _NextCoordinate(const JPSCoord& c, const Int32 dir)
	{
		static Int32 dirMov[] = { 0,-1,1,-1,1,0,1,1,0,1,-1,1,-1,0,-1,-1,0,0 };
		return JPSCoord(c.m_x + dirMov[dir * 2], c.m_y + dirMov[dir * 2 + 1]);
	}

	Int32 _GetCoordinateDir(const JPSCoord& S, const JPSCoord& D)
	{
		Int32 dirs = 0;
		if		(S.m_x > D.m_x)  dirs += 1 << 4; 
		else if (S.m_x < D.m_x)	 dirs += 1 << 2; 
		if		(S.m_y > D.m_y)  dirs += 1 << 3; 
		else if (S.m_y < D.m_y)	 dirs += 1 << 1; 
		return dirs;
	}

	inline Int32 _DirIsDiagonal(const Int32 dir)
	{
		return (dir % 2) != 0;
	}
	inline Int32 _Implies(const Int32 a, const Int32 b)
	{
		return a ? b : 1;
	}
	inline Int32 _AddDirectionToSet(const Int32 dirs, const Int32 dir)
	{
		return dirs | 1 << dir;
	}
	Int32 _ForcedNeighbours(const JPSCoord &coord, const Int32 dir)
	{
		if (dir > 7)
			return 0;

		Int32 dirs = 0;
#define ENTERABLE(n) _IsPassable ( _NextCoordinate (coord, (dir + (n)) % 8))

		if (_DirIsDiagonal(dir)) {
			if (!_Implies(ENTERABLE(6), ENTERABLE(5)))
				dirs = _AddDirectionToSet(dirs, (dir + 6) % 8);
			if (!_Implies(ENTERABLE(2), ENTERABLE(3)))
				dirs = _AddDirectionToSet(dirs, (dir + 2) % 8);

		}
		else {
			if (!_Implies(ENTERABLE(7), ENTERABLE(6)))
				dirs = _AddDirectionToSet(dirs, (dir + 7) % 8);
			if (!_Implies(ENTERABLE(1), ENTERABLE(2)))
				dirs = _AddDirectionToSet(dirs, (dir + 1) % 8);
		}

#undef ENTERABLE	

		return dirs;
	}

	Int32 _NaturalNeighbours(const Int32 dir)
	{
		if (dir == NO_DIRECTION)
			return 255;

		Int32 dirs = 0;
		dirs = _AddDirectionToSet(dirs, dir);
		if (_DirIsDiagonal(dir)) {
			dirs = _AddDirectionToSet(dirs, (dir + 1) % 8);
			dirs = _AddDirectionToSet(dirs, (dir + 7) % 8);
		}
		return dirs;
	}

	bool _GetJumpPointNew(JPSCoord s, const char direction, JPSCoord & jp)
	{
		s = _NextCoordinate(s, direction);

		if (!_IsPassable(s))
			return false;
		bool ret = false;
		std::pair<Int32, Int32> up, center, down;
		switch (direction)
		{

		case 0://North
			up		= _GetNorthEndPointReOpenBB(s.m_x - 1, s.m_y);
			center	= _GetNorthEndPointReOpenBB(s.m_x, s.m_y);
			down	= _GetNorthEndPointReOpenBB(s.m_x + 1, s.m_y);

			if (s.m_x == m_EndPos.m_x && s.m_y >= m_EndPos.m_y && center.first <= m_EndPos.m_y)
			{
				jp = m_EndPos;
				return true;
			}
			if (down.first != -1 && ((down.second > -1 && down.first > center.first && down.second + 2 > center.first) || (down.first == down.second && down.first + 2 > center.first)))
			{
				jp = JPSCoord(s.m_x, down.second + 1);
				ret = true;
			}
			if (up.first != -1 && ((up.second > -1 && up.first > center.first&&up.second + 2 > center.first) || (up.first == up.second && up.first + 2 > center.first)))
			{
				jp = JPSCoord(s.m_x, ret ? std::max(jp.m_y, up.second + 1) : up.second + 1);
				return true;
			}
			return ret;
		case 2://EAST
			up		= _GetEastEndPointReOpenBB(s.m_x, s.m_y - 1);
			center	= _GetEastEndPointReOpenBB(s.m_x, s.m_y);
			down	= _GetEastEndPointReOpenBB(s.m_x, s.m_y + 1);

			if (s.m_y == m_EndPos.m_y && s.m_x <= m_EndPos.m_x && center.first >= m_EndPos.m_x)
			{
				jp = m_EndPos;
				return true;
			}

			if (down.first != m_GridWidth && ((down.second < m_GridWidth&&down.first < center.first && down.second - 2 < center.first) || (down.first == down.second && down.first - 2 < center.first)))
			{
				jp = JPSCoord(down.second - 1, s.m_y);
				ret = true;
			}
			if (up.first != m_GridWidth && ((up.second < m_GridWidth&&up.first < center.first&&up.second - 2 < center.first) || (up.first == up.second && up.first - 2 < center.first)))
			{
				jp = JPSCoord(ret ? std::min(jp.m_x, up.second - 1) : up.second - 1, s.m_y);
				return true;
			}
			return ret;
		case 4://SOUTH
			up		= _GetSouthEndPointReOpenBB(s.m_x - 1, s.m_y);
			center	= _GetSouthEndPointReOpenBB(s.m_x, s.m_y);
			down	= _GetSouthEndPointReOpenBB(s.m_x + 1, s.m_y);

			if (s.m_x == m_EndPos.m_x && s.m_y <= m_EndPos.m_y && center.first >= m_EndPos.m_y )
			{
				jp = m_EndPos;
				return true;
			}
			if (down.first != m_GridHeight && ((down.second < m_GridHeight&& down.first < center.first && down.second - 2 < center.first) || (down.first == down.second && down.first - 2 < center.first)))
			{
				jp = JPSCoord(s.m_x, down.second - 1);
				ret = true;
			}
			if (up.first != m_GridHeight && ((up.second < m_GridHeight&&up.first < center.first&&up.second - 2 < center.first) || (up.first == up.second && up.first - 2 < center.first)))
			{
				jp = JPSCoord(s.m_x, ret ? std::min(jp.m_y, up.second - 1) : up.second - 1);
				return true;
			}
			return ret;
		case 6://WEST
			up		= _GetWestEndPointReOpenBB(s.m_x, s.m_y - 1);
			center	= _GetWestEndPointReOpenBB(s.m_x, s.m_y);
			down	= _GetWestEndPointReOpenBB(s.m_x, s.m_y + 1);

			if (s.m_y == m_EndPos.m_y && s.m_x >= m_EndPos.m_x && center.first <= m_EndPos.m_x)
			{
				jp = m_EndPos;
				return true;
			}
			if (down.first != -1 && ((down.second > -1 && down.first > center.first && down.second + 2 > center.first) || (down.first == down.second && down.first + 2 > center.first)))
			{
				jp = JPSCoord(down.second + 1, s.m_y);
				ret = true;
			}
			if (up.first != -1 && ((up.second > -1 && up.first > center.first&&up.second + 2 > center.first) || (up.first == up.second && up.first + 2 > center.first)))
			{
				jp = JPSCoord(ret ? std::max(jp.m_x, up.second + 1) : up.second + 1, s.m_y);
				return true;
			}
			return ret;
		}
		return false;
	}

	JPSCoord _JumpNew(const JPSCoord &c, const char dir)
	{
		JPSCoord nc = _NextCoordinate(c, dir);
		bool isDiag = _DirIsDiagonal(dir);
		JPSCoord offset(0, 0);
		offset		= _NextCoordinate(offset, dir);

		while (1)
		{
			if (!_IsPassable(nc))
				return JPSCoord(-1, -1);
			if (_ForcedNeighbours(nc, dir) || m_EndPos == nc)
				return nc;
			if (isDiag)
			{
				JPSCoord newP(-1, -1);
				if (_GetJumpPointNew(nc, (dir + 7) % 8, newP))
					return nc;
				if (_GetJumpPointNew(nc, (dir + 1) % 8, newP))
					return nc;
			}
			else
			{
				JPSCoord newP(-1, -1);
				_GetJumpPointNew(nc, dir, newP);
				return newP;
			}
			nc.Add(offset);
		}
		return JPSCoord(-1, -1);
	}

public:
	//--------------------------------------------------------------------
	// Code	: PullingString()
	// Desc	: Straighten JigZag Coordinates 
	//--------------------------------------------------------------------
	bool PullingString(std::list<JPSCoord>& ResultNodes)
	{
		if (ResultNodes.size() <= 2) return false;

		auto iterS = ResultNodes.begin();
		auto iterE = ResultNodes.end();

		auto iterB = iterS;	++iterS;
		auto iterP = iterS;	++iterS;

		for (; iterS != iterE; iterP = iterS, ++iterS)
		{
			auto& BaseNode = (*iterB);
			auto& PrevNode = (*iterP);
			auto& CurrNode = (*iterS);

			if (IsStraightPassable(BaseNode.m_x, BaseNode.m_y, CurrNode.m_x, CurrNode.m_y) == true)
			{
				ResultNodes.erase(iterP);
			}
			else
			{
				iterB = iterP;
			}
		}

		if (ResultNodes.size() <= 0) return false;

		return true;
	}

	bool IsStraightPassable(Int32 fromX, Int32 fromY, Int32 toX, Int32 toY)
	{
		float difX = (float)(toX - fromX);
		float difY = (float)(toY - fromY);
		float step = std::max(std::abs(difX), std::abs(difY));

		float dx = difX / step;
		float dy = difY / step;

		for (int i = 0; i <= std::ceil(step); i++)
		{
			JPSCoord  pos(fromX + Int32(dx * (float)i), fromY + Int32(dy * (float)i));
			if (core::InBoundsEx(pos.m_x, fromX, toX) == false)		continue;
			if (core::InBoundsEx(pos.m_y, fromY, toY) == false)		continue;

			// 해당 SPOT이 Passable 검사
			if (_IsPassable(pos) == false) return false;
		}

		return true;
	}
};
