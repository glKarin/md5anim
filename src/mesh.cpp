#include "mesh.h"
#include <iostream>
#include <cstdlib>

#include "text.h"

using namespace std;

void idMd5MeshJoint::Parse(const idStr &line)
{
    auto str = line.trim();
    auto i = str.find('"');
    auto end = str.find('"', i + 1);
    name = str.substr(i + 1, end - i - 1);

    str = idStr::Trim(str.substr(end + 1));
    end = str.find('(');
    auto pi = idStr::Trim(str.substr(0, end));
    parentIndex = stoi(pi);

    transform = idStr::Trim(str.substr(end));
}

void idMd5MeshJoint::SetRotation(const idStr &rot)
{
    auto pos = transform.find(')');
    transform = transform.substr(0, pos + 1) + " ( " + rot + " )";
}

idStr idMd5MeshJoint::Numbers() const
{
    idStr tr = transform;
    tr.Replace('(', ' ')
            .Replace(')', ' ');
    return tr.trim();
}

void idMd5MeshJoints::Parse(idLexer &lexer, int numJoints)
{
    lexer.ReadLine();
    joints.clear();
    for(int i = 0; i < numJoints; i++)
    {
        idStr str = lexer.ReadLine();
        idMd5MeshJoint joint;
        joint.Parse(str);
        joint.index = i;
        joints.push_back(joint);
    }
    lexer.ReadLine();
}

void idMd5MeshFile::Parse(idLexer &lexer) {
    MD5Version = idLexer::GetValue(lexer.ReadLine(), "MD5Version ");
    lexer.ReadLine();

    numJoints = stoi(idLexer::GetValue(lexer.ReadLine(), "numJoints "));
    numMeshes = stoi(idLexer::GetValue(lexer.ReadLine(), "numMeshes "));

    joints.Parse(lexer, numJoints);

    for(int i = 0; i < numMeshes; i++)
    {
        idMd5Mesh mesh;
        mesh.Parse(lexer);
        meshes.push_back(mesh);
    }
}

void idMd5MeshWeight::Parse(idLexer &lexer) {
    idStr line = lexer.ReadLine();
    line = idLexer::Skip(line, "weight");
    idStr p;
    weightIndex = idLexer::GetInteger(line, &p);
    jointIndex = idLexer::GetInteger(p, &p);
    weightValue = idLexer::GetToken(p, &p);
    pos = idLexer::GetVec3(p, true, &p);
#if 0
    //idVec3 tr(1.66727, 0.000062, 65.813);
    jointIndex = 225;
    //Translate(tr);
    line = "";
    Dump(line);
    cout << "  " << line << endl;
#endif
}

void idMd5Mesh::Parse(idLexer &lexer) {
    int i;
    idStr line = lexer.ReadLine();
    idLexer::Skip(line, "mesh");
    idLexer::Skip(line, "{");

    line = lexer.ReadLine();
    shader = idLexer::GetValue(line, "shader");

    line = lexer.ReadLine();
    numverts = idLexer::GetValue(line, "numverts").ToInteger();
    for(i = 0; i < numverts; i++)
    {
        line = lexer.ReadLine();
        idMd5MeshVert vert;
        vert.vert = line;
        verts.push_back(vert);
    }

    line = lexer.ReadLine();
    numtris = idLexer::GetValue(line, "numtris").ToInteger();
    for(i = 0; i < numtris; i++)
    {
        line = lexer.ReadLine();
        idMd5MeshTri tri;
        tri.tri = line;
        tris.push_back(tri);
    }

    line = lexer.ReadLine();
    numweights = idLexer::GetValue(line, "numweights").ToInteger();
    for(i = 0; i < numweights; i++)
    {
        idMd5MeshWeight weight;
        weight.Parse(lexer);
        weights.push_back(weight);
    }
}

void idMd5MeshWeight::Translate(const idVec3 &t)
{
    pos += t;
}

void idMd5MeshWeight::Dump(idStr &out) const
{
    out += "weight ";
    out += weightIndex;
    out += " ";
    out += jointIndex;
    out += " ";
    out += weightValue;
    out += " ";
    out += "( ";
    out += pos.x;
    out += " ";
    out += pos.y;
    out += " ";
    out += pos.z;
    out += " )";
}