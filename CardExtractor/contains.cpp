//
//  contains.cpp
//  CardExtractor
//
//  Created by Young Ng on 8/4/13.
//  Copyright (c) 2013 Young Ng. All rights reserved.
//

#include "contains.h"

template<class TInputIterator, class T> inline
bool contains(TInputIterator first, TInputIterator last, const T& value)
{
    return std::find(first, last, value) != last;
}

template<class TContainer, class T> inline
bool contains(const TContainer& container, const T& value)
{
    // This works with more containers but requires std::begin and std::end
    // from C++0x, which you can get either:
    //  1. By using a C++0x compiler or
    //  2. Including the utility functions below.
    return contains(begin(container), end(container), value);
    
    // This works pre-C++0x (and without the utility functions below, but doesn't
    // work for fixed-length arrays.
    //return contains(container.begin(), container.end(), value);
}

template<class T> inline
bool contains(const std::set<T>& container, const T& value)
{
    return container.find(value) != container.end();
}