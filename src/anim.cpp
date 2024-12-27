#include "anim.h"

#include <cctype>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdio>

#include "geomath.h"
#include "text.h"
#include "mesh.h"

using namespace std;

bool operator==(const idMd5AnimBone &a, const idMd5AnimBone &b)
{
    return a.index == b.index;
}

idList<const idMd5AnimBone *> idMd5AnimBone::AllChildren() const {
    idList<const idMd5AnimBone *> res;
    if (children.empty())
        return res;
    for (const idMd5AnimBone *child: children) {
        res.push_back(child);
        res.addAll(child->AllChildren());
    }
    return res;
}

bool operator<(const idMd5AnimBone &a, const idMd5AnimBone &b)
{
    return a.index < b.index;
}

idList<const idMd5AnimBone *>
idMd5AnimBone::Filter(const idList<idMd5AnimBone> &total, idList<const idMd5AnimBone *> *list, const idStr &str) {
    idList<const idMd5AnimBone *> res;
    if (str.front() == '*') {
        idStr bone = str.substr(1);
        auto first = find_if(total.cbegin(), total.cend(), [&bone](const auto &x) {
            return x.name == bone;
        });
        if (list)
            res.addAll(*list);
        if (first == total.cend())
            return res;
        const auto &firstBone = *first;
        res.push_back(&firstBone);
        res.addAll(firstBone.AllChildren());
    } else if (str.front() == '-') {
        if (list) {
            idList<const idMd5AnimBone *> subs = Filter(total, nullptr, str.substr(1));
            idList<idStr> collect;
            for_each(subs.cbegin(), subs.cend(), [&collect](const auto &item) {
                collect.push_back(item->name);
            });
            for_each(list->cbegin(), list->cend(), [&res, &collect](const auto &item) {
                if (!collect.contains(item->name))
                    res.push_back(item);
            });
        }
    } else {
        auto first = find_if(total.cbegin(), total.cend(), [&str](const auto &x) {
            return x.name == str;
        });
        if (list)
            res.addAll(*list);
        if (first == total.cend())
            return res;
        const auto &firstBone = *first;
        res.push_back(&firstBone);
    }
    sort(res.begin(), res.end());
    return res;
}

idList<const idMd5AnimBone *> idMd5AnimBone::Filter(const idList<idMd5AnimBone> &total, const idList<idStr> &strs) {
    idList<const idMd5AnimBone *> list;
    for (const auto &str: strs) {
        list = Filter(total, &list, str);
    }
    return list;
}

void idMd5AnimTransform::Parse(const idStr &str) {
    auto t = str.trim();
    if (t.front() == '(') {
        auto i = t.find(')');
        t = idStr::Trim(t.substr(1, i - 1));
        sscanf(t.c_str(), "%lf %lf %lf", &xt, &yt, &zt);
        i = t.find('(', i);
        auto m = t.find(')', i);
        t = idStr::Trim(t.substr(i + 1, m - i - 1));
        sscanf(t.c_str(), "%lf %lf %lf", &xr, &yr, &zr);
    } else {
        sscanf(t.c_str(), "%lf %lf %lf %lf %lf %lf", &xt, &yt, &zt, &xr, &yr, &zr);
    }
}

idMd5AnimTransform idMd5AnimTransform::From(const idStr &str) {
    idMd5AnimTransform transform;
    transform.Parse(str);
    return transform;
}

idStr idMd5AnimTransform::Tostring(bool q) const {
    char text[4096];
    idStr arr[] = {idMath::ToString(xt), idMath::ToString(yt), idMath::ToString(zt), idMath::ToString(xr),
                   idMath::ToString(yr), idMath::ToString(zr)};
    snprintf(text, sizeof(text) - 1, q ? "( %s %s %s ) ( %s %s %s )" : "%s %s %s %s %s %s", arr[0].c_str(),
             arr[1].c_str(), arr[2].c_str(), arr[3].c_str(), arr[4].c_str(), arr[5].c_str());
    return {text};
}

void idMd5AnimHierarchy::Parse(idLexer &lexer, int numJoints) {
    lexer.ReadLine();
    for (int i = 0; i < numJoints; i++) {
        joints.push_back(lexer.ReadLine());
    }
    lexer.ReadLine();
}

void idMd5AnimHierarchy::Dump(idStr &sb) {
    sb.append("hierarchy {").append("\n");
    for (const auto &joint: joints) {
        sb.append(joint).append("\n");
    }
    sb.append("}\n");
}

idList<idMd5AnimBone> idMd5AnimHierarchy::ParseBone() {
    idList<idMd5AnimBone> list;
    for (int i = 0; i < joints.size(); i++) {
        const idStr &joint = joints[i];
        idMd5AnimBone bone;
        auto start = joint.find('"');
        start++;
        auto end = joint.find('"', start);
        bone.name = joint.substr(start, end - start);
        bone.index = i;
        start = end + 1;
        while (start < joint.length()) {
            if (!isspace(joint.at(start)))
                break;
            start++;
        }
        end = joint.find(' ', start);
        bone.parentIndex = stoi(joint.substr(start, end - start));
        list.push_back(bone);
    }

    return list;
}

void idMd5AnimBounds::Parse(idLexer &lexer, int numFrames) {
    lexer.ReadLine();
    for (int i = 0; i < numFrames; i++) {
        frames.push_back(lexer.ReadLine());
    }
    lexer.ReadLine();
}

void idMd5AnimBounds::Dump(idStr &sb) {
    sb.append("bounds {").append("\n");
    for (const auto &joint: frames) {
        sb.append(joint).append("\n");
    }
    sb.append("}\n");
}

void idMd5AnimBounds::Reverse() {
	std::reverse(frames.begin(), frames.end());
}

void idMd5AnimBaseFrame::Parse(idLexer &lexer, int numJoints) {
    lexer.ReadLine();
    for (int i = 0; i < numJoints; i++) {
        auto s = lexer.ReadLine();
        joints.push_back(s);
    }
    lexer.ReadLine();
}

void idMd5AnimBaseFrame::Dump(idStr &sb) {
    sb.append("baseframe {").append("\n");
    for (const auto &joint: joints) {
        sb.append(joint).append("\n");
    }
    sb.append("}\n");
}

void idMd5AnimFrame::Parse(idLexer &lexer, int numJoints) {
    index = stoi(idLexer::GetValue(lexer.ReadLine(), "frame "));
    for (int i = 0; i < numJoints; i++) {
        auto s = lexer.ReadLine();
        joints.push_back(s);
    }
    lexer.ReadLine();
}

void idMd5AnimFrame::Dump(idStr &sb) {

    sb.append("frame ").append(idMath::ToString(index)).append(" {").append("\n");
    for (const auto &joint: joints) {
        sb.append(joint).append("\n");
    }
    sb.append("}\n");
}

void idMd5AnimFrame::Rotate(int i, const double *xr, const double *yr, const double *zr) {
    if (!xr && !yr && !zr)
        return;

    idMd5AnimTransform transform = idMd5AnimTransform::From(joints[i]);
    idQuat quat;
    quat.Set(transform.xr, transform.yr, transform.zr);
    idAngles angles = quat.ToAngles();
    if (xr)
        angles.pitch += *xr;
    if (yr)
        angles.yaw += *yr;
    if (zr)
        angles.roll += *zr;
    quat = angles.ToQuat();
    transform.xr = quat.x;
    transform.yr = quat.y;
    transform.zr = quat.z;
    joints[i] = "  " + transform.Tostring(false);
}

void idMd5AnimFrame::SetRotate(int i, const double *xr, const double *yr, const double *zr) {
    if (!xr && !yr && !zr)
        return;

    idMd5AnimTransform transform = idMd5AnimTransform::From(joints[i]);
    idQuat quat;
    quat.Set(transform.xr, transform.yr, transform.zr);
    idAngles angles = quat.ToAngles();
    if (xr)
        angles.pitch = *xr;
    if (yr)
        angles.yaw = *yr;
    if (zr)
        angles.roll = *zr;
    quat = angles.ToQuat();
    transform.xr = quat.x;
    transform.yr = quat.y;
    transform.zr = quat.z;
    joints[i] = "  " + transform.Tostring(false);
}

void idMd5AnimFrame::Translate(int i, const double *xt, const double *yt, const double *zt)
{
    if (!xt && !yt && !zt)
        return;

    idMd5AnimTransform transform = idMd5AnimTransform::From(joints[i]);
    idQuat quat;
    quat.Set(transform.xr, transform.yr, transform.zr);
    idVec3 forward, right, up;
    quat.ToVectors(&forward, &right, &up);
    idVec3 v{transform.xt, transform.yt, transform.zt};
    if (xt)
    {
        right *= *xt;
        v += right;
    }
    if (yt)
    {
        forward *= *yt;
        v += forward;
    }
    if (zt)
    {
        up *= *zt;
        v += up;
    }
    transform.xt = v.x;
    transform.yt = v.y;
    transform.zt = v.z;
    joints[i] = "  " + transform.Tostring(false);
}

void idMd5AnimFrame::SetTranslate(int i, const double *xt, const double *yt, const double *zt)
{
    if (!xt && !yt && !zt)
        return;

    idMd5AnimTransform transform = idMd5AnimTransform::From(joints[i]);
    if (xt)
        transform.xt = *xt;
    if (yt)
        transform.yt = *yt;
    if (zt)
        transform.zt = *zt;
    joints[i] = "  " + transform.Tostring(false);
}

void idMd5AnimFrame::TranslateAbsolute(int i, const double *xt, const double *yt, const double *zt)
{
    if (!xt && !yt && !zt)
        return;

    idMd5AnimTransform transform = idMd5AnimTransform::From(joints[i]);
    if (xt)
        transform.xt += *xt;
    if (yt)
        transform.yt += *yt;
    if (zt)
        transform.zt += *zt;
    joints[i] = "  " + transform.Tostring(false);
}

void idMd5AnimFile::ParseBone() {
    boneList = hierarchy.ParseBone();
    root = &boneList[0];
    MakeChildren();
}

void idMd5AnimFile::Parse(idLexer &lexer) {
    MD5Version = idLexer::GetValue(lexer.ReadLine(), "MD5Version ");
    lexer.ReadLine();

    numFrames = stoi(idLexer::GetValue(lexer.ReadLine(), "numFrames "));
    numJoints = stoi(idLexer::GetValue(lexer.ReadLine(), "numJoints "));
    frameRate = stoi(idLexer::GetValue(lexer.ReadLine(), "frameRate "));
    numAnimatedComponents = stoi(idLexer::GetValue(lexer.ReadLine(), "numAnimatedComponents "));

    hierarchy.Parse(lexer, numJoints);
    bounds.Parse(lexer, numFrames);
    baseFrame.Parse(lexer, numJoints);
    for (int i = 0; i < numFrames; i++) {
        idMd5AnimFrame frame;
        frame.Parse(lexer, numJoints);
        frames.push_back(frame);
    }

    ParseBone();
}

void idMd5AnimFile::Dump(idStr &sb) {
    sb.append("MD5Version ").append(MD5Version).append("\n");
    sb.append("commandline \"\"").append("\n");
    sb.append("\n");
    sb.append("numFrames ").append(idMath::ToString(numFrames)).append("\n");
    sb.append("numJoints ").append(idMath::ToString(numJoints)).append("\n");
    sb.append("frameRate ").append(idMath::ToString(frameRate)).append("\n");
    sb.append("numAnimatedComponents ").append(idMath::ToString(numAnimatedComponents)).append("\n");
    sb.append("\n");
    hierarchy.Dump(sb);
    sb.append("\n");
    bounds.Dump(sb);
    sb.append("\n");
    baseFrame.Dump(sb);
    sb.append("\n");
    for (idMd5AnimFrame frame: frames) {
        frame.Dump(sb);
        sb.append("\n");
    }
}

idMd5AnimFile::idMd5AnimFile(const idMd5AnimFile &other)
{
    MD5Version = other.MD5Version;
    commandline = other.commandline;
    numFrames = other.numFrames;
    numJoints = other.numJoints;
    frameRate = other.frameRate;
    numAnimatedComponents = other.numAnimatedComponents;
    hierarchy = other.hierarchy;
    bounds = other.bounds;
    baseFrame = other.baseFrame;
    frames = other.frames;
    ParseBone();
}

idMd5AnimFile &idMd5AnimFile::operator=(const idMd5AnimFile &other) {
    if (this == &other)
        return *this;
    MD5Version = other.MD5Version;
    commandline = other.commandline;
    numFrames = other.numFrames;
    numJoints = other.numJoints;
    frameRate = other.frameRate;
    numAnimatedComponents = other.numAnimatedComponents;
    hierarchy = other.hierarchy;
    bounds = other.bounds;
    baseFrame = other.baseFrame;
    frames = other.frames;
    ParseBone();
    return *this;
}

idMd5AnimFile idMd5AnimFile::Cut(int start, int count) const {
    idMd5AnimFile res;
    res.MD5Version = MD5Version;
    res.commandline = commandline;
    res.numFrames = count;
    res.numJoints = numJoints;
    res.frameRate = frameRate;
    res.numAnimatedComponents = numAnimatedComponents;
    res.hierarchy = hierarchy;
    for (int i = 0; i < count; i++)
        res.bounds.frames.push_back(bounds.frames[start + i]);
    res.baseFrame = baseFrame;
    for (int i = 0; i < count; i++) {
        const auto &frame = frames[start + i];
        idMd5AnimFrame f;
        f.index = i;
        f.joints = frame.joints;
        res.frames.push_back(f);
    }
    res.ParseBone();
    return res;
}

idMd5AnimFile idMd5AnimFile::Combine(const idMd5AnimFile &a, int aStart, const idMd5AnimFile &b, int bStart, int count,
                                     const idList<idStr> &bBones) {
    idMd5AnimFile aCut = a.Cut(aStart, count);
    idMd5AnimFile bCut = b.Cut(bStart, count);
    idList<const idMd5AnimBone *> bBoneList = idMd5AnimBone::Filter(aCut.boneList, bBones);
    idList<int> bBonIndexes;
    std::for_each(bBoneList.cbegin(), bBoneList.cend(), [&bBonIndexes](const auto &item) {
        bBonIndexes.push_back(item->index);
    });
    for (int i = 0; i < aCut.frames.size(); i++) {
        idMd5AnimFrame &aFrame = aCut.frames[i];
        const idMd5AnimFrame &bFrame = bCut.frames[i];
        for (int m = 0; m < aFrame.joints.size(); m++) {
            if (bBonIndexes.contains(m))
                aFrame.joints[m] = bFrame.joints[m];
        }
    }
    return aCut;
}

idMd5AnimFile idMd5AnimFile::Rotate(const idMd5AnimFile &a, const double *xr, const double *yr, const double *zr, const idList<idStr> &bBones) {
    idMd5AnimFile clone(a);
    idList<const idMd5AnimBone *> bBoneList = idMd5AnimBone::Filter(clone.boneList, bBones);
    for (auto &frame: clone.frames) {
        for (const auto &idMd5AnimBone: bBoneList) {
            frame.Rotate(idMd5AnimBone->index, xr, yr, zr);
        }
    }
    return clone;
}

idMd5AnimFile idMd5AnimFile::SetRotate(const idMd5AnimFile &a, const double *xr, const double *yr, const double *zr, const idList<idStr> &bBones) {
    idMd5AnimFile clone(a);
    idList<const idMd5AnimBone *> bBoneList = idMd5AnimBone::Filter(clone.boneList, bBones);
    for (auto &frame: clone.frames) {
        for (const auto &idMd5AnimBone: bBoneList) {
            frame.SetRotate(idMd5AnimBone->index, xr, yr, zr);
        }
    }
    return clone;
}

idMd5AnimFile idMd5AnimFile::Translate(const idMd5AnimFile &a, const double *xt, const double *yt, const double *zt, const idList<idStr> &bBones) {
    idMd5AnimFile clone(a);
    idList<const idMd5AnimBone *> bBoneList = idMd5AnimBone::Filter(clone.boneList, bBones);
    for (auto &frame: clone.frames) {
        for (const auto &idMd5AnimBone: bBoneList) {
            frame.Translate(idMd5AnimBone->index, xt, yt, zt);
        }
    }
    return clone;
}

idMd5AnimFile idMd5AnimFile::SetTranslate(const idMd5AnimFile &a, const double *xt, const double *yt, const double *zt, const idList<idStr> &bBones) {
    idMd5AnimFile clone(a);
    idList<const idMd5AnimBone *> bBoneList = idMd5AnimBone::Filter(clone.boneList, bBones);
    for (auto &frame: clone.frames) {
        for (const auto &idMd5AnimBone: bBoneList) {
            frame.SetTranslate(idMd5AnimBone->index, xt, yt, zt);
        }
    }
    return clone;
}

idMd5AnimFile idMd5AnimFile::TranslateAbsolute(const idMd5AnimFile &a, const double *xt, const double *yt, const double *zt, const idList<idStr> &bBones) {
    idMd5AnimFile clone(a);
    idList<const idMd5AnimBone *> bBoneList = idMd5AnimBone::Filter(clone.boneList, bBones);
    for (auto &frame: clone.frames) {
        for (const auto &idMd5AnimBone: bBoneList) {
            frame.TranslateAbsolute(idMd5AnimBone->index, xt, yt, zt);
        }
    }
    return clone;
}

void idMd5AnimFile::MakeChildren() {
    for (int i = 1; i < boneList.size(); i++) {
        idMd5AnimBone &b = boneList[i];
        idMd5AnimBone &p = boneList[b.parentIndex];
        p.children.push_back(&b);
        b.parent = &p;
    }
}

void idMd5AnimFile::AppendBones(const idMd5MeshJoints &joints, const char *boneName)
{
    const auto &itor = std::find_if(boneList.begin(), boneList.end(), [boneName](const auto &x) {
        return x.name == boneName;
    });
    if(itor == boneList.cend())
        throw idStr("bone not exists: ") + boneName;

    const idMd5AnimBone &bone = *itor;
    auto n = boneList.size() * 6;
    auto numBone = boneList.size();
    int i = 0;
    idStr parentBoneName = bone.name;

    for (const auto &joint : joints.joints)
    {
        idStr bname = joint.name;
        bname.ToLower();
        //std::cout << bname.c_str() << std::endl;
        bname.Replace('-', '_');
        const int BoneIndex = numBone + i;

        const int ParentBoneIndex = i == 0 ? bone.index : (numBone + i - 1);

        idStr h = idStr::Format("  \"%s\" %d 63 %d // %d %s", bname.c_str(), ParentBoneIndex, n, BoneIndex, parentBoneName.c_str());
        hierarchy.joints.push_back(h);

        baseFrame.joints.push_back("  " + joint.Transform() + idStr::Format(" // %d %s", BoneIndex, bname.c_str()));

        for (auto &frame: frames)
        {
            frame.joints.push_back("  " + joint.Numbers() + idStr::Format(" // %d %s %d %s", BoneIndex, bname.c_str(), ParentBoneIndex, parentBoneName.c_str()));
        }

        n += 6;
        i++;
        parentBoneName = bname;
    }

    this->numJoints += i;
    this->numAnimatedComponents = n;
}

idMd5AnimFile idMd5AnimFile::Reverse() const {
    idMd5AnimFile res(*this);
	res.bounds.Reverse();
    for (int i = 0; i < res.frames.size(); i++) {
        auto &frame = res.frames[i];
        frame.index = res.frames.size() - 1 - i;
    }
	std::reverse(res.frames.begin(), res.frames.end());
    res.ParseBone();
    return res;
}

