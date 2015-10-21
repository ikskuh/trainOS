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

    template<typename T1, typename T2>
    static inline bool operator == (const Pair<T1,T2> &lhs, const Pair<T1,T2> &rhs)
    {
        return (lhs.first == rhs.first) && (lhs.second == rhs.second);
    }

    template<typename T1, typename T2>
    static inline bool operator != (const Pair<T1,T2> &lhs, const Pair<T1,T2> &rhs)
    {
        return (lhs.first != rhs.first) || (lhs.second != rhs.second);
    }
}
