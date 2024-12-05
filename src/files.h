//
// Created by Administrator on 2024/2/3.
//

#ifndef _FILES_H
#define _FILES_H

#include <cstddef>

#include "str.h"

class idFileSystem {
public:
    static size_t Write(const idStr &data, const char *to);
    static idStrList Ls(const char *path);
    static bool Mkdir(const char *path);
    static idStr Filename(const char *path);
    static idStr AppendPath(const idStrList &paths);
};


#endif //_FILES_H
