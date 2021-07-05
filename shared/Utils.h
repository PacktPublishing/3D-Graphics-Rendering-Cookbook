#pragma once

#if !defined(_CRT_SECURE_NO_WARNINGS)
#	define _CRT_SECURE_NO_WARNINGS 1
#endif // _CRT_SECURE_NO_WARNINGS

#include <malloc.h>
#include <string.h>
#include <algorithm>
#include <string>
#include <vector>

int endsWith(const char* s, const char* part);

std::string readShaderFile(const char* fileName);

void printShaderSource(const char* text);

template <typename T>
inline void mergeVectors(std::vector<T>& v1, const std::vector<T>& v2)
{
	v1.insert( v1.end(), v2.begin(), v2.end() );
}

inline int addUnique(std::vector<std::string>& files, const std::string& file)
{
	if (file.empty())
		return -1;

	auto i = std::find(std::begin(files), std::end(files), file);

	if (i == files.end())
	{
		files.push_back(file);
		return (int)files.size() - 1;
	}

	return (int)std::distance(files.begin(), i);
}

// From https://stackoverflow.com/a/64152990/1182653
// Delete a list of items from std::vector with indices in 'selection'
template <class T, class Index = int> inline void eraseSelected(std::vector<T>& v, const std::vector<Index>& selection)
// e.g., eraseSelected({1, 2, 3, 4, 5}, {1, 3})  ->   {1, 3, 5}
//                         ^     ^    2 and 4 get deleted
{
	// cut off the elements moved to the end of the vector by std::stable_partition
	v.resize(std::distance(
			v.begin(),
			// the stable_partition moves any element whose index is in 'selection' to the end
			std::stable_partition(v.begin(), v.end(),
				[&selection, &v](const T& item) {
					return !std::binary_search(
						selection.begin(), selection.end(),
						/* std::distance(std::find(v.begin(), v.end(), item), v.begin()) - if you don't like the pointer arithmetic below */
						static_cast<Index>(static_cast<const T*>(&item) - &v[0]));
				})));
}
