#include "utility.hpp"

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