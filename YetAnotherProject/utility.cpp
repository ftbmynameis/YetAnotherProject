#include "utility.hpp"

#include <cmath>

namespace util
{
	namespace math
	{
		float cot(float rad)
		{
			return std::cosf(rad) / std::sinf(rad);
		}
	}
}