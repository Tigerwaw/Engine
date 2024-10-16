#include "Enginepch.h"
#include "AIPollingController.h"
#include "../PollingStation.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

void AIPollingController::Start()
{
}

CU::Vector3f AIPollingController::GetDirection(CU::Vector3f aCurrentPosition)
{
    CU::Vector3f goal;
    bool playerIsNearComp1 = false;
    bool playerIsNearComp2 = false;
    bool playerIsNearComp3 = false;

    if (auto player = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Player"); player)
    {
        goal = player->GetComponent<Transform>()->GetTranslation(true);
    }

    if (auto comp1 = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Computer1"); comp1)
    {
        playerIsNearComp1 = (goal - comp1->GetComponent<Transform>()->GetTranslation(true)).LengthSqr() < 6000.0f;
    }

    if (auto comp2 = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Computer2"); comp2)
    {
        playerIsNearComp2 = (goal - comp2->GetComponent<Transform>()->GetTranslation(true)).LengthSqr() < 6000.0f;
    }

    if (auto comp3 = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Computer3"); comp3)
    {
        playerIsNearComp3 = (goal - comp3->GetComponent<Transform>()->GetTranslation(true)).LengthSqr() < 6000.0f;
    }

    if (!playerIsNearComp1 && !playerIsNearComp2 && !playerIsNearComp3) return CU::Vector3f();

    CU::Vector3f diff = goal - aCurrentPosition;

    if (diff.LengthSqr() < 1.0f)
    {
        return CU::Vector3f();
    }

    return diff;
}
