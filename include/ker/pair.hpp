#pragma once

namespace ker
{
	template<typename First, typename Second>
	struct Pair
	{
		First first;
		Second second;

		Pair() : first(), second() { }

		Pair(const First &first, const Second &second) :
			first(first),
			second(second)
		{

		}
	};
}
