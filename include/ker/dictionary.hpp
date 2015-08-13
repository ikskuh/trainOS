#pragma once

#include "pair.hpp"
#include "vector.hpp"

namespace ker
{
	template<typename Key, typename Value>
	class Dictionary
	{
	public:
		typedef Pair<Key, Value> Entry;
	private:
		Vector<Entry> contents;
	public:
		Dictionary() :
			contents()
		{

		}

		Value &at(const Key &key)
		{
			for(auto &&pair : this->contents)
			{
				if(pair.first == key) {
					return pair.second;
				}
			}
		}

		const Value &at(const Key &key) const
		{
			for(auto &&pair : this->contents)
			{
				if(pair.first == key) {
					return pair.second;
				}
			}
		}

		bool contains(const Key &key) const
        {
			for(const auto &pair : this->contents)
			{
				if(pair.first == key) {
					return true;
				}
			}
			return false;
		}

		void add(const Key &key, const Value &value)
        {
			if(this->contains(key)) {
				for(auto &&pair : this->contents)
				{
					if(pair.first == key) {
						pair.second = value;
						return;
					}
				}
			} else {
				this->contents.append(Entry(key, value));
			}
		}

		Value& operator [](const Key &key)
		{
			return this->at(key);
		}

		const Value& operator [](const Key &key) const
		{
			return this->at(key);
		}

		Entry * begin()
		{
			return this->contents.begin();
		}

		Entry * end()
		{
			return this->contents.end();
		}

		const Entry * begin() const
		{
			return this->contents.begin();
		}

		const Entry * end() const
		{
			return this->contents.end();
		}
	};
}
