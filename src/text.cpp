//
// Created by Administrator on 2024/2/2.
//

#include "text.h"
#include <iostream>
#include "geomath.h"

using namespace std;

idLexer::~idLexer()
{
    Close();
}

void idLexer::Parse(const idStr &path)
{
    Close();
    is.open(path);
}

void idLexer::Close()
{
    if(is.is_open())
        is.close();
}

idStr idLexer::ReadLine()
{
    idStr line;
    while(!is.eof())
    {
        getline(is, line);
		if(line.empty())
			continue;
		const char *ptr = line.c_str();
		while(*ptr)
		{
			if(!isspace(*ptr))
            return line;
			ptr++;
		}
    }
    return "";
}

idStr idLexer::Skip(const idStr &s, const idStr &word)
{
    idStr str = s.trim();
    if(str.startsWith(word))
        return str.substr(word.length());
    return "";
}

idStr idLexer::GetValue(const idStr &str, const idStr &word)
{
    idStr s = Skip(str, word);
    s.trimSelf();
    auto i = s.find(' ');
    return i != string::npos ? s.substr(0, i) : s;
}

idStr idLexer::GetToken(const idStr &str, idStr *pos, int start)
{
    idStr t = str.substr(start);
    t.trimSelf();
    auto i = t.find(' ');
    if(pos)
        *pos = i != string::npos ? t.substr(i) : "";
    return i != string::npos ? t.substr(0, i) : t;
}

int idLexer::GetInteger(const idStr &str, idStr *pos, int start)
{
    idStr t = GetToken(str, pos, start);
    return ::atoi(t.c_str());
}

float idLexer::GetFloat(const idStr &str, idStr *pos, int start)
{
    idStr t = GetToken(str, pos, start);
    return ::strtof(t.c_str(), nullptr);
}

double idLexer::GetDouble(const idStr &str, idStr *pos, int start)
{
    idStr t = GetToken(str, pos, start);
    return ::strtod(t.c_str(), nullptr);
}

idVec3 idLexer::GetVec3(const idStr &str, bool b, idStr *pos, int start)
{
    idStr text = str;
    if(b)
        text = Skip(text, "(");

    idStr p = text;
    idVec3 v;
    v.x = GetDouble(p, &p);
    v.y = GetDouble(p, &p);
    v.z = GetDouble(p, &p);

    if(b)
        p = Skip(text, ")");

    if(pos)
        *pos = p;

    return v;
}