#ifndef MATHDEF_H
#define MATHDEF_H

#include <array>
#include <cmath>
#include <valarray>

#include "export_lua.h"

#ifndef PI
constexpr float PI	   = 3.1415926f;
constexpr float TWO_PI = 2 * PI;
#endif

#include "RandomGet.h"

export_lua class Math
{
public:
	export_lua static inline bool isNaN(float f)
	{
		// std::isnan() is C99, not supported by all compilers
		// However NaN always fails this next test, no other number does.
		return f != f;
	}

	export_lua static inline bool is2Pow(unsigned int target) { return (!(target & (target - 1))); }

	export_lua static inline int get2Pow(unsigned int target)
	{
		int result = -1;
		if(is2Pow(target) == true)
		{
			while(target != 0)
			{
				target >>= 1;
				result++;
			}
		}
		return result;

		// return log10(target) / log10(2);
	}

	static inline bool FloatEqual(float a, float b, float tolerance = std::numeric_limits<float>::epsilon())
	{
		if(fabs(b - a) <= tolerance)
			return true;
		else
			return false;
	}

	/** Clamp a value within an inclusive range. */
	template<typename T>
	static T Clamp(T val, T minval, T maxval)
	{
		assert(minval < maxval && "Invalid clamp range");
		return std::max(std::min(val, maxval), minval);
	}

	template<size_t nTrigTableSize>
	class CTrigTable
	{
	public:
		CTrigTable()
		{
			for(int i = 0; i < nTrigTableSize; ++i)
			{
				float angle	  = TWO_PI * i / nTrigTableSize;
				m_SinTable[i] = sin(angle);
				m_TanTable[i] = tan(angle);
			}
		}

		float SinTable(float fValue)
		{
			int idx;
			if(fValue >= 0)
			{
				idx = int(fValue * m_TrigTableFactor) % nTrigTableSize;
			}
			else
			{
				idx = nTrigTableSize - (int(-fValue * m_TrigTableFactor) % nTrigTableSize) - 1;
			}

			return m_SinTable[idx];
		}

		float TanTable(float fValue)
		{
			// Convert range to index values, wrap if required
			int idx = int(fValue *= m_TrigTableFactor) % nTrigTableSize;
			return m_TanTable[idx];
		}

	private:
		static constexpr int			  m_TrigTableFactor = nTrigTableSize / TWO_PI;
		std::array<float, nTrigTableSize> m_SinTable;
		std::array<float, nTrigTableSize> m_TanTable;
	};
	const CTrigTable<360> TRIG_TABLE;
};
#endif /* MATHDEF_H */
