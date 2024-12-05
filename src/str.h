//
// Created by Administrator on 2024/2/2.
//

#ifndef _STR_H
#define _STR_H

#include <string>
#include <sstream>
#include <cstdarg>
#include <cstdio>
#include <algorithm>
#include <iomanip>

#include "container.h"

class idStr : public std::string
{
public:
    using std::string::string;
    idStr(const std::string &str);
    idStr(std::string &&str);
    idStr() = default;

    bool startsWith(const std::string &target);
    idStr & trimSelf();
    idStr trim() const;
    static std::string Trim(const std::string &str);
    static std::string & TrimSelf(std::string &str);

    operator const char *()
    {
        return c_str();
    }

    friend int stoi(const idStr &str)
    {
        return std::stoi(static_cast<const std::string &>(str));
    }

    using std::string::operator+=;
    idStr & operator+=(int i);
    idStr & operator+=(double d)
    {
        return Append(d);
    }

    idStr & Append(double d, int p = 10);
    static idStr Format(const char *format, ...);
    idStr & Replace(char from, char to);
    idStr & ToLower();
    idStr & ToUpper();

    int ToInteger()
    {
        return ::atoi(c_str());
    }

    static idStr dtostr(double d, int p = 10);

    static idStr dtostr(const idList<double> list, int p = 10);
};

typedef idList<idStr> idStrList;

#endif //_STR_H
