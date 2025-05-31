#include "GraphicsEngine.pch.h"
#include "RenderParticles.h"

#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "ComponentSystem/Components/Graphics/ParticleSystem.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"

RenderParticles::RenderParticles(std::shared_ptr<ParticleSystem> aParticleSystem)
{
	particleSystem = aParticleSystem;
    transform = aParticleSystem->gameObject->GetComponent<Transform>()->GetWorldMatrix();
}

void RenderParticles::Execute()
{
    GraphicsEngine& gfx = GraphicsEngine::Get();

    ObjectBuffer objBufferData;
    objBufferData.World = transform;
    objBufferData.WorldInvT = transform.GetFastInverse().GetTranspose();
    gfx.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    for (auto& emitter : particleSystem->GetEmitters())
    {
        gfx.RenderParticleEmitter(emitter);
    }
}

void RenderParticles::Destroy()
{
	particleSystem = nullptr;
}
