//
// Created by Administrator on 2024/2/2.
//

#include "str.h"

using namespace std;

idStr::idStr(const string &str)
:string(str)
{
}

idStr::idStr(string &&str)
        :string(str)
{
}

bool idStr::startsWith(const string &target)
{
    return find(target) == 0;
}

idStr & idStr::trimSelf()
{
    TrimSelf(*this);
    return *this;
}

idStr idStr::trim() const
{
    return Trim(*this);
}

std::string idStr::Trim(const std::string &str)
{
    string s(str);
    while(!s.empty() && isspace(s.front()))
        s.erase(0, 1);
    while(!s.empty() && isspace(s.back()))
        s.pop_back();
    return s;
}

std::string & idStr::TrimSelf(std::string &str)
{
    while(!str.empty() && isspace(str.front()))
        str.erase(0, 1);
    while(!str.empty() && isspace(str.back()))
        str.pop_back();
    return str;
}

idStr idStr::dtostr(double d, int p)
{
    std::stringstream is;
    is << std::fixed << std::setprecision(p);
    is << d;
    return is.str();
}

idStr & idStr::ToLower()
{
    for (auto &ch : *this)
    {
        if(ch >= 'A' && ch <= 'Z')
            ch += ('a' - 'A');
    }
    return *this;
}

idStr & idStr::ToUpper()
{
    for (auto &ch : *this)
    {
        if(ch >= 'a' && ch <= 'z')
            ch -= ('a' - 'A');
    }
    return *this;
}

idStr & idStr::Replace(char from, char to)
{
    std::replace(begin(), end(), from, to);
    return *this;
}

idStr idStr::Format(const char *format, ...)
{
    va_list va;
    char str[4096];

            va_start(va, format);
    std::vsnprintf(str, sizeof(str), format, va);
            va_end(va);

    return str;
}

idStr & idStr::Append(double d, int p)
{
    std::stringstream is;
    is << std::fixed << std::setprecision(p);
    is << d;
    const auto &str = is.str();
    append(str);
    return *this;
}

idStr & idStr::operator+=(int i)
{
    std::stringstream is;
    is << i;
    const auto &str = is.str();
    append(str);
    return *this;
}

idStr idStr::dtostr(const idList<double> list, int p)
{
    idStr str;
    int i = 0;

    for (const auto &item: list)
    {
        str.Append(item, p);
        if(i < list.size() - 1)
            str += " ";
        i++;
    }

    return str;
}