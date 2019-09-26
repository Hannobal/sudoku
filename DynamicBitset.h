#ifndef DYNAMICBITSET_H_
#define DYNAMICBITSET_H_

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

// very similar to the std::bitset but with dynamic size

template <typename Storage=unsigned char>
class DynamicBitset {
public:
	DynamicBitset() :
		m_size(0),
		m_count(0) {};

	DynamicBitset(size_t s):
		m_size(s),
		m_count(0),
		m_bytes(s%(sizeof(Storage)*8)==0 ? s/(sizeof(Storage)*8) : s/(sizeof(Storage)*8)+1)
	{};

	// check if all bits are set
	bool all() {return m_count == m_size;}

	// check if at least one bit is set
	bool any() {return m_count > static_cast<size_t>(0);}

	// check if no bit is set
	bool none() {return m_count == static_cast<size_t>(0);}

	// returns the number of set bits
	constexpr size_t count() const noexcept {return m_count;}

	// returns the total number bits
	constexpr size_t size() const noexcept {return m_size;}

	// set all bits to true
	void set() noexcept
	{
		m_count = m_size;
		for(size_t i=0;i<m_bytes.size();i++)
			m_bytes[i]=~(static_cast<Storage>(0));
	}

	// sets a single bit (no boundary check!)
	void set(size_t pos, bool value = true)
	 {
		if((*this)[pos]==value)
			flip(pos);
	}

	// reset all bits to false
	void reset() noexcept
	 {
		m_count = static_cast<size_t>(0);
		for(size_t i=0;i<m_bytes.size();i++)
			m_bytes[i]=static_cast<Storage>(0);
	}

	// reset a single bit to false (no boundary check!)
	void reset(size_t pos)
	 {
		if((*this)[pos]) {
			m_count--;
			m_bytes[pos/(sizeof(Storage)*8)] &=
					~(static_cast<Storage>(1) << pos%(sizeof(Storage)*8));
		}
	}

	// flips all bits
	void flip() noexcept
	{
		for(size_t i=0;i<m_bytes.size();i++)
			m_bytes[i] ^= ~(static_cast<Storage>(0));
		m_count = m_size - m_count;
	}

	// flip a single bit (no boundary check!)
	void flip(size_t pos)
	 {
		if((*this)[pos])
			m_count--;
		else
			m_count++;
		m_bytes[pos/(sizeof(Storage)*8)] ^=
				(static_cast<size_t>(1) << pos%(sizeof(Storage)*8));
	}

	// access a bit (no boundary check!)
	bool operator[]( std::size_t pos ) const
	{
		return (m_bytes[pos/(sizeof(Storage)*8)] >>
				pos%(sizeof(Storage)*8)) & static_cast<size_t>(1);
	}

	// check if a bit is set
	bool test( size_t pos ) const
	{
		return m_bytes[pos/(sizeof(Storage)*8)] &
				(static_cast<size_t>(1) << pos%(sizeof(Storage)*8));
	}

	// returns a string of all bits
	template<
	    class CharT = char,
	    class Traits = std::char_traits<CharT>,
	    class Allocator = std::allocator<CharT>
	>
	std::basic_string<CharT,Traits,Allocator>
	    to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const {
		std::stringstream strm;
		for(size_t i=0;i<size();i++)
			strm << ((*this)[i] ? one : zero);
		return strm.str();
	}

	// not yet implemented
	unsigned long to_ulong() const;
	// not yet implemented
	unsigned long long to_ullong() const;

	void resize(size_t s) {
		size_t originalSize=m_bytes.size();
		m_bytes.resize(s%(sizeof(Storage)*8)==0 ? s/(sizeof(Storage)*8) : s/(sizeof(Storage)*8)+1);
		if(s>m_size) {
			size_t endBit=8*sizeof(Storage)*originalSize;
			for(size_t i=m_size; i<endBit; i++)
				m_bytes[i/(sizeof(Storage)*8)] &=
						~(static_cast<Storage>(1) << i%(sizeof(Storage)*8));
		} else {
			for(size_t i=s; i<m_size; i++)
				if((*this)[i]) m_count--;
		}
		m_size=s;
	}

	// returns a vector of all bits that are (un)set
	void getIndices(std::vector<size_t>& indices, bool isSet=true) {
		indices.resize(isSet ? count() : size()-count());
		size_t j=0;
		for(size_t i=0;i<size();i++) {
			if((*this)[i] == isSet)
				indices[j++] = i;
		}

	}


private:

	size_t m_size; // how many bits are available
	size_t m_count; // how many bits are currently set
	std::vector<Storage> m_bytes;

};

#endif /* DYNAMICBITSET_H_ */
