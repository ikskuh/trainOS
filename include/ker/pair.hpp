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

        Pair(const Pair &other) :
            first(other.first),
            second(other.second)
        {

        }

        Pair(Pair &&other) :
            first(other.first),
            second(other.second)
        {
            other.first = First();
            other.second = Second();
        }

        Pair & operator = (const Pair &other)
        {
            this->first = other.first;
            this->second = other.second;
            return *this;
        }
	};
}
