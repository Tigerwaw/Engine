#pragma once

class Texture2D;

class GBuffer
{
public:
	void CreateGBuffer(unsigned aWidth, unsigned aHeight);
	std::shared_ptr<Texture> GetAlbedo() { return myAlbedo; }
	std::shared_ptr<Texture> GetMaterial() { return myMaterial; }
	std::shared_ptr<Texture> GetEffects() { return myEffects; }
	std::shared_ptr<Texture> GetWorldNormal() { return myWorldNormal; }
	std::shared_ptr<Texture> GetWorldPosition() { return myWorldPosition; }
private:
	std::shared_ptr<Texture> myAlbedo;
	std::shared_ptr<Texture> myMaterial;
	std::shared_ptr<Texture> myEffects;
	std::shared_ptr<Texture> myWorldNormal;
	std::shared_ptr<Texture> myWorldPosition;
};

