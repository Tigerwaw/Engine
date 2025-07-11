#pragma once

class Mesh;
class Material;
class DynamicVertexBuffer;
class Text;
class ParticleEmitter;
class TrailEmitter;

class Drawer
{
public:
	void RenderQuad();
	void RenderMesh(const Mesh& aMesh, const std::vector<std::shared_ptr<Material>>& aMaterialList);
	void RenderMeshShadow(const Mesh& aMesh);
	void RenderInstancedMesh(const Mesh& aMesh, unsigned aMeshCount, const std::vector<std::shared_ptr<Material>>& aMaterialList, DynamicVertexBuffer& aInstanceBuffer);
	void RenderInstancedMeshShadow(const Mesh& aMesh, unsigned aMeshCount, DynamicVertexBuffer& aInstanceBuffer);
	void RenderSprite();
	void RenderText(const Text& aText);
	void RenderDebugLines(DynamicVertexBuffer& aDynamicBuffer, unsigned aLineAmount);
	void RenderParticleEmitter(ParticleEmitter& aParticleEmitter);
	void RenderTrailEmitter(TrailEmitter& aTrailEmitter);
};