#include "Enginepch.h"
#include "HealthComponent.h"
#include <algorithm>
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/GameObject.h"

void HealthComponent::Start()
{
    myMaxHealth = 100.0f;
    myHealth = myMaxHealth;
}

void HealthComponent::Update()
{
}

void HealthComponent::TakeDamage(float aDamage)
{
    myHealth = std::fmaxf(myHealth - aDamage, 0.0f);
    UpdateMaterial();
}

void HealthComponent::Heal(float aHP)
{
    myHealth = std::fminf(myHealth + aHP, myMaxHealth);
    UpdateMaterial();
}

void HealthComponent::UpdateMaterial()
{
    float percentage = myHealth / myMaxHealth;
    if (percentage < 0.3f)
    {
        gameObject->GetComponent<AnimatedModel>()->SetMaterialOnSlot(0, AssetManager::Get().GetAsset<MaterialAsset>("MAT_ColorRed.json")->material);
    }
    else if (percentage < 0.6f)
    {
        gameObject->GetComponent<AnimatedModel>()->SetMaterialOnSlot(0, AssetManager::Get().GetAsset<MaterialAsset>("MAT_ColorYellow.json")->material);
    }
    else
    {
        gameObject->GetComponent<AnimatedModel>()->SetMaterialOnSlot(0, AssetManager::Get().GetAsset<MaterialAsset>("MAT_ColorGreen.json")->material);
    }
}