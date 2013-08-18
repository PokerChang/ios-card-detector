//
//  contains.h
//  code copied from
// http://stackoverflow.com/questions/1701067/how-to-check-that-an-element-is-in-a-stdset
//

#ifndef __CardExtractor__contains__
#define __CardExtractor__contains__

#include <iostream>
#include <algorithm>
#include <iterator>
#include <set>
using namespace std;

template<class TInputIterator, class T> inline
bool contains(TInputIterator first, TInputIterator last, const T& value);

template<class TContainer, class T> inline
bool contains(const TContainer& container, const T& value);

template<class T> inline
bool contains(const std::set<T>& container, const T& value);


#endif /* defined(__CardExtractor__contains__) */

