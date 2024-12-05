//
// Created by Administrator on 2024/2/3.
//

#include "files.h"

#include <fstream>
#include <iostream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

size_t idFileSystem::Write(const idStr &data, const char *to)
{
    ofstream os(to);
    os << data;
    return data.length();
}

idStrList idFileSystem::Ls(const char *pathname)
{
    idStrList list;
    path p(pathname);
    auto entrys = directory_iterator(p);
    for(auto &itor : entrys)
    {
        list.push_back(itor.path().string());
    }
    return list;
}

bool idFileSystem::Mkdir(const char *pathname)
{
    path p(pathname);
    return create_directories(p);
}

idStr idFileSystem::Filename(const char *pathname)
{
    path p(pathname);
    return p.filename().string();
}

idStr idFileSystem::AppendPath(const idStrList &paths)
{
    path p;
    for (const auto &item: paths)
        p /= item.c_str();
    return p.string();
}