#include <iostream>
#include <algorithm>

template <class T>
void print(T begin, T end)
{
	for (; begin != end; ++begin)
	{
		std::cout << *begin << " ";
	}
	std::cout << std::endl;
}
