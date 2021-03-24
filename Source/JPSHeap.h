#pragma once

//--------------------------------------------------------------------
// Class: JPSCoord 
// Desc : TILE COORDINATE
//--------------------------------------------------------------------
struct JPSCoord
{
	Int32 m_x = -1, m_y = -1;

public:
	JPSCoord() = default;
	JPSCoord(Int32 _x, Int32 _y) : m_x(_x), m_y(_y)	{ }

	void Add(const JPSCoord& rhs)	{ m_x += rhs.m_x;	m_y += rhs.m_y; }
	void Clear()					{ m_x = -1;	m_y = -1; }
	bool IsEmpty()					{ return (m_x == -1 && m_y == -1); }

	bool     operator ==(const JPSCoord& C) const { return ((C.m_x == m_x) && (C.m_y == m_y)); }
	bool     operator !=(const JPSCoord& C) const { return ((C.m_x != m_x) || (C.m_y != m_y)); }

	float DistanceSqrt(const JPSCoord& rhs)
	{
		Int32 absX = std::abs(m_x - rhs.m_x);
		Int32 absY = std::abs(m_y - rhs.m_y);

		Int32 diagDist = std::min(absX, absY);
		Int32 straightDist = std::max(absX, absY) - diagDist;
		return diagDist * 1.414213562373095f + straightDist;
	}
};

//--------------------------------------------------------------------
// Class: JPSNode
// Desc : A star 길찾기 방식에 사용되어지는 정보를 포함하는 JPSNode
//--------------------------------------------------------------------
struct JPSNode
{
	typedef std::shared_ptr<JPSNode>	SharedPtr;
	typedef std::weak_ptr<JPSNode>		WeakPtr;

	JPSNode::SharedPtr	m_spParent = nullptr;
	JPSCoord			m_Pos;					// Compare Same Position
	char				m_CardinalDir = 0;
	float				m_Score = 0;
	float				m_Heuri = 0;
	float				m_Total = 0;			// Total Score가 적은 Node 가 P.Queue 에서 위로 올라간다.

	JPSNode() = default;

	void Clear()
	{
		m_spParent = nullptr;
		m_Pos.Clear();
		m_CardinalDir = 0;
		m_Score		= 0;
		m_Heuri		= 0;
		m_Total		= 0;
	}

	void Set(JPSNode::SharedPtr spParent, const JPSCoord& Pos, const JPSCoord& End, const char CardinalDir)
	{
		m_spParent	= spParent;
		m_Pos		= Pos;
		m_CardinalDir	= CardinalDir;
		if (m_spParent)
			m_Score = m_Pos.DistanceSqrt(m_spParent->m_Pos) + m_spParent->m_Score;
		else
			m_Score = 0;
		m_Heuri		= m_Pos.DistanceSqrt(End);
		m_Total		= m_Score + m_Heuri;
	}

};


//--------------------------------------------------------------------
// class: JPSHeap 
// Desc : priority queue
//--------------------------------------------------------------------
class JPSHeap
{
	std::vector<JPSNode::SharedPtr> m_Heap;

public:
	JPSHeap()	= default;
	~JPSHeap()	= default;

	void InsertNewone(JPSNode::SharedPtr value)
	{
		m_Heap.push_back(value);
		_ShiftUp(m_Heap.size() - 1);
	}

	bool InsertSmaller(JPSNode::SharedPtr value)
	{
		auto	pSeg = m_Heap.data();
		auto	cbSize = m_Heap.size();

		for (size_t n = 0; n < cbSize; ++n, ++pSeg)
		{
			if (value->m_Pos != (*pSeg)->m_Pos)
				continue;
			
			if (value->m_Total >= (*pSeg)->m_Total)
			{
				return false;
			}
			else
			{
				(*pSeg) = value;
				_ShiftUp(n);
				return true;
			}
		}

		return false;
	}

	JPSNode::SharedPtr PopMax()
	{
		auto	result = m_Heap.front();
		m_Heap.front() = m_Heap.back();
		m_Heap.pop_back();
		_ShiftDown(0);
		return result;
	}

	const	JPSNode::SharedPtr Max() const	{	return m_Heap.front();		}
	Int32	Count() const					{	return (Int32)m_Heap.size();}
	void	Clear()							{	m_Heap.clear();				}

private:
	typedef std::vector<JPSNode::SharedPtr>::size_type _heap_size_t;

	void _BuildHeap()
	{
		for (Int64 i = (Int64)(m_Heap.size() / 2); i >= 0; --i)
			_ShiftDown(i);
	}

	void _ShiftDown(_heap_size_t parent)
	{
		auto	pSeg = m_Heap.data();
		auto	cbSize = m_Heap.size();
		for (;;)
		{
			auto	left = static_cast<_heap_size_t>(parent * 2 + 1);
			// no children so done
			if (left >= cbSize) break;

			auto	right = left + 1;
			// if there is a right child, and it is smaller than left child, use the left instead
			if (right < cbSize && pSeg[right]->m_Total < pSeg[left]->m_Total)
				left = right;

			// parent is smaller than (or equal to) smallest child so done
			if (pSeg[parent]->m_Total < pSeg[left]->m_Total)
				break;

			std::swap(pSeg[parent], pSeg[left]);
			parent = left;
		}
	}

	void _ShiftUp(_heap_size_t node)
	{
		auto	pSeg = m_Heap.data();
		while (node > 0)
		{
			auto	parent = static_cast<_heap_size_t>((node - 1) / 2);		//	parent index

			// child item is larger than (or equal) parent so we're done
			if (pSeg[node]->m_Total > pSeg[parent]->m_Total)
				return;

			std::swap(pSeg[node], pSeg[parent]);
			node = parent;
		}
	}
};
