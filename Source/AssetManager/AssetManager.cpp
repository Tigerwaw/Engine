#include "AssetManager.h"
#include "Logger\Logger.h"
#include <d3d11.h>
#include "Asset.h"
#include "GraphicsEngine.h"
#include "RHI/Vertex.h"

#include <iostream>
#include <fstream>
#include "Utilities/nlohmann/json.hpp"
#include "TGAFBX/TGAFbx.h"

#include "DefaultTextures/Default_C.h"
#include "DefaultTextures/Default_N.h"
#include "DefaultTextures/Default_M.h"
#include "DefaultTextures/Default_FX.h"

namespace nl = nlohmann;

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(AssetManagerLog, "AssetManager", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(AssetManagerLog, "AssetManager", Error);
#endif

DEFINE_LOG_CATEGORY(AssetManagerLog);

bool AssetManager::Initialize(const std::filesystem::path& aContentRootPath, bool aAutoRegisterAllAssetsInRoot)
{
    LOG(AssetManagerLog, Log, "Initializing Asset Manager...");

    myContentRoot = std::filesystem::absolute(aContentRootPath);
    if (!std::filesystem::exists(aContentRootPath))
    {
        LOG(AssetManagerLog, Error, "Can't find directory at path '{}'! Trying to create a new directory!", aContentRootPath.string());

        if (!std::filesystem::create_directories(aContentRootPath))
        {
            LOG(AssetManagerLog, Error, "Failed to initialize Asset Manager! Can't find or create directory at path '{}'!", aContentRootPath.string());
            return false;
        }
    }

    RegisterEngineAssets();

    if (aAutoRegisterAllAssetsInRoot)
    {
        RegisterAllAssetsInDirectory();
    }

    LOG(AssetManagerLog, Log, "Asset Manager Initialized! Root content directory set to '{}'.", myContentRoot.string());
    return true;
}

void AssetManager::RegisterEngineAssets()
{
    RegisterEngineTextureAsset("Default_C", BuiltIn_Default_C_ByteCode, sizeof(BuiltIn_Default_C_ByteCode));
    RegisterEngineTextureAsset("Default_N", BuiltIn_Default_N_ByteCode, sizeof(BuiltIn_Default_N_ByteCode));
    RegisterEngineTextureAsset("Default_M", BuiltIn_Default_M_ByteCode, sizeof(BuiltIn_Default_M_ByteCode));
    RegisterEngineTextureAsset("Default_FX", BuiltIn_Default_FX_ByteCode, sizeof(BuiltIn_Default_FX_ByteCode));

    std::shared_ptr<MaterialAsset> asset = std::make_shared<MaterialAsset>();
    asset->material = std::make_shared<Material>();
    asset->material->MaterialSettings().albedoTint = { 1.0f, 1.0f, 1.0f, 1.0f };
    asset->material->SetAlbedoTexture(GetAsset<TextureAsset>("Default_C")->texture);
    asset->material->SetNormalTexture(GetAsset<TextureAsset>("Default_N")->texture);
    asset->material->SetMaterialTexture(GetAsset<TextureAsset>("Default_M")->texture);
    asset->name = "DefaultMaterial";
    myAssets.emplace(asset->name, asset);

    LOG(AssetManagerLog, Log, "Registered material asset {}", asset->name.string());

    RegisterPlanePrimitive();
    RegisterCubePrimitive();
    RegisterRampPrimitive();

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Models/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterMeshAsset(file.path());
        }
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Textures/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterTextureAsset(file.path());
        }
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Materials/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterMaterialAsset(file.path());
        }
    }
}

void AssetManager::RegisterAllAssetsInDirectory()
{
    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "Models/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterMeshAsset(file.path());
        }
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "Animations/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterAnimationAsset(file.path());
        }
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "Textures/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterTextureAsset(file.path());
        }
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "Materials/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterMaterialAsset(file.path());
        }
    }
}

bool AssetManager::RegisterMeshAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("fbx")) return false;

    TGA::FBX::Mesh tgaMesh;
    TGA::FBX::Importer::LoadMesh(aPath, tgaMesh);

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    std::vector<Mesh::Element> elements;

    unsigned nextVertexOffset = 0;
    unsigned nextIndexOffset = 0;

    for (auto& tgaElement : tgaMesh.Elements)
    {
        Mesh::Element element;
        element.VertexOffset = nextVertexOffset;
        element.IndexOffset = nextIndexOffset;
        element.NumVertices = static_cast<int>(tgaElement.Vertices.size());
        element.NumIndices = static_cast<int>(tgaElement.Indices.size());
        element.MaterialIndex = tgaElement.MaterialIndex;
        elements.push_back(element);

        for (auto& v : tgaElement.Vertices)
        {
            vertices.push_back(Vertex(v.Position, v.VertexColors, v.BoneIDs, v.BoneWeights, v.UVs, v.Normal, v.Tangent));
            nextVertexOffset++;
        }

        for (auto& i : tgaElement.Indices)
        {
            indices.push_back(i + element.VertexOffset);
            nextIndexOffset++;
        }
    }

    Mesh::Skeleton skeleton;

    for (auto& tgaJoint : tgaMesh.Skeleton.Bones)
    {
        Mesh::Skeleton::Joint& joint = skeleton.myJoints.emplace_back(Mesh::Skeleton::Joint());
        joint.Parent = tgaJoint.ParentIdx;
        joint.Children = tgaJoint.Children;
        joint.Name = tgaJoint.Name;
        auto& matrix = tgaJoint.BindPoseInverse.Data;
        joint.BindPoseInverse = { matrix[0], matrix[1], matrix[2], matrix[3],
                                  matrix[4], matrix[5], matrix[6], matrix[7],
                                  matrix[8], matrix[9], matrix[10], matrix[11],
                                  matrix[12], matrix[13], matrix[14], matrix[15] };

        joint.BindPoseInverse = joint.BindPoseInverse.GetTranspose();
    }

    skeleton.JointNameToIndex = tgaMesh.Skeleton.BoneNameToIndex;

    Mesh mesh;
    mesh.Initialize(std::move(vertices), std::move(indices), std::move(elements), std::move(skeleton));

    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(mesh));
    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(assetPath, asset);

    LOG(AssetManagerLog, Log, "Registered mesh asset {}", asset->path.filename().string());
    return true;
}

bool AssetManager::RegisterAnimationAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("fbx")) return false;

    TGA::FBX::Animation tgaAnimation;
    TGA::FBX::Importer::LoadAnimation(aPath, tgaAnimation);

    Animation animation;
    animation.Duration = static_cast<float>(tgaAnimation.Duration);
    animation.FramesPerSecond = tgaAnimation.FramesPerSecond;

    for (auto& tgaAnimFrame : tgaAnimation.Frames)
    {
        Animation::Frame frame;
        for (auto& tgaAnimFrameJoint : tgaAnimFrame.LocalTransforms)
        {
            auto& matrix = tgaAnimFrameJoint.second.Data;
            CommonUtilities::Matrix4x4<float> jointTransform = { matrix[0], matrix[1], matrix[2], matrix[3],
                                                                 matrix[4], matrix[5], matrix[6], matrix[7],
                                                                 matrix[8], matrix[9], matrix[10], matrix[11],
                                                                 matrix[12], matrix[13], matrix[14], matrix[15] };
            frame.BoneTransforms.emplace(tgaAnimFrameJoint.first, jointTransform);
        }

        animation.Frames.emplace_back(frame);
    }

    std::shared_ptr<AnimationAsset> asset = std::make_shared<AnimationAsset>();
    asset->animation = std::make_shared<Animation>(std::move(animation));
    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(assetPath, asset);

    LOG(AssetManagerLog, Log, "Registered animation asset {}", asset->path.filename().string());
    return true;
}

bool AssetManager::RegisterMaterialAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("json")) return false;
    //if (!ext.ends_with("mat")) return false;

    std::shared_ptr<MaterialAsset> asset = std::make_shared<MaterialAsset>();
    asset->material = std::make_shared<Material>();
    asset->material->SetAlbedoTexture(GetAsset<TextureAsset>("Default_C")->texture);
    asset->material->SetNormalTexture(GetAsset<TextureAsset>("Default_N")->texture);
    asset->material->SetMaterialTexture(GetAsset<TextureAsset>("Default_M")->texture);

    std::ifstream path(aPath);
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(path);
    }
    catch (nl::json::parse_error e)
    {
        LOG(AssetManagerLog, Error, "Failed to read material asset {}, {}", asset->path.filename().string(), e.what());
        return false;
    }
    path.close();

    asset->material->MaterialSettings().albedoTint = {data["AlbedoTint"]["R"].get<float>(),
                                                      data["AlbedoTint"]["G"].get<float>(),
                                                      data["AlbedoTint"]["B"].get<float>(),
                                                      data["AlbedoTint"]["A"].get<float>() };

    if (data.contains("AlbedoTexture"))
    {
        std::filesystem::path albedoPath = data["AlbedoTexture"].get<std::string>();
        asset->material->SetAlbedoTexture(GetAsset<TextureAsset>(albedoPath)->texture);
    }

    if (data.contains("NormalTexture"))
    {
        std::filesystem::path normalPath = data["NormalTexture"].get<std::string>();
        asset->material->SetNormalTexture(GetAsset<TextureAsset>(normalPath)->texture);
    }

    if (data.contains("MaterialTexture"))
    {
        std::filesystem::path materialPath = data["MaterialTexture"].get<std::string>();
        asset->material->SetMaterialTexture(GetAsset<TextureAsset>(materialPath)->texture);
    }

    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(assetPath, asset);

    LOG(AssetManagerLog, Log, "Registered material asset {}", asset->path.filename().string());
    return true;
}

bool AssetManager::RegisterTextureAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;
    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("dds")) return false;

    std::shared_ptr<TextureAsset> asset = std::make_shared<TextureAsset>();
    asset->texture = std::make_shared<Texture>();
    std::filesystem::path absolutePath = aPath;
    if (!GraphicsEngine::Get().LoadTexture(absolutePath, *asset->texture))
    {
        LOG(AssetManagerLog, Error, "Failed to register texture asset {}", assetPath.string());
        return false;
    }
    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(assetPath, asset);

    LOG(AssetManagerLog, Log, "Registered texture asset {}", asset->path.filename().string());
    return true;
}

AssetManager::AssetManager()
{
    TGA::FBX::Importer::InitImporter();
}

AssetManager::~AssetManager()
{
    TGA::FBX::Importer::UninitImporter();
}

bool AssetManager::ValidateAsset(const std::filesystem::path& aPath)
{
    if (!aPath.has_extension())
    {
        LOG(AssetManagerLog, Error, "Path '{}' does not contain an extension!", aPath.string());
        return false;
    }

    if (!std::filesystem::exists(aPath))
    {
        LOG(AssetManagerLog, Error, "Could not find asset at path '{}'!", aPath.string());
        return false;
    }

    return true;
}

std::filesystem::path AssetManager::MakeRelative(const std::filesystem::path& aPath)
{
    std::filesystem::path targetPath = relative(aPath, myContentRoot);
    targetPath = myContentRoot / targetPath;
    targetPath = weakly_canonical(targetPath);

    //auto [rootEnd, nothing] = std::mismatch(myContentRoot.begin(), myContentRoot.end(), targetPath.begin());
    //if (rootEnd != myContentRoot.end())
    //{
    //    LOG(AssetManagerLog, Error, "Path '{}' is not below the root '{}'!", targetPath.string(), myContentRoot.string());
    //    throw std::invalid_argument("Path is not below the root!");
    //}

    if (aPath.is_absolute())
    {
        return relative(aPath, myContentRoot);
    }

    return aPath;
}

bool AssetManager::RegisterEngineTextureAsset(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize)
{
    std::shared_ptr<TextureAsset> asset = std::make_shared<TextureAsset>();
    asset->texture = std::make_shared<Texture>();
    if (!GraphicsEngine::Get().LoadTexture(aName, aTextureDataPtr, aTextureDataSize, *asset->texture))
    {
        LOG(AssetManagerLog, Error, "Failed to register default texture asset {}", aName);
        return false;
    }
    asset->name = aName;
    myAssets.emplace(aName, asset);

    LOG(AssetManagerLog, Log, "Registered default texture asset {}", aName);

    return true;
}

bool AssetManager::RegisterPlanePrimitive()
{
    float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    float uv[4][4][2] = { {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}}
    };
    float pos[4][4] = {
        {1, 0, 1, 1},
        {1, 0, -1, 1},
        {-1, 0, -1, 1},
        {-1, 0, 1, 1}
    };
    float normals[4][3] = {
        { 0, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 0 }
    };
    float tangents[4][3] = {
        { 1, 0, 0 },
        { 1, 0, 0 },
        { 1, 0, 0 },
        { 1, 0, 0 }
    };

    std::vector<Vertex> vertexList;
    vertexList.emplace_back(Vertex(pos[0], emptyColor, uv[0], normals[0], tangents[0]));
    vertexList.emplace_back(Vertex(pos[1], emptyColor, uv[1], normals[1], tangents[1]));
    vertexList.emplace_back(Vertex(pos[2], emptyColor, uv[2], normals[2], tangents[2]));
    vertexList.emplace_back(Vertex(pos[3], emptyColor, uv[3], normals[3], tangents[3]));

    std::vector<unsigned> indexList = {
        0, 1, 2,
        2, 3, 0,
    };

    Mesh::Element element;
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertexList.size());
    element.NumIndices = static_cast<unsigned>(indexList.size());
    std::vector<Mesh::Element> elementList = { element };

    Mesh plane;
    plane.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());
    
    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(plane));
    asset->name = "PlanePrimitive";
    myAssets.emplace(asset->name, asset);

    LOG(AssetManagerLog, Log, "Registered mesh asset {}", asset->name.string());
    return true;
}

bool AssetManager::RegisterCubePrimitive()
{
    float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    float uv[24][4][2] = { {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                          {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                          {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
    };

    float pos[24][4] = {
        {1,1,-1, 1}, // BACK
        {1,-1,-1, 1},
        {-1,-1,-1, 1},
        {-1,1,-1, 1},

        {1,1,1, 1}, // FRONT
        {-1,1,1, 1},
        {-1,-1,1, 1},
        {1,-1,1, 1},

        {1,1,-1, 1}, // RIGHT
        {1,1,1, 1},
        {1,-1,1, 1},
        {1,-1,-1, 1},

        {1,-1,-1, 1}, // DOWN
        {1,-1,1, 1},
        {-1,-1,1, 1},
        {-1,-1,-1, 1},

        {-1,-1,-1, 1}, // LEFT
        {-1,-1,1, 1},
        {-1,1,1, 1},
        {-1,1,-1, 1},

        {1,1,1, 1}, // UP
        {1,1,-1, 1},
        {-1,1,-1, 1},
        {-1,1,1, 1},
    };

    float normals[24][3] = {
    { 0, 0, -1 }, // BACK
    { 0, 0, -1 },
    { 0, 0, -1 },
    { 0, 0, -1 },

    { 0, 0, 1 }, // FRONT
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 0, 1 },

    { 1, 0, 0 }, // RIGHT
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },

    { 0, -1, 0 }, // DOWN
    { 0, -1, 0 },
    { 0, -1, 0 },
    { 0, -1, 0 },

    { -1, 0, 0 }, // LEFT
    { -1, 0, 0 },
    { -1, 0, 0 },
    { -1, 0, 0 },

    { 0, 1, 0 }, // UP
    { 0, 1, 0 },
    { 0, 1, 0 },
    { 0, 1, 0 }
    };
    float tangents[24][3] = {
    { 1, 0, 0 }, // BACK
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },

    { -1, 0, 0 }, // FRONT
    { -1, 0, 0 },
    { -1, 0, 0 },
    { -1, 0, 0 },

    { 0, 0, 1 }, // RIGHT
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 0, 1 },

    { 1, 0, 0 }, // DOWN
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },

    { 0, 0, -1 }, // LEFT
    { 0, 0, -1 },
    { 0, 0, -1 },
    { 0, 0, -1 },

    { 1, 0, 0 }, // UP
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 }
    };

    std::vector<Vertex> vertexList;

    vertexList.emplace_back(Vertex(pos[0], emptyColor, uv[0], normals[0], tangents[0]));
    vertexList.emplace_back(Vertex(pos[1], emptyColor, uv[1], normals[1], tangents[1]));
    vertexList.emplace_back(Vertex(pos[2], emptyColor, uv[2], normals[2], tangents[2]));
    vertexList.emplace_back(Vertex(pos[3], emptyColor, uv[3], normals[3], tangents[3]));
    vertexList.emplace_back(Vertex(pos[4], emptyColor, uv[4], normals[4], tangents[4]));
    vertexList.emplace_back(Vertex(pos[5], emptyColor, uv[5], normals[5], tangents[5]));
    vertexList.emplace_back(Vertex(pos[6], emptyColor, uv[6], normals[6], tangents[6]));
    vertexList.emplace_back(Vertex(pos[7], emptyColor, uv[7], normals[7], tangents[7]));
    vertexList.emplace_back(Vertex(pos[8], emptyColor, uv[8], normals[8], tangents[8]));
    vertexList.emplace_back(Vertex(pos[9], emptyColor, uv[9], normals[9], tangents[9]));
    vertexList.emplace_back(Vertex(pos[10], emptyColor, uv[10], normals[10], tangents[10]));
    vertexList.emplace_back(Vertex(pos[11], emptyColor, uv[11], normals[11], tangents[11]));
    vertexList.emplace_back(Vertex(pos[12], emptyColor, uv[12], normals[12], tangents[12]));
    vertexList.emplace_back(Vertex(pos[13], emptyColor, uv[13], normals[13], tangents[13]));
    vertexList.emplace_back(Vertex(pos[14], emptyColor, uv[14], normals[14], tangents[14]));
    vertexList.emplace_back(Vertex(pos[15], emptyColor, uv[15], normals[15], tangents[15]));
    vertexList.emplace_back(Vertex(pos[16], emptyColor, uv[16], normals[16], tangents[16]));
    vertexList.emplace_back(Vertex(pos[17], emptyColor, uv[17], normals[17], tangents[17]));
    vertexList.emplace_back(Vertex(pos[18], emptyColor, uv[18], normals[18], tangents[18]));
    vertexList.emplace_back(Vertex(pos[19], emptyColor, uv[19], normals[19], tangents[19]));
    vertexList.emplace_back(Vertex(pos[20], emptyColor, uv[20], normals[20], tangents[20]));
    vertexList.emplace_back(Vertex(pos[21], emptyColor, uv[21], normals[21], tangents[21]));
    vertexList.emplace_back(Vertex(pos[22], emptyColor, uv[22], normals[22], tangents[22]));
    vertexList.emplace_back(Vertex(pos[23], emptyColor, uv[23], normals[23], tangents[23]));

    std::vector<unsigned> indexList = {
        0,1,2,
        0,2,3,
        4,5,6,
        4,6,7,
        8,9,10,
        8,10,11,
        12,13,14,
        12,14,15,
        16,17,18,
        16,18,19,
        20,21,22,
        20,22,23
    };

    Mesh::Element element;
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertexList.size());
    element.NumIndices = static_cast<unsigned>(indexList.size());
    std::vector<Mesh::Element> elementList = { element };

    Mesh cube;
    cube.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());

    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(cube));
    asset->name = "CubePrimitive";
    myAssets.emplace(asset->name, asset);

    LOG(AssetManagerLog, Log, "Registered mesh asset {}", asset->name.string());
    return true;
}

bool AssetManager::RegisterRampPrimitive()
{
    float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    float uv[18][4][2] = { {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                           {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                           {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                           {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                           {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
    };

    float pos[18][4] = {
        {-1, 1, 1, 1}, // LEFT
        {-1, 1, -1, 1},
        {-1, -1, -1, 1},
        {-1, -1, 1, 1},

        {1, -1, -1, 1}, // DOWN
        {1, -1, 1, 1},
        {-1, -1, 1, 1},
        {-1, -1, -1, 1},

        {-1, 1, 1, 1}, // DIAGONAL
        {1, -1, 1, 1},
        {1, -1, -1, 1},
        {-1, 1, -1, 1},

        {-1, 1, 1, 1}, // FRONT
        {-1, -1, 1, 1},
        {1, -1, 1, 1},

        {1, -1, -1, 1}, // BACK
        {-1, -1, -1, 1},
        { -1, 1, -1, 1 },
    };

    float normals[18][3] = {
        { -1, 0, 0 }, // LEFT
        { -1, 0, 0 },
        { -1, 0, 0 },
        { -1, 0, 0 },

        { 0, -1, 0 }, // DOWN
        { 0, -1, 0 },
        { 0, -1, 0 },
        { 0, -1, 0 },

        { 1, 1, 0 }, // DIAGONAL
        { 1, 1, 0 },
        { 1, 1, 0 },
        { 1, 1, 0 },

        { 0, 0, 1 }, // FRONT
        { 0, 0, 1 },
        { 0, 0, 1 },

        { 0, 0, -1 }, // BACK
        { 0, 0, -1 },
        { 0, 0, -1 }
    };

    float tangents[18][3] = {
        { 0, 0, -1 }, // LEFT
        { 0, 0, -1 },
        { 0, 0, -1 },
        { 0, 0, -1 },

        { 0, 0, 1 }, // DOWN
        { 0, 0, 1 },
        { 0, 0, 1 },
        { 0, 0, 1 },

        { 0, 0, 1 }, // DIAGONAL
        { 0, 0, 1 },
        { 0, 0, 1 },
        { 0, 0, 1 },

        { -1, 0, 0 }, // FRONT
        { -1, 0, 0 },
        { -1, 0, 0 },

        { 1, 0, 0 }, // BACK
        { 1, 0, 0 },
        { 1, 0, 0 }
    };

    std::vector<Vertex> vertexList;
    vertexList.emplace_back(Vertex(pos[0], emptyColor, uv[0], normals[0], tangents[0]));
    vertexList.emplace_back(Vertex(pos[1], emptyColor, uv[1], normals[1], tangents[1]));
    vertexList.emplace_back(Vertex(pos[2], emptyColor, uv[2], normals[2], tangents[2]));
    vertexList.emplace_back(Vertex(pos[3], emptyColor, uv[3], normals[3], tangents[3]));
    vertexList.emplace_back(Vertex(pos[4], emptyColor, uv[4], normals[4], tangents[4]));
    vertexList.emplace_back(Vertex(pos[5], emptyColor, uv[5], normals[5], tangents[5]));
    vertexList.emplace_back(Vertex(pos[6], emptyColor, uv[6], normals[6], tangents[6]));
    vertexList.emplace_back(Vertex(pos[7], emptyColor, uv[7], normals[7], tangents[7]));
    vertexList.emplace_back(Vertex(pos[8], emptyColor, uv[8], normals[8], tangents[8]));
    vertexList.emplace_back(Vertex(pos[9], emptyColor, uv[9], normals[9], tangents[9]));
    vertexList.emplace_back(Vertex(pos[10], emptyColor, uv[10], normals[10], tangents[10]));
    vertexList.emplace_back(Vertex(pos[11], emptyColor, uv[11], normals[11], tangents[11]));
    vertexList.emplace_back(Vertex(pos[12], emptyColor, uv[12], normals[12], tangents[12]));
    vertexList.emplace_back(Vertex(pos[13], emptyColor, uv[13], normals[13], tangents[13]));
    vertexList.emplace_back(Vertex(pos[14], emptyColor, uv[14], normals[14], tangents[14]));
    vertexList.emplace_back(Vertex(pos[15], emptyColor, uv[15], normals[15], tangents[15]));
    vertexList.emplace_back(Vertex(pos[16], emptyColor, uv[16], normals[16], tangents[16]));
    vertexList.emplace_back(Vertex(pos[17], emptyColor, uv[17], normals[17], tangents[17]));

    std::vector<unsigned> indexList = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        15, 16, 17,
    };

    Mesh::Element element;
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertexList.size());
    element.NumIndices = static_cast<unsigned>(indexList.size());
    std::vector<Mesh::Element> elementList = { element };

    Mesh ramp;
    ramp.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());

    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(ramp));
    asset->name = "RampPrimitive";
    myAssets.emplace(asset->name, asset);

    LOG(AssetManagerLog, Log, "Registered mesh asset {}", asset->name.string());
    return true;
}
