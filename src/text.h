//
// Created by Administrator on 2024/2/2.
//

#ifndef _TEXT_H
#define _TEXT_H

#include <fstream>
#include "str.h"

class idVec3;

class idLexer
{
public:
    void Parse(const idStr &path);
    virtual ~idLexer();

    idStr ReadLine();
    void Close();

    static idStr Skip(const idStr &s, const idStr &word);
    static idStr GetValue(const idStr &str, const idStr &word);
    static idStr GetToken(const idStr &str, idStr *pos = nullptr, int start = 0);
    static int GetInteger(const idStr &str, idStr *pos = nullptr, int start = 0);
    static float GetFloat(const idStr &str, idStr *pos = nullptr, int start = 0);
    static double GetDouble(const idStr &str, idStr *pos = nullptr, int start = 0);
    static idVec3 GetVec3(const idStr &str, bool b = true, idStr *pos = nullptr, int start = 0);

private:
    std::ifstream is;
};


#endif //_TEXT_H
