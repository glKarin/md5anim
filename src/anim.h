//
// Created by Administrator on 2024/2/2.
//

#ifndef _ANIM_H
#define _ANIM_H

#include "str.h"

#include "container.h"

class idLexer;
class idMd5MeshJoints;

class idMd5AnimBone
{
public:
    int index = 0;
    idStr name;
    int parentIndex = -1;

    const idMd5AnimBone *parent = nullptr;
    idList<const idMd5AnimBone *> children;

    idList<const idMd5AnimBone *> AllChildren() const;
    friend bool operator<(const idMd5AnimBone &a, const idMd5AnimBone &b);
    friend bool operator==(const idMd5AnimBone &a, const idMd5AnimBone &b);

    static idList<const idMd5AnimBone *> Filter(const idList<idMd5AnimBone> &total, idList<const idMd5AnimBone *> *list, const idStr &str);
    static idList<const idMd5AnimBone *> Filter(const idList<idMd5AnimBone> &total, const idList<idStr> &strs);
};

class idMd5AnimTransform
{
public:
    double xt = 0;
    double yt = 0;
    double zt = 0;
    double xr = 0;
    double yr = 0;
    double zr = 0;

    void Parse(const idStr &str);
    static idMd5AnimTransform From(const idStr &str);
    idStr Tostring(bool q) const;
};


class idMd5AnimHierarchy {
public:
    idList<idStr> joints;

    void Parse(idLexer &lexer, int numJoints);
    void Dump(idStr &sb);
    idList<idMd5AnimBone> ParseBone();
};

class idMd5AnimBounds {
public:
    idList<idStr> frames;

    void Parse(idLexer &lexer, int numFrames);
    void Dump(idStr &sb);
    void Reverse();
};

class idMd5AnimBaseFrame {
public:
    idList<idStr> joints;

    void Parse(idLexer &lexer, int numJoints);
    void Dump(idStr &sb);
};

class idMd5AnimFrame {
public:
    int index = 0;
    idList<idStr> joints;

    void Parse(idLexer &lexer, int numJoints);
    void Dump(idStr &sb);
    void Rotate(int index, const double *xr, const double *yr, const double *zr);
    void Translate(int index, const double *xt, const double *yt, const double *zt);
    void TranslateAbsolute(int index, const double *xt, const double *yt, const double *zt);
    void SetRotate(int index, const double *xr, const double *yr, const double *zr);
    void SetTranslate(int index, const double *xt, const double *yt, const double *zt);
};
using idMd5AnimFrameList = idList<idMd5AnimFrame>;

class idMd5AnimFile {
public:
    idStr MD5Version;
    idStr commandline;

    int numFrames = 0;
    int numJoints = 0;
    int frameRate = 0;
    int numAnimatedComponents = 0;

    idMd5AnimHierarchy hierarchy;
    idMd5AnimBounds bounds;
    idMd5AnimBaseFrame baseFrame;
    idMd5AnimFrameList frames;

    idMd5AnimBone *root = nullptr;
    idList<idMd5AnimBone> boneList;

    idMd5AnimFile() = default;
    idMd5AnimFile(const idMd5AnimFile &other);
    idMd5AnimFile(idMd5AnimFile &&other) = default;

    void ParseBone();
    void Parse(idLexer &lexer);
    void Dump(idStr &sb);
    idMd5AnimFile & operator=(const idMd5AnimFile &other);
    idMd5AnimFile Cut(int start, int count) const;
    idMd5AnimFile Reverse() const;

    static idMd5AnimFile Combine(const idMd5AnimFile &a, int aStart, const idMd5AnimFile &b, int bStart, int count, const idList<idStr> &bBones);
    static idMd5AnimFile Rotate(const idMd5AnimFile &a, const double *xr, const double *yr, const double *zr, const idList<idStr> &bBones);
    static idMd5AnimFile Translate(const idMd5AnimFile &a, const double *xt, const double *yt, const double *zt, const idList<idStr> &bBones);
    static idMd5AnimFile TranslateAbsolute(const idMd5AnimFile &a, const double *xt, const double *yt, const double *zt, const idList<idStr> &bBones);
    static idMd5AnimFile SetRotate(const idMd5AnimFile &a, const double *xr, const double *yr, const double *zr, const idList<idStr> &bBones);
    static idMd5AnimFile SetTranslate(const idMd5AnimFile &a, const double *xt, const double *yt, const double *zt, const idList<idStr> &bBones);

    void AppendBones(const idMd5MeshJoints &joints, const char *boneName);

private:
    void MakeChildren();
};

#endif //_ANIM_H
