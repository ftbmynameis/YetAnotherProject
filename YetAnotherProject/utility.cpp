#include "utility.hpp"

namespace util
{
	namespace math
	{
		float cot(float rad)
		{
			return std::cosf(rad) / std::sinf(rad);
		}

		float pi()
		{
			static auto pi_val = std::atanf(1.0f) * 4;
			return pi_val;
		}

		float pi2()
		{
			static auto pi_val = std::atanf(1.0f) * 2;
			return pi_val;
		}

		float pi4()
		{
			static auto pi_val = std::atanf(1.0f);
			return pi_val;
		}
	}
}