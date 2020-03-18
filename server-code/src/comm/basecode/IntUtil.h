#ifndef INTUTIL_H
#define INTUTIL_H

#include "CheckUtil.h"

#define UNUSED(var) (void)((var) = (var))

export_lua inline int64_t MakeINT64(int32_t left, int32_t right)
{
	return static_cast<int64_t>(left) << 32 | static_cast<int64_t>(right);
}

export_lua inline uint64_t MakeUINT64(uint32_t left, uint32_t right)
{
	return static_cast<uint64_t>(left) << 32 | static_cast<uint64_t>(right);
}


// a*b/c
export_lua int32_t	MulDivSign(int32_t a, int32_t b, int32_t c);
export_lua uint32_t MulDiv(uint32_t a, uint32_t b, uint32_t c);

export_lua inline uint32_t MAKE32(uint32_t a, uint32_t b)
{
	return (a << 16) | (b & 0xFFFF);
}

export_lua inline uint64_t MAKE64(uint64_t a, uint64_t b)
{
	return (a << 32) | (b & 0xFFFFFFFF);
}

export_lua inline uint32_t GetHighFromU64(uint64_t v)
{
	return (v >> 32) & 0xFFFFFFFF;
}

export_lua inline uint32_t GetLowFromU64(uint64_t v)
{
	return (v)&0xFFFFFFFF;
}


export_lua inline int32_t MulDivSign(int32_t a, int32_t b, int32_t c)
{
	return ((int32_t)(((int64_t)(a) * (int64_t)(b)) / (int64_t)(c)));
}

export_lua inline uint32_t MulDiv(uint32_t a, uint32_t b, uint32_t c)
{
	return ((uint32_t)(((uint64_t)(a) * (uint64_t)(b)) / (uint64_t)(c)));
}

export_lua inline uint32_t hex_set(uint32_t dwFlag, uint8_t nHex, uint8_t ucVal)
{
	CHECKF(nHex < 8);
	return (dwFlag & ~(0xF << (nHex * 4))) | ((ucVal & 0xF) << (nHex * 4));
}

export_lua inline uint8_t hex_get(uint32_t dwFlag, uint8_t nHex)
{
	CHECKF(nHex < 8);
	return (dwFlag >> (nHex * 4)) & 0xF;
}

export_lua inline bool bit_test(uint32_t dwFlag, uint8_t nBit)
{
	CHECKF(nBit < 32);
	return (dwFlag & ((uint32_t)1 << nBit)) != 0;
}

export_lua inline uint32_t bit_flip(uint32_t dwFlag, uint8_t nBit)
{
	CHECKF(nBit < 32);
	return dwFlag ^ ((uint32_t)1 << nBit);
}

export_lua inline uint32_t bit_set(uint32_t dwFlag, uint8_t nBit, bool bVal)
{
	CHECKF(nBit < 32);
	if(bVal)
		return dwFlag | (uint32_t)1 << nBit;
	else
		return dwFlag & ~((uint32_t)1 << nBit);
}



export_lua inline BYTE toHex(const BYTE& x)
{
	return x > 9 ? x + 55 : x + 48;
}

export_lua inline bool HasFlag(uint32_t flag, uint32_t mask)
{
	return (flag & mask) != 0;
}

#endif /* INTUTIL_H */
