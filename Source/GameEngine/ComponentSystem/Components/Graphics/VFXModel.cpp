#include "Enginepch.h"
#include "VFXModel.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/Utility/SerializationUtils.hpp"

VFXModel::~VFXModel()
{
}

VFXModel::VFXModel(VFXData aVFXData)
{
	SetLifetime(aVFXData.lifetime);
	SetEventTrigger(aVFXData.eventTrigger);
	SetWorldspaceSprite(aVFXData.isBillboard, aVFXData.isSpritesheet, aVFXData.spritesheetSize);
}

void VFXModel::Start()
{
}

void VFXModel::Update()
{
	myCurrentTimeAlive += Engine::GetInstance().GetTimer().GetDeltaTime();
	myCustomShaderParameters.y = myCurrentTimeAlive;

	if (myLifetime <= 0)
	{
		myCustomShaderParameters.x = 0;
	}
	else
	{
		myCustomShaderParameters.x = myCurrentTimeAlive / myLifetime;
	}

	std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
	if (model)
	{
		model->SetCastShadows(myCastShadows);
		model->SetCustomShaderData_2(myCustomShaderParameters);

		if (myIsBillboard)
		{
			// Transform look at camera
		}

		if (myLifetime > 0 && myCurrentTimeAlive >= myLifetime)
		{
			SetActive(false);
			model->SetActive(false);
		}
	}
}

void VFXModel::PlayVFX()
{
	myCurrentTimeAlive = 0;
	SetActive(true);
	std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
	if (model)
	{
		model->SetActive(true);
	}
}

void VFXModel::ReceiveEvent(const GameObjectEvent& aEvent)
{
	if (aEvent.type == myEventTrigger)
	{
		PlayVFX();
	}
}

void VFXModel::SetWorldspaceSprite(bool aIsBillboard, bool aIsSpritesheet, CU::Vector2f aSpritesheetSize)
{
    myIsBillboard = aIsBillboard;

	if (aIsSpritesheet)
	{
		myCustomShaderParameters.z = aSpritesheetSize.x;
		myCustomShaderParameters.w = aSpritesheetSize.y;
	}
}

bool VFXModel::Serialize(nl::json& outJsonObject)
{
	outJsonObject;
	return false;
}

bool VFXModel::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("Lifetime"))
	{
		SetLifetime(aJsonObject["Lifetime"].get<float>());
	};

	if (aJsonObject.contains("CastShadows"))
	{
		SetCastShadows(aJsonObject["CastShadows"].get<bool>());
	}

	if (aJsonObject.contains("EventTrigger"))
	{
		SetEventTrigger(static_cast<GameObjectEventType>(aJsonObject["EventTrigger"].get<unsigned>()));
	}

	bool isBillboard = myIsBillboard;
	bool isSpritesheet = false;
	CU::Vector2f spritesheetSize;

	if (aJsonObject.contains("IsBillboard"))
	{
		isBillboard = aJsonObject["IsBillboard"].get<bool>();
	}

	if (aJsonObject.contains("IsSpritesheet"))
	{
		isSpritesheet = aJsonObject["IsSpritesheet"].get<bool>();
	}

	if (aJsonObject.contains("SpritesheetSize"))
	{
		spritesheetSize = Utility::DeserializeVector2<float>(aJsonObject["SpritesheetSize"]);
	}

	SetWorldspaceSprite(isBillboard, isSpritesheet, spritesheetSize);

	return true;
}
