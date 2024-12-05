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
    idStr transform = "(0 0 0) (0 0 0)"; // Absolution

    void Parse(const idStr &line);
    void SetRotation(const idStr &rot);
    idStr Numbers() const;
};

class idMd5MeshJoints {
public:
    idList<idMd5MeshJoint> joints;

    void Parse(idLexer &lexer, int numJoints);
};

class idMd5MeshVert
{
public:
    idStr vert;
};

class idMd5MeshTri
{
public:
    idStr tri;
};

class idMd5MeshWeight
{
public:
    idStr weight;
    int weightIndex;
    int jointIndex;
    idStr weightValue;
    idVec3 pos;

    void Translate(const idVec3 &t);

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
};

#endif //_MESH_H
