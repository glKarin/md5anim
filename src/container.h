//
// Created by Administrator on 2024/2/2.
//

#ifndef _LIST_H
#define _LIST_H

#include <vector>
#include <algorithm>

template <typename T>
class idList : public std::vector<T> {
public:
    using std::vector<T>::vector;

    bool contains(const T &t) const;
    void addAll(const idList<T> &others);

    idList & operator<<(const T &t) {
        this->push_back(t);
        return *this;
    }
};

template <typename T>
bool idList<T>::contains(const T &t) const
{
    return find(this->cbegin(), this->cend(), t) != this->cend();
}

template <typename T>
void idList<T>::addAll(const idList<T> &others)
{
    for_each(others.cbegin(), others.cend(), [this](const T &x) {
        this->push_back(x);
    });
}


#endif //_LIST_H
