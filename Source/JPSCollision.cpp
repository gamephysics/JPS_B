#include "Base.h"

//--------------------------------------------------------------------
// Code : Create
// Desc : Collision 생성
//--------------------------------------------------------------------
bool JPSCollision::Create(const Int32& width, const Int32& height)
{
	this->m_width = width;
	this->m_height = height;

	m_xBoundaryPoints.Create(width, height);
	m_yBoundaryPoints.Create(height, width);

	// OUT OF BOUND COLLISION SET
	for (int x = width; x < m_xBoundaryPoints.GetBitsWidths(); ++x)
	{
		for (int y = height; y < m_yBoundaryPoints.GetBitsWidths(); ++y)
		{
			SetAt(x, y);
		}
	}

	return true;
}

//--------------------------------------------------------------------
// Code : operator = 
// Desc : Collision 복사
//--------------------------------------------------------------------
JPSCollision& JPSCollision::operator = (const JPSCollision& B)
{
	this->m_width		= B.m_width;
	this->m_height		= B.m_height;

	this->m_xBoundaryPoints = B.m_xBoundaryPoints;
	this->m_yBoundaryPoints = B.m_yBoundaryPoints;

	return *this;
}

//--------------------------------------------------------------------
// Code : IsOutBound
// Desc : x, y 좌표가 크기를 벗어나는지 
//--------------------------------------------------------------------
bool JPSCollision::IsOutBound(Int32 x, Int32 y)
{
	if (x < 0)			return true;
	if (y < 0)			return true;
	if (x >= m_width)	return true;
	if (y >= m_height)	return true;
	return false;
}

//--------------------------------------------------------------------
// Code : IsCollision
// Desc : x, y Collision 검사
//--------------------------------------------------------------------
bool JPSCollision::IsCollision(Int32 x, Int32 y)
{
	return m_xBoundaryPoints.IsSet(x, y);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//	[충돌맵]
//--------------------------------------------------------------------
// Code : SetAt
// Desc : x, y 좌표를 충돌지점으로 설정
//--------------------------------------------------------------------
void JPSCollision::SetAt(Int32 x, Int32 y)
{
	m_xBoundaryPoints.SetAt(x, y, true);
	m_yBoundaryPoints.SetAt(y, x, true);
}

//--------------------------------------------------------------------
// Code : ClrAt
// Desc : x, y 좌표를 충돌지점에서 해제
//--------------------------------------------------------------------
void JPSCollision::ClrAt(Int32 x, Int32 y)
{
	m_xBoundaryPoints.SetAt(x, y, false);
	m_yBoundaryPoints.SetAt(y, x, false);
}

// [길찾기 가속용]  : BLJPS 실시간 검색을 위한 함수
//--------------------------------------------------------------------
// Code : GetCloseValue
// Desc : x, y 좌표에서 x or y 축 forward(+) or backward(-) 방향으로 검색
//--------------------------------------------------------------------
Int32  JPSCollision::GetCloseValue(Int32 x, Int32 y, bool xaxis, bool forward)
{
	static const UInt64 plusTable[] = {
		18446744073709551615ULL,		18446744073709551614ULL,		18446744073709551612ULL,		18446744073709551608ULL,
		18446744073709551600ULL,		18446744073709551584ULL,		18446744073709551552ULL,		18446744073709551488ULL,
		18446744073709551360ULL,		18446744073709551104ULL,		18446744073709550592ULL,		18446744073709549568ULL,
		18446744073709547520ULL,		18446744073709543424ULL,		18446744073709535232ULL,		18446744073709518848ULL,
		18446744073709486080ULL,		18446744073709420544ULL,		18446744073709289472ULL,		18446744073709027328ULL,
		18446744073708503040ULL,		18446744073707454464ULL,		18446744073705357312ULL,		18446744073701163008ULL,
		18446744073692774400ULL,		18446744073675997184ULL,		18446744073642442752ULL,		18446744073575333888ULL,
		18446744073441116160ULL,		18446744073172680704ULL,		18446744072635809792ULL,		18446744071562067968ULL,
		18446744069414584320ULL,		18446744065119617024ULL,		18446744056529682432ULL,		18446744039349813248ULL,
		18446744004990074880ULL,		18446743936270598144ULL,		18446743798831644672ULL,		18446743523953737728ULL,
		18446742974197923840ULL,		18446741874686296064ULL,		18446739675663040512ULL,		18446735277616529408ULL,
		18446726481523507200ULL,		18446708889337462784ULL,		18446673704965373952ULL,		18446603336221196288ULL,
		18446462598732840960ULL,		18446181123756130304ULL,		18445618173802708992ULL,		18444492273895866368ULL,
		18442240474082181120ULL,		18437736874454810624ULL,		18428729675200069632ULL,		18410715276690587648ULL,
		18374686479671623680ULL,		18302628885633695744ULL,		18158513697557839872ULL,		17870283321406128128ULL,
		17293822569102704640ULL,		16140901064495857664ULL,		13835058055282163712ULL,		9223372036854775808ULL,
	};

	static const UInt64 minusTable[] = {
		1ULL,						3ULL,						7ULL,						15ULL,
		31ULL,						63ULL,						127ULL,						255ULL,
		511ULL,						1023ULL,					2047ULL,					4095ULL,
		8191ULL,					16383ULL,					32767ULL,					65535ULL,
		131071ULL,					262143ULL,					524287ULL,					1048575ULL,
		2097151ULL,					4194303ULL,					8388607ULL,					16777215ULL,
		33554431ULL,				67108863ULL,				134217727ULL,				268435455ULL,
		536870911ULL,				1073741823ULL,				2147483647ULL,				4294967295ULL,
		8589934591ULL,				17179869183ULL,				34359738367ULL,				68719476735ULL,
		137438953471ULL,			274877906943ULL,			549755813887ULL,			1099511627775ULL,
		2199023255551ULL,			4398046511103ULL,			8796093022207ULL,			17592186044415ULL,
		35184372088831ULL,			70368744177663ULL,			140737488355327ULL,			281474976710655ULL,
		562949953421311ULL,			1125899906842623ULL,		2251799813685247ULL,		4503599627370495ULL,
		9007199254740991ULL,		18014398509481983ULL,		36028797018963967ULL,		72057594037927935ULL,
		144115188075855871ULL,		288230376151711743ULL,		576460752303423487ULL,		1152921504606846975ULL,
		2305843009213693951ULL,		4611686018427387903ULL,		9223372036854775807ULL,		18446744073709551615ULL,
	};


	Int32	maxvalue = xaxis ? m_width : m_height;
	auto&	maskbound = xaxis ? m_xBoundaryPoints : m_yBoundaryPoints;
	auto	variable = xaxis ? x : y;
	auto	nbitmask = xaxis ? m_xBoundaryPoints.__NBITMASK : m_yBoundaryPoints.__NBITMASK;
	if (forward)
	{
		auto	MAX = maskbound.GetWordWidths();
		for (int i = 0; i < MAX; ++i)
		{
			auto  pos = xaxis ? _GetPosX(x + i * nbitmask, y) : _GetPosY(x, y + i * nbitmask);
			if (pos == npos) return maxvalue;

			//auto  value = maskbound.GetValue(pos);
			auto  value = maskbound[pos / nbitmask];
			if (i == 0) value &= plusTable[(variable%nbitmask)];

			unsigned long index = 0;
			if (JPSCollision::_ffsll(index, value))
			{
				return (variable - (variable%nbitmask) + i * nbitmask) + index;
			}
		}
		return maxvalue;
	}
	else
	{

		auto	MAX = maskbound.GetWordWidths();
		for (int i = 0; i < MAX; ++i)
		{
			auto  pos = xaxis ? _GetPosX(x - i * nbitmask, y) : _GetPosY(x, y - i * nbitmask);
			if (pos == npos) return -1;

			//auto  value = maskbound.GetValue(pos);
			auto  value = maskbound[pos / nbitmask];
			if (i == 0) value &= minusTable[(variable%nbitmask)];

			unsigned long index = 0;
			if (JPSCollision::_flsll(index, value))
			{
				return (variable - (variable%nbitmask) - i * nbitmask) + index;
			}
		}
		return -1;
	}
}

//--------------------------------------------------------------------
// Code : GetOpenValue
// Desc : x, y 좌표에서 x or y 축 forward(+) or backward(-) 방향으로 검색
//--------------------------------------------------------------------
Int32 JPSCollision::GetOpenValue(Int32 x, Int32 y, bool xaxis, bool forward)
{
	const static UInt64 plusTable[] = {
		18446744073709551615ULL,		18446744073709551614ULL,		18446744073709551612ULL,		18446744073709551608ULL,
		18446744073709551600ULL,		18446744073709551584ULL,		18446744073709551552ULL,		18446744073709551488ULL,
		18446744073709551360ULL,		18446744073709551104ULL,		18446744073709550592ULL,		18446744073709549568ULL,
		18446744073709547520ULL,		18446744073709543424ULL,		18446744073709535232ULL,		18446744073709518848ULL,
		18446744073709486080ULL,		18446744073709420544ULL,		18446744073709289472ULL,		18446744073709027328ULL,
		18446744073708503040ULL,		18446744073707454464ULL,		18446744073705357312ULL,		18446744073701163008ULL,
		18446744073692774400ULL,		18446744073675997184ULL,		18446744073642442752ULL,		18446744073575333888ULL,
		18446744073441116160ULL,		18446744073172680704ULL,		18446744072635809792ULL,		18446744071562067968ULL,
		18446744069414584320ULL,		18446744065119617024ULL,		18446744056529682432ULL,		18446744039349813248ULL,
		18446744004990074880ULL,		18446743936270598144ULL,		18446743798831644672ULL,		18446743523953737728ULL,
		18446742974197923840ULL,		18446741874686296064ULL,		18446739675663040512ULL,		18446735277616529408ULL,
		18446726481523507200ULL,		18446708889337462784ULL,		18446673704965373952ULL,		18446603336221196288ULL,
		18446462598732840960ULL,		18446181123756130304ULL,		18445618173802708992ULL,		18444492273895866368ULL,
		18442240474082181120ULL,		18437736874454810624ULL,		18428729675200069632ULL,		18410715276690587648ULL,
		18374686479671623680ULL,		18302628885633695744ULL,		18158513697557839872ULL,		17870283321406128128ULL,
		17293822569102704640ULL,		16140901064495857664ULL,		13835058055282163712ULL,		9223372036854775808ULL,
	};

	const static UInt64 minusTable[] = {
		1ULL,						3ULL,						7ULL,						15ULL,
		31ULL,						63ULL,						127ULL,						255ULL,
		511ULL,						1023ULL,					2047ULL,					4095ULL,
		8191ULL,					16383ULL,					32767ULL,					65535ULL,
		131071ULL,					262143ULL,					524287ULL,					1048575ULL,
		2097151ULL,					4194303ULL,					8388607ULL,					16777215ULL,
		33554431ULL,				67108863ULL,				134217727ULL,				268435455ULL,
		536870911ULL,				1073741823ULL,				2147483647ULL,				4294967295ULL,
		8589934591ULL,				17179869183ULL,				34359738367ULL,				68719476735ULL,
		137438953471ULL,			274877906943ULL,			549755813887ULL,			1099511627775ULL,
		2199023255551ULL,			4398046511103ULL,			8796093022207ULL,			17592186044415ULL,
		35184372088831ULL,			70368744177663ULL,			140737488355327ULL,			281474976710655ULL,
		562949953421311ULL,			1125899906842623ULL,		2251799813685247ULL,		4503599627370495ULL,
		9007199254740991ULL,		18014398509481983ULL,		36028797018963967ULL,		72057594037927935ULL,
		144115188075855871ULL,		288230376151711743ULL,		576460752303423487ULL,		1152921504606846975ULL,
		2305843009213693951ULL,		4611686018427387903ULL,		9223372036854775807ULL,		18446744073709551615ULL,
	};


	Int32	maxvalue = xaxis ? m_width : m_height;
	auto&	maskbound = xaxis ? m_xBoundaryPoints : m_yBoundaryPoints;
	auto	variable = xaxis ? x : y;
	auto	nbitmask = xaxis ? m_xBoundaryPoints.__NBITMASK : m_yBoundaryPoints.__NBITMASK;
	if (forward)
	{
		auto	MAX = maskbound.GetWordWidths();
		for (int i = 0; i < MAX; ++i)
		{
			auto  pos = xaxis ? _GetPosX(x + i * nbitmask, y) : _GetPosY(x, y + i * nbitmask);
			if (pos == npos) return maxvalue;

			//auto  value = ~maskbound.GetValue(pos);
			auto  value = ~maskbound[pos / nbitmask];
			if (i == 0) value &= plusTable[(variable%nbitmask)];

			unsigned long index = 0;
			if (JPSCollision::_ffsll(index, value))
			{
				return (variable - (variable%nbitmask) + i * nbitmask) + index;
			}
		}
		return maxvalue;
	}
	else
	{
		auto	MAX = maskbound.GetWordWidths();
		for (int i = 0; i < MAX; ++i)
		{
			auto  pos = xaxis ? _GetPosX(x - i * nbitmask, y) : _GetPosY(x, y - i * nbitmask);
			if (pos == npos) return -1;

			//auto  value = ~maskbound.GetValue(pos);
			auto  value = ~maskbound[pos / nbitmask];
			if (i == 0) value &= minusTable[(variable%nbitmask)];
			
			unsigned long index = 0;
			if (JPSCollision::_flsll(index, value))
			{
				return (variable - (variable%nbitmask) - i * nbitmask) + index;
			}
		}
		return -1;
	}

}

Int32 JPSCollision::_GetPosX(Int32 x, Int32 y)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) return npos;
	return x + y * m_xBoundaryPoints.GetBitsWidths();
}

Int32 JPSCollision::_GetPosY(Int32 x, Int32 y)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) return npos;
	return x * m_yBoundaryPoints.GetBitsWidths() + y;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
// Code : _flsll
// Desc : _BitScanReverse64 과 동일한 코드 (Windows, Android, IOS 모두 동일한 코드를 위해)
//--------------------------------------------------------------------
bool JPSCollision::_flsll(unsigned long& index, UInt64 word)
{
	unsigned long num = 63;
	if (!word)
		return false;

	if (!(word & 0xffffffff00000000ULL)) {
		word <<= 32;
		num -= 32;
	}
	if (!(word & 0xffff000000000000ULL)) {
		word <<= 16;
		num -= 16;
	}
	if (!(word & 0xff00000000000000ULL)) {
		word <<= 8;
		num -= 8;
	}
	if (!(word & 0xf000000000000000ULL)) {
		word <<= 4;
		num -= 4;
	}
	if (!(word & 0xc000000000000000ULL)) {
		word <<= 2;
		num -= 2;
	}
	if (!(word & 0x8000000000000000ULL)) {
		word <<= 1;
		num -= 1;
	}
	index = num;
	return true;
}

//--------------------------------------------------------------------
// Code : _ffsll
// Desc : _BitScanForward64 과 동일한 코드 (Windows, Android, IOS 모두 동일한 코드를 위해)
//--------------------------------------------------------------------
bool JPSCollision::_ffsll(unsigned long& index, UInt64 word)
{
	unsigned long num = 0;
	if (!word)
		return false;
	if ((word & 0xffffffffULL) == 0) {
		num += 32;
		word >>= 32;
	}
	if ((word & 0xffffULL) == 0) {
		num += 16;
		word >>= 16;
	}
	if ((word & 0xffULL) == 0) {
		num += 8;
		word >>= 8;
	}
	if ((word & 0xfULL) == 0) {
		num += 4;
		word >>= 4;
	}
	if ((word & 0x3ULL) == 0) {
		num += 2;
		word >>= 2;
	}
	if ((word & 0x1ULL) == 0)
		num += 1;

	index = num;
	return true;
}

