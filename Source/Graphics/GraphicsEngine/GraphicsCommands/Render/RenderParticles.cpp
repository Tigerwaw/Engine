#include "GraphicsEngine.pch.h"
#include "RenderParticles.h"

#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderParticles::RenderParticles(const RenderParticlesData& aParticleSystemData)
{
    myData = aParticleSystemData;
}

void RenderParticles::Execute()
{
    GraphicsEngine& gfx = GraphicsEngine::Get();

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.WorldInvT = myData.transform.GetFastInverse().GetTranspose();
    gfx.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    for (auto& emitter : myData.emitters)
    {
        gfx.RenderParticleEmitter(emitter);
    }
}

void RenderParticles::Destroy()
{
    myData.emitters.~vector();
}
