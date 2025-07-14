#include "Enginepch.h"
#include "NavMeshAsset.h"

std::vector<NavPolygon> CreateNavPolygons(const TGA::FBX::NavMesh& tgaNavMesh);
std::vector<NavNode> CreateNavNodes(const std::vector<NavPolygon>& navPolygons);
std::vector<NavPortal> CreateNavPortals(const std::vector<NavPolygon>& navPolygons, const std::vector<NavNode>& aNavNodes);

bool NavMeshAsset::Load()
{
    TGA::FBX::NavMesh tgaNavMesh;
    TGA::FBX::Importer::LoadNavMesh(GetPath(), tgaNavMesh, true);

    // Create Nav Polygons.
    std::vector<NavPolygon> navPolygons = CreateNavPolygons(tgaNavMesh);

    // Create Nav nodes with connections between eachother.
    std::vector<NavNode> navNodes = CreateNavNodes(navPolygons);

    // Create Nav Portals.
    std::vector<NavPortal> navPortals = CreateNavPortals(navPolygons, navNodes);

    for (int portalIndex = 0; portalIndex < static_cast<int>(navPortals.size()); ++portalIndex)
    {
        NavPortal& portal = navPortals[portalIndex];
        navNodes[portal.nodes[0]].portals.emplace_back(portalIndex);
    }

    NavMesh navMesh;
    navMesh.Init(navNodes, navPolygons, navPortals);
    Math::Vector3f boxExtents = { tgaNavMesh.BoxSphereBounds.BoxExtents[0], tgaNavMesh.BoxSphereBounds.BoxExtents[1], tgaNavMesh.BoxSphereBounds.BoxExtents[2] };
    Math::Vector3f boxCenter = { tgaNavMesh.BoxSphereBounds.Center[0], tgaNavMesh.BoxSphereBounds.Center[1], tgaNavMesh.BoxSphereBounds.Center[2] };
    navMesh.SetBoundingBox(boxCenter, boxExtents * 2.0f);

    navmesh = std::make_shared<NavMesh>(std::move(navMesh));
    return true;
}

bool NavMeshAsset::Unload()
{
    navmesh = nullptr;
    return true;
}

std::vector<NavPolygon> CreateNavPolygons(const TGA::FBX::NavMesh& tgaNavMesh)
{
    std::vector<NavPolygon> navPolygons;
    for (auto& tgaChunk : tgaNavMesh.Chunks)
    {
        navPolygons.reserve(navPolygons.size() + tgaChunk.Polygons.size());

        for (auto& tgaPolygon : tgaChunk.Polygons)
        {
            assert(tgaPolygon.Indices.size() < 4 && "Navmesh isn't triangulated >:((");
            NavPolygon& navPolygon = navPolygons.emplace_back();
            for (std::size_t i = 0; i < tgaPolygon.Indices.size(); ++i)
            {
                const int vertexIndex = tgaPolygon.Indices[i];

                Math::Vector3f vertexPos = { tgaChunk.Vertices[vertexIndex].Position[0], tgaChunk.Vertices[vertexIndex].Position[1], tgaChunk.Vertices[vertexIndex].Position[2] };
                navPolygon.vertexPositions[i] = vertexPos;
            }
        }
    }

    return navPolygons;
}

std::vector<NavNode> CreateNavNodes(const std::vector<NavPolygon>& navPolygons)
{
    std::vector<NavNode> navNodes(navPolygons.size());

    for (int i = 0; i < static_cast<int>(navPolygons.size()); ++i)
    {
        NavNode& navNode = navNodes.emplace_back();
        Math::Vector3f pos;
        int numVertices = static_cast<int>(navPolygons[i].vertexPositions.size());
        for (int vertexPos = 0; vertexPos < numVertices; vertexPos++)
        {
            pos += navPolygons[i].vertexPositions[vertexPos];
        }

        pos.x /= numVertices;
        pos.y /= numVertices;
        pos.z /= numVertices;

        navNode.position = pos;
    }

    return navNodes;
}

std::vector<NavPortal> CreateNavPortals(const std::vector<NavPolygon>& navPolygons, const std::vector<NavNode>& aNavNodes)
{
    std::vector<NavPortal> navPortals;

    for (int i = 0; i < static_cast<int>(navPolygons.size()); ++i)
    {
        const NavPolygon& navPoly1 = navPolygons[i];

        for (int j = 0; j < static_cast<int>(navPolygons.size()); ++j)
        {
            if (i == j) continue;

            const NavPolygon& navPoly2 = navPolygons[j];

            std::array<Math::Vector3f, 2> sharedVertices;
            int sharedCount = 0;

            for (int vertexPos = 0; vertexPos < static_cast<int>(navPoly2.vertexPositions.size()); vertexPos++)
            {
                const Math::Vector3f& vertexPosToTest = navPoly2.vertexPositions[vertexPos];
                if (std::find(navPoly1.vertexPositions.begin(), navPoly1.vertexPositions.end(), vertexPosToTest) != navPoly1.vertexPositions.end())
                {
                    assert(sharedCount < 2 && "More than 2 shared vertices means mesh is not triangulated, or polygon is really damn small");
                    sharedVertices[sharedCount++] = vertexPosToTest;
                }
            }

            if (sharedCount == 2) // Polygons share an edge
            {
                NavPortal& navPortal = navPortals.emplace_back();
                navPortal.nodes[0] = i;
                navPortal.nodes[1] = j;
                navPortal.vertices[0] = sharedVertices[0];
                navPortal.vertices[1] = sharedVertices[1];
                navPortal.cost = (aNavNodes[i].position - aNavNodes[j].position).Length();
            }
        }
    }

    return navPortals;
}
