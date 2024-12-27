//
// Created by Administrator on 2024/2/2.
//
#include <iostream>

#include "geomath.h"
#include "anim.h"
#include "text.h"
#include "files.h"
#include "mesh.h"

using namespace std;

int atoq(int argc, char *argv[])
{
    idQuat quat;
    quat.Set(-0.5036774874, -0.4318675399, -0.2934795916);
    idAngles angles = quat.ToAngles();
    cout << quat.ToString() << endl;
    idQuat quat2 = angles.ToQuat();
    cout << quat2.ToString() << endl;
    return 0;
}

int anim(int argc, char *argv[])
{
    idStr file1 = R"(F:\pak\cait\md5\13\idle1.md5anim)";
    idStr file2 = R"(F:\pak\cait\md5\13\idle2.md5anim)"; // 87
    idLexer lexer;
    lexer.Parse(file1);
    idMd5AnimFile md51;
    md51.Parse(lexer);
    //System.err.println(JSON.toJSONString(file));

/*    idList<const idMd5AnimBone *> filter = idMd5AnimBone::Filter(md51.boneList, { "*Pelvis"});
    for (const auto &item: filter)
    {
        cout << item->name << endl;
    }*/

    lexer.Parse(file2);
    idMd5AnimFile md52;
    md52.Parse(lexer);

    idMd5AnimFile combine = idMd5AnimFile::Combine(md51, 0, md52, 91, 7, {"*L_Shoulder", "*R_Shoulder"});
    double xr = 20;
    double yr = -30;
    double zr = 0;
    combine = idMd5AnimFile::Rotate(combine, &xr, &yr, &zr, {"R_Shoulder"});
    xr = -30;
    yr = 50;
    combine = idMd5AnimFile::Rotate(combine, &xr, &yr, &zr, {"L_Shoulder"});
    //combine = idMd5AnimFile::TranslateAbsolute(combine, nullptr, &yr, nullptr, {"Spine1"});

    idStr sb;
    combine.Dump(sb);
    //cout << sb << endl;
    idFileSystem::Write(sb, R"(C:\Users\Administrator\Documents\attack.md5anim)");

    return 0;
}

int mesh(int argc, char *argv[])
{
    idStr file1 = R"(D:\project\c\anim\girl011a_02_body01_skm.md5mesh)";
    idLexer lexer;
    lexer.Parse(file1);

    idMd5MeshFile mesh;
    mesh.Parse(lexer);

    int i = 0;
    for(const auto &joint : mesh.joints.joints)
    {
        cout << "\t\"" << joint.name.c_str() << R"(" )" << joint.parentIndex << ' ' << 63  << ' ' << (6 * i) << endl;
        i++;
    }

    return 0;
}

idStr calc_rot(int mask, double pitch, double yaw, double roll)
{
    idQuat quat;
    quat.Set(0, 0, 0);
    idAngles angles = quat.ToAngles();
    if(mask & 1)
        angles.pitch += pitch;
    if(mask & 2)
        angles.yaw += yaw;
    if(mask & 4)
        angles.roll += roll;
    quat = angles.ToQuat();

    idStr str = idStr::dtostr({quat.x, quat.y, quat.z});

    cout << str << endl;
    return str;
}

int append_weapon(int argc, char *argv[])
{
//#define JOINT_NAME "socket_face01" // girl017_02
#define JOINT_NAME "bip001_prop1" // "bip001_r_hand" // "bip001_neck2" // girl017_02
#define GIRL "girl007a_02"
#define WP "wp01a_noface"
#ifdef _WIN32
    idStr inAnimDir = R"(F:\pak\test\doom3\snow\models\girl007a_02\wp01a)";
    idStr outAnimDir = R"(F:\pak\test\doom3\snow\models\girl007a_02\wp02b)";
    idStr weaponMeshFile = R"(C:\Users\Administrator\Desktop\wp.md5mesh)";
    idStr inMeshFile = R"(F:\pak\test\doom3\snow\models\girl007a_02\girl007a_02b.md5mesh)";
    idStr outMeshFile = R"(F:\pak\test\doom3\snow\models\girl007a_02\girl007a_02_wp.md5mesh)";
#else
    idStr inAnimDir = "/sdcard/idtech4amm/doom3/snow/models/" GIRL "/" WP;
    idStr outAnimDir = "/sdcard/idtech4amm/doom3/snow/models/" GIRL "/to_" WP;
    idStr weaponMeshFile = "/sdcard/idtech4amm/doom3/snow/models/" GIRL "/" WP ".md5mesh";
    idStr inMeshFile = R"(/sdcard/idtech4amm/doom3/snow/models/" GIRL "/" GIRL ".md5mesh)";
    idStr outMeshFile = R"(/sdcard/idtech4amm/doom3/snow/models/" GIRL "/to_" GIRL "_" WP ".md5mesh)";
#endif
    
    idStr rot = calc_rot(4, 0, 0, 180); // weapon

/*    cout << "Input any to continue......" << endl;
    char a;
    cin >> a;*/

    cout << "Scanning animations: " << inAnimDir << endl;
    idStrList anims = idFileSystem::Ls(inAnimDir.c_str());
    cout << "Read " << anims.size() << " animations" << endl;

    idFileSystem::Mkdir(outAnimDir.c_str());
    cout << "Create directory: " << outAnimDir << endl;

    idLexer lexer;

    lexer.Parse(weaponMeshFile);
    idMd5MeshFile mesh;
    mesh.Parse(lexer);

    auto &joint = mesh.joints.joints[0];
    //joint.rotation = rot;
    cout << joint.Transform() << endl;

    lexer.Parse(inMeshFile);
    idMd5MeshFile masterMesh;
    masterMesh.Parse(lexer);

    cout << "Handle mesh file: " << inMeshFile << endl;
    masterMesh.Append(mesh, JOINT_NAME);

    idStr str;
    masterMesh.Dump(str);
    idFileSystem::Write(str, outMeshFile);
    cout << "Save mesh to file: " << outMeshFile << endl;

    for (const auto &item: anims)
    {
        cout << "Handle animation file: " << item << endl;

        idStr filename = idFileSystem::Filename(item.c_str());

        idStr toanimfile = idFileSystem::AppendPath({outAnimDir, filename});
        cout << "Save to file: " << toanimfile << endl;

        idMd5AnimFile anim;
        lexer.Parse(item);
        anim.Parse(lexer);

        anim.AppendBones(mesh.joints, JOINT_NAME);

        idStr text;
        anim.Dump(text);

        idFileSystem::Write(text, toanimfile.c_str());
    }

    cout << "Done!!!" << endl;

    return 0;
}

int append_face(int argc, char *argv[])
{
//#define JOINT_NAME "socket_face01" // girl017_02
#define JOINT_NAME "bip001_head" // "bip001_neck2" // girl017_02
#define GIRL "girl007a_02"
#define WP "wp01a_noface"
#ifdef _WIN32
    idStr inAnimDir = R"(F:\pak\snow\wp02b)";
    idStr outAnimDir = R"(D:\project\c\neo\Release\base\models\girl017_02\wp02b)";
    idStr faceMeshFile = R"(F:\pak\snow\girl017_02_face.md5mesh)";
#else
    idStr inAnimDir = "/sdcard/idtech4amm/doom3/snow/models/" GIRL "/" WP;
    idStr outAnimDir = "/sdcard/idtech4amm/doom3/snow/models/" GIRL "/to_" WP;
    idStr faceMeshFile = "/sdcard/idtech4amm/doom3/snow/models/" GIRL "/" GIRL "_face.md5mesh";
#endif

    idStr rot = calc_rot(2 | 4, 0, 90, 90);

    cout << "Input any to continue......" << endl;
    char a;
    cin >> a;

    cout << "Scanning animations: " << inAnimDir << endl;
    idStrList anims = idFileSystem::Ls(inAnimDir.c_str());
    cout << "Read " << anims.size() << " animations" << endl;

    idFileSystem::Mkdir(outAnimDir.c_str());
    cout << "Create directory: " << outAnimDir << endl;

    idLexer lexer;

    lexer.Parse(faceMeshFile);
    idMd5MeshFile mesh;
    mesh.Parse(lexer);

    auto &joint = mesh.joints.joints[0];
    joint.rotation = rot;
    cout << joint.Transform() << endl;

    for (const auto &item: anims)
    {
        cout << "Handle animation file: " << item << endl;

        idStr filename = idFileSystem::Filename(item.c_str());

        idStr toanimfile = idFileSystem::AppendPath({outAnimDir, filename});
        cout << "Save to file: " << toanimfile << endl;

        idMd5AnimFile anim;
        lexer.Parse(item);
        anim.Parse(lexer);

        anim.AppendBones(mesh.joints, JOINT_NAME);

        idStr text;
        anim.Dump(text);

        idFileSystem::Write(text, toanimfile.c_str());
    }

    cout << "Done!!!" << endl;

    return 0;
}

int rot(int argc, char *argv[])
{
    idStr file1 = R"(/data/data/com.termux/files/home/MyDocs/idtech4amm/doom3/snow/models/girl007a_02/wp01a/halfbunker_move_fl.md5anim)";
	idStr file2 = R"(/data/data/com.termux/files/home/MyDocs/idtech4amm/doom3/snow/models/girl007a_02/wp01a/halfbunker_move_fr.md5anim)";
    idStr to_file1 = R"(/data/data/com.termux/files/home/MyDocs/idtech4amm/doom3/snow/models/girl007a_02/wp01a/halfbunker_move_f.md5anim)";
    idStr to_file2 = R"(/data/data/com.termux/files/home/MyDocs/idtech4amm/doom3/snow/models/girl007a_02/wp01a/halfbunker_move_b.md5anim)";

    idLexer lexer;
    lexer.Parse(file1);
    idMd5AnimFile md51;
    md51.Parse(lexer);

    lexer.Parse(file2);
    idMd5AnimFile md52;
    md52.Parse(lexer);

    double xr = 0;
    double yr = -90;
    double zr = 0;
    idStr sb;
    idMd5AnimFile combine;

    combine = idMd5AnimFile::Rotate(md51, nullptr, &yr, nullptr, {"root_01"});
    combine.Dump(sb);
    idFileSystem::Write(sb, to_file1);

    combine = idMd5AnimFile::Rotate(md52, nullptr, &yr, nullptr, {"root_01"});
    combine.Dump(sb);
    idFileSystem::Write(sb, to_file2);

    return 0;
}

int rev(int argc, char *argv[])
{
    idStr file1 = R"(/data/data/com.termux/files/home/MyDocs/idtech4amm/doom3/snow/models/girl007a_02/wp01a/halfbunker_move_b.md5anim)";
    idStr to_file1 = R"(/data/data/com.termux/files/home/MyDocs/idtech4amm/doom3/snow/models/girl007a_02/wp01a/halfbunker_move_b_rev.md5anim)";

    idLexer lexer;
    lexer.Parse(file1);
    idMd5AnimFile md51;
    md51.Parse(lexer);

    idStr sb;
    idMd5AnimFile combine;

    combine = md51.Reverse();
    combine.Dump(sb);
    idFileSystem::Write(sb, to_file1);

    return 0;
}

int trans(int argc, char *argv[])
{
    idStr file1 = R"(/data/data/com.termux/files/home/MyDocs/idtech4amm/doom3/snow/models/girl007a_02/wp01a/halfbunker_loop_l.md5anim)";
    idStr to_file1 = R"(/data/data/com.termux/files/home/MyDocs/idtech4amm/doom3/snow/models/girl007a_02/wp01a/halfbunker_loop_l_idle.md5anim)";

    idLexer lexer;
    lexer.Parse(file1);
    idMd5AnimFile md51;
    md51.Parse(lexer);

    double xt = -35;
    double yt = 0;
    double zt = 0;
    idStr sb;
    idMd5AnimFile combine;

    combine = idMd5AnimFile::TranslateAbsolute(md51, &xt, nullptr, nullptr, {"root_01"});

    double xr = 0;
    double yr = -90;
    double zr = 0;
    combine = idMd5AnimFile::Rotate(combine, nullptr, &yr, nullptr, {"root_01"});

    combine.Dump(sb);
    idFileSystem::Write(sb, to_file1);

    return 0;
}

int cut(int argc, char *argv[])
{
    idStr file1 = R"(/data/data/com.termux/files/home/MyDocs/idtech4amm/doom3/snow/models/girl007a_02/wp01a/base_jump_end.md5anim)";
    idStr to_file1 = R"(/data/data/com.termux/files/home/MyDocs/idtech4amm/doom3/snow/models/girl007a_02/wp01a/base_jump_end_down.md5anim)";

    idLexer lexer;
    lexer.Parse(file1);
    idMd5AnimFile md51;
    md51.Parse(lexer);

    idStr sb;
    idMd5AnimFile combine;

    combine = md51.Cut(0, 12 + 1);
    //combine = md51.Cut(13, 33 - 13 + 1);

    combine.Dump(sb);
    idFileSystem::Write(sb, to_file1);

    return 0;
}

int main(int argc, char *argv[])
{
    append_weapon(argc, argv);
    //rot(argc, argv);
    //rev(argc, argv);
    //trans(argc, argv);
    //cut(argc, argv);
    return 0;
}
