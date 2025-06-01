#include "GraphicsEngine.pch.h"
#include "RenderTrail.h"

#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderTrail::RenderTrail(const TrailData& aTrailData)
{
    myData = aTrailData;
}

void RenderTrail::Execute()
{
    GraphicsEngine& gfx = GraphicsEngine::Get();

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.WorldInvT = myData.transform.GetFastInverse().GetTranspose();
    gfx.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    for (auto& emitter : myData.emitters)
    {
        gfx.RenderTrailEmitter(emitter);
    }
}

void RenderTrail::Destroy()
{
    myData.emitters.~vector();
}
