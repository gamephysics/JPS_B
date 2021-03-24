#pragma once

//=============================================================================
// TYPEDEFS
//=============================================================================
typedef		uint64_t		UInt64;
typedef		int64_t			Int64;
typedef		uint32_t		UInt32;
typedef		int32_t			Int32;
typedef		uint16_t		UInt16;
typedef		int16_t			Int16;

namespace core
{
	template<typename _Ty>
	_Ty clamp(const _Ty& val, const _Ty& min, const _Ty& max)
	{
		return std::max(min, std::min(max, val));
	}

	template < class _Ty >
	bool InBoundsEx(_Ty const& v, _Ty Min, _Ty Max)
	{
		if (Min > Max) { _Ty T = Min; Min = Max; Max = T; }
		return v<Min ? false : v>Max ? false : true;
	}
}
