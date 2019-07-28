#ifndef UTILITY_H_
#define UTILITY_H_

#include <type_traits>

// Helper to determine whether there's a const_iterator for T.
template<typename T>
struct has_const_iterator
{
private:
    template<typename C> static char test(typename C::const_iterator*);
    template<typename C> static int  test(...);
public:
    enum { value = sizeof(test<T>(0)) == sizeof(char) };
};


// check if a container has a certain value
template <typename Container>
typename std::enable_if<has_const_iterator<Container>::value, bool>::type
contains(Container const& c, typename Container::value_type const& t)
{
	for(typename Container::const_iterator it(c.begin()); it != c.end(); it++) {
		if(*it == t) return true;
	}
	return false;
}


#endif /* UTILITY_H_ */
