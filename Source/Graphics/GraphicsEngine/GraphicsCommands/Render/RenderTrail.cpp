#include "GraphicsEngine.pch.h"
#include "RenderTrail.h"

#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "ComponentSystem/Components/Graphics/TrailSystem.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"

RenderTrail::RenderTrail(std::shared_ptr<TrailSystem> aTrailSystem)
{
    trailSystem = aTrailSystem;
    transform = aTrailSystem->gameObject->GetComponent<Transform>()->GetWorldMatrix();
}

void RenderTrail::Execute()
{
    GraphicsEngine& gfx = GraphicsEngine::Get();

    ObjectBuffer objBufferData;
    objBufferData.World = transform;
    objBufferData.WorldInvT = transform.GetFastInverse().GetTranspose();
    gfx.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    for (auto& emitter : trailSystem->GetEmitters())
    {
        gfx.RenderTrailEmitter(emitter);
    }
}

void RenderTrail::Destroy()
{
    trailSystem = nullptr;
}
