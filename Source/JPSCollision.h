#pragma once

//--------------------------------------------------------------------
// Class: JPSCollision
// Desc	: 
//--------------------------------------------------------------------
class JPSCollision
{
	Int32	m_width  = 0;
	Int32	m_height = 0;

	static const Int64 npos = ~(0);	//	default npos == -1

	BitArray<Int64>	m_xBoundaryPoints;
	BitArray<Int64>	m_yBoundaryPoints;

public:
	typedef std::shared_ptr<JPSCollision>		SharedPtr;
	typedef std::weak_ptr<JPSCollision>			WeakPtr;

	virtual	~JPSCollision()		{};
	bool	Create(const Int32& width, const Int32& height);
	JPSCollision& operator = (const JPSCollision& B);

	//	[일반함수]
	auto	GetWidth() const ->decltype(m_width)	{ return m_width;	}
	auto	GetHeight() const->decltype(m_height)	{ return m_height;	}
	void	SetWidth (const decltype(m_width)& o)	{ m_width = o;		}
	void	SetHeight(const decltype(m_height)& o)	{ m_height = o;		}
	bool	IsOutBound(Int32 x, Int32 y);
	bool	IsCollision(Int32 x, Int32 y);		

	//	[충돌맵]
	void	SetAt(Int32 x, Int32 y);
	void	ClrAt(Int32 x, Int32 y);

	// [길찾기 가속용]  : BLJPS 실시간 검색을 위한 함수
	Int32	GetCloseValue(Int32 x, Int32 y, bool xaxis, bool forward);
	Int32	GetOpenValue(Int32 x, Int32 y, bool xaxis, bool forward);

private:
	Int32	_GetPosX(Int32 x, Int32 y);
	Int32	_GetPosY(Int32 x, Int32 y);

	static bool _flsll(unsigned long& index, UInt64 word);
	static bool _ffsll(unsigned long& index, UInt64 word);
};
