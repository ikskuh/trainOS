#pragma once

#include <kernel.h>

#include "pair.hpp"
#include "vector.hpp"

namespace ker
{
	template<typename Key, typename Value>
	class Dictionary
	{
	public:
		typedef Pair<Key, Value> Entry;
		static Value _default;
	public:
		Vector<Entry> contents;
	public:
		Dictionary() :
			contents()
		{

		}

        Dictionary(const Dictionary &other) :
            contents(other.contents)
        {

        }

        Dictionary(Dictionary &&other) :
            contents(other.contents)
        {
            other.contents = Vector<Entry>();
        }

        Dictionary & operator = (const Dictionary &other)
        {
            this->contents = other.contents;
            return *this;
        }

		Value &at(const Key &key)
		{
			for(auto &&pair : this->contents)
			{
				if(pair.first == key) {
					return pair.second;
				}
			}
			die("Key not found in dictionary.");
			return _default;
		}

		const Value &at(const Key &key) const
		{
			static Value _default;
			for(auto &&pair : this->contents)
			{
				if(pair.first == key) {
					return pair.second;
				}
			}
			die("Key not found in dictionary.");
			return _default;
		}

        Value get(const Key &key) const
        {
            if(this->contains(key)) {
                return this->at(key);
            } else {
                return Value();
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

	template<typename Key, typename Value>
	Value Dictionary<Key, Value>::_default = Value();
}
