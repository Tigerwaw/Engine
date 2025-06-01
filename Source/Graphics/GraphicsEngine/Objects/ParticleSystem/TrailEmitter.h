#pragma once
#include "Objects/Vertices/TrailVertex.h"
#include "TrailEmitterSettings.h"

class TrailEmitter
{
	friend class GraphicsEngine;
	friend class TrailSystem;

public:
	TrailEmitter();
	virtual ~TrailEmitter();
	void Update(Math::Vector3f aFollowTarget, float aDeltaTime);

	void SetMaterial(std::shared_ptr<Material> aMaterial) { myMaterial = aMaterial; }
	std::shared_ptr<Material> GetMaterial() { return myMaterial; }

	const TrailEmitterSettings& GetEmitterSettings() const { return mySettings; }
	const unsigned GetCurrentLength() const { return myCurrentLength; }

protected:
	virtual void InitTrailVertex(TrailVertex& aTrailVertex);
	virtual void UpdateTrailVertex(TrailVertex& aTrailVertex, float aDeltaTime, unsigned aIndex);
	virtual void InitInternal();
	std::vector<TrailVertex> myTrailVertices;
private:
	std::shared_ptr<DynamicVertexBuffer> myVertexBuffer;
	std::shared_ptr<Material> myMaterial;
	TrailEmitterSettings mySettings;
	std::vector<Math::Vector3f> myPreviousPositions;
	unsigned myCurrentLength = 0;
};

