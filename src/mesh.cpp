#include "mesh.h"
#include <iostream>
#include <cstdlib>
#include <unordered_map>

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

    idStr transform = idStr::Trim(str.substr(end));
    auto startIndex = transform.find("(");
    auto endIndex = transform.find(")", startIndex);
    translation = idStr::Trim(transform.substr(startIndex + 1, endIndex - startIndex - 1));

    startIndex = transform.rfind("(");
    endIndex = transform.rfind(")");
    rotation = idStr::Trim(transform.substr(startIndex + 1, endIndex - startIndex - 1));
}

void idMd5MeshJoint::SetTranslation(const idVec3 &trans)
{
    translation = trans.ToString();
}

idVec3 idMd5MeshJoint::TranslationVector() const
{
    return idVec3{translation.c_str()};
}

void idMd5MeshJoint::Dump(idStr &str) const
{
    str.append("\"").append(name).append("\"").append(" ").append(idStr::itostr(parentIndex)).append(" ").append(Transform());
}

void idMd5MeshJoint::SetRotation(const idVec3 &trans)
{
    rotation = trans.ToString();
}

idVec3 idMd5MeshJoint::RotationVector() const
{
    return idVec3{rotation.c_str()};
}

idStr idMd5MeshJoint::Numbers() const
{
    return translation + " " + rotation;
}

idStr idMd5MeshJoint::Transform() const
{
    return idStr("( ").append(translation) + " ) ( " + rotation + " )";
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

void idMd5MeshJoints::Dump(idStr &out) const
{
    out.append("joints {\n");

    std::unordered_map<int, const idStr &> boneMap;
    std::for_each(joints.begin(), joints.end(), [&boneMap](const idMd5MeshJoint &x) {
        boneMap.insert({x.index, x.name});
    });

    for (const auto &item: joints)
    {
        out.append("  ");
        idStr str;
        item.Dump(str);
        if(string::size_type index; (index = str.find("//")) != string::npos)
        {
            str = str.substr(0, index);
            str.trimSelf();
        }
        out.append(str);

        out.append(" // ").append(idStr::itostr(item.index));
        auto itor = boneMap.find(item.parentIndex);
        if(itor != boneMap.end())
            out.append(" ").append(itor->second);
        out.append("\n");
    }

    out.append("}");
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
    pos = idLexer::GetQuota(p, true, &p);
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
    line = lexer.ReadLine();
    idLexer::Skip(line, "}");
}

void idMd5MeshWeight::Translate(const idVec3 &t)
{
    idVec3 v = idLexer::GetVec3(pos);
    v += t;
    pos = v.ToString();
}

void idMd5MeshWeight::Rotate(const idVec3 &r)
{
    idVec3 v = idLexer::GetVec3(pos);
    v.Rotate(r);
    pos = v.ToString();
}

void idMd5MeshWeight::Dump(idStr &out) const
{
    out += "  weight ";
    out += weightIndex;
    out += " ";
    out += jointIndex;
    out += " ";
    out += weightValue;
    out += " ";
    out += "( ";
    out += pos;
    out += " )";
}

void idMd5MeshVert::Dump(idStr &out) const
{
    out.append(vert);
}

void idMd5MeshTri::Dump(idStr &out) const
{
    out.append(tri);
}

void idMd5Mesh::Dump(idStr &out) const
{
    out.append("mesh {\n");

    out.append("  ").append("shader ").append(shader).append("\n");
    out.append("\n");

    out.append("  ").append("numverts ").append(idStr::itostr(numverts)).append("\n");
    for (const auto &item: verts)
    {
        item.Dump(out);
        out.append("\n");
    }
    out.append("\n");

    out.append("  ").append("numtris ").append(idStr::itostr(numtris)).append("\n");
    for (const auto &item: tris)
    {
        item.Dump(out);
        out.append("\n");
    }
    out.append("\n");

    out.append("  ").append("numweights ").append(idStr::itostr(numweights)).append("\n");
    for (const auto &item: weights)
    {
        item.Dump(out);
        out.append("\n");
    }
    out.append("\n");

    out.append("}");
}

void idMd5MeshFile::Dump(idStr &str) const
{
    str.append("MD5Version ").append(MD5Version).append("\n");
    str.append("commandline \"").append(commandline).append("\"\n");
    str.append("\n");
    str.append("numJoints ").append(idStr::itostr(numJoints)).append("\n");
    str.append("numMeshes ").append(idStr::itostr(numMeshes)).append("\n");
    str.append("\n");

    joints.Dump(str);
    str.append("\n");
    str.append("\n");

    for (const auto &item: meshes)
    {
        item.Dump(str);
        str.append("\n");
        str.append("\n");
    }
}

void idMd5MeshFile::Append(const idMd5MeshFile &meshFile, const char *boneName)
{
    auto &myJoints = joints.joints;
    auto &otherJoints = meshFile.joints.joints;

    const auto &itor = std::find_if(myJoints.begin(), myJoints.end(), [boneName](const auto &x) {
        return x.name == boneName;
    });
    if(itor == myJoints.cend())
        throw idStr("bone not exists: ") + boneName;

    const idMd5MeshJoint &bone = *itor;
    int numBone = myJoints.size();
    int i = 0;
    int offsetIndex = bone.index;
    idVec3 offsetPos = bone.TranslationVector();
    idVec3 offsetRot = bone.RotationVector();

    for (const auto &joint : otherJoints)
    {
        idStr bname = joint.name;
        bname.ToLower();
        //std::cout << bname.c_str() << std::endl;
        bname.Replace('-', '_');
        const int BoneIndex = numBone + i;

        const int ParentBoneIndex = i == 0 ? offsetIndex : (numBone + i - 1);

        idMd5MeshJoint j = joint;
        j.index = BoneIndex;
        j.parentIndex = ParentBoneIndex;
        j.SetTranslation(offsetPos + joint.TranslationVector());
        idVec3 rot = offsetRot;
        rot.Rotate(joint.RotationVector());
        j.SetRotation(rot);

        myJoints.push_back(j);

        i++;
    }
    this->numJoints += i;

    for(i = 0; i < meshFile.numMeshes; i++)
    {
        auto mesh = meshFile.meshes[i];
        for(int m = 0; m < mesh.numweights; m++)
        {
            auto &weight = mesh.weights[m];
            /*if(weight.jointIndex == 0)
            {
                //cout << otherJoints[0].RotationVector().ToString() << endl;
                weight.Rotate(otherJoints[0].RotationVector());
            }*/
            weight.jointIndex += numBone;
        }

        meshes.push_back(mesh);
        numMeshes++;
    }
}