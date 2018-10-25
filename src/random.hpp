#pragma once

namespace random
{
	inline int fastrand()
	{
		static unsigned int g_seed = static_cast<unsigned int>(time(nullptr));
		g_seed = (214013 * g_seed + 2531011);
		return (g_seed >> 16) & 0x7FFF;
	}
}
