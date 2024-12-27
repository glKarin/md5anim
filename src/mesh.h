#ifndef _MESH_H
#define _MESH_H

#include "str.h"

#include "container.h"
#include "geomath.h"

class idLexer;

class idMd5MeshJoint
{
public:
    idStr name;
    int parentIndex = -1;
    int index = 0;
    idStr translation = "0 0 0"; // Absolution
    idStr rotation = "0 0 0"; // Absolution

    void Parse(const idStr &line);
    idStr Numbers() const;
    void Dump(idStr &str) const;

    void SetTranslation(const idVec3 &trans);
    idVec3 TranslationVector() const;

    void SetRotation(const idVec3 &trans);
    idVec3 RotationVector() const;
    idStr Transform() const;
};

class idMd5MeshJoints {
public:
    idList<idMd5MeshJoint> joints;

    void Parse(idLexer &lexer, int numJoints);
    void Dump(idStr &out) const;
};

class idMd5MeshVert
{
public:
    idStr vert;
    void Dump(idStr &out) const;
};

class idMd5MeshTri
{
public:
    idStr tri;
    void Dump(idStr &out) const;
};

class idMd5MeshWeight
{
public:
    idStr weight;
    int weightIndex;
    int jointIndex;
    idStr weightValue;
    idStr pos;

    void Translate(const idVec3 &t);
    void Rotate(const idVec3 &r);

    void Parse(idLexer &lexer);
    void Dump(idStr &out) const;
};

class idMd5Mesh {
public:
    idStr shader;
    int numverts;
    idList<idMd5MeshVert> verts;
    int numtris;
    idList<idMd5MeshTri> tris;
    int numweights;
    idList<idMd5MeshWeight> weights;

    void Parse(idLexer &lexer);
    void Dump(idStr &out) const;
};

class idMd5MeshFile
{
public:
    idStr MD5Version;
    idStr commandline;
    int numJoints = 0;
    int numMeshes = 0;

    idMd5MeshJoints joints;
    idList<idMd5Mesh> meshes;

    void Parse(idLexer &lexer);
    void Dump(idStr &str) const;
    void Append(const idMd5MeshFile &joints, const char *boneName);
};

#endif //_MESH_H
