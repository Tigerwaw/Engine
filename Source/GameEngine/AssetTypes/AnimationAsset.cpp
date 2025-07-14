#include "Enginepch.h"
#include "AnimationAsset.h"
#include "TGAFBX/TGAFbx.h"

bool AnimationAsset::Load()
{
    TGA::FBX::Animation tgaAnimation;
    TGA::FBX::Importer::LoadAnimation(GetPath(), tgaAnimation);

    Animation newAnimation;
    newAnimation.Duration = static_cast<float>(tgaAnimation.Duration);
    newAnimation.FramesPerSecond = tgaAnimation.FramesPerSecond;

    for (auto& tgaAnimFrame : tgaAnimation.Frames)
    {
        Animation::Frame frame;
        for (auto& tgaAnimFrameJoint : tgaAnimFrame.LocalTransforms)
        {
            auto& matrix = tgaAnimFrameJoint.second.Data;
            Math::Matrix4x4<float> jointTransform = { matrix[0], matrix[1], matrix[2], matrix[3],
                                                                 matrix[4], matrix[5], matrix[6], matrix[7],
                                                                 matrix[8], matrix[9], matrix[10], matrix[11],
                                                                 matrix[12], matrix[13], matrix[14], matrix[15] };
            frame.BoneTransforms.emplace(tgaAnimFrameJoint.first, jointTransform);
        }

        newAnimation.Frames.emplace_back(frame);
    }

    animation = std::make_shared<Animation>(std::move(newAnimation));
    return true;
}

bool AnimationAsset::Unload()
{
    animation = nullptr;
    return true;
}
