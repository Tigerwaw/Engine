#include "Enginepch.h"
#include "TrailSystem.h"
#include <d3d11.h>

#include "GameEngine/Engine.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Utility/SerializationUtils.hpp"

TrailSystem::~TrailSystem()
{
}

void TrailSystem::Start()
{
}

void TrailSystem::Update()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();

	for (auto& emitter : myEmitters)
	{
		emitter.Update(gameObject->GetComponent<Transform>()->GetTranslation(true), dt);
	}
}

TrailEmitter& TrailSystem::AddEmitter(const TrailEmitterSettings& aSettings)
{
	TrailEmitter& emitter = myEmitters.emplace_back(TrailEmitter());
	emitter.mySettings = aSettings;
	return emitter;
}

bool TrailSystem::Serialize(nl::json&)
{
    return false;
}

bool TrailSystem::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("Emitters"))
	{
		for (auto& emitter : aJsonObject["Emitters"])
		{
			TrailEmitterSettings teSettings;

			if (emitter.contains("Vertices"))
			{
				teSettings.Vertices = emitter["Vertices"].get<unsigned>();
			}

			if (emitter.contains("Length"))
			{
				teSettings.Length = emitter["Length"].get<unsigned>();
			}

			if (emitter.contains("Width"))
			{
				for (auto& key : emitter["Width"])
				{
					teSettings.Width.AddKey(key["Time"].get<float>(), key["Value"].get<float>());
				}
			}

			if (emitter.contains("Color"))
			{
				for (auto& key : emitter["Color"])
				{
					teSettings.Color.AddKey(key["Time"].get<float>(), Utility::DeserializeVector4<float>(key["Value"]));
				}
			}

			if (emitter.contains("ChannelMask"))
			{
				teSettings.ChannelMask = Utility::DeserializeVector4<float>(emitter["ChannelMask"]);
			}

			TrailEmitter& te = AddEmitter(teSettings);

			if (emitter.contains("Material"))
			{
				te.SetMaterial(AssetManager::Get().GetAsset<MaterialAsset>(emitter["Material"].get<std::string>())->material);
			}

			te.InitInternal();
		}
	}

	return true;
}
