#include "Enginepch.h"
#include "PollingStation.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

PollingStation::PollingStation()
{
}

PollingStation::~PollingStation()
{
}

void PollingStation::Update()
{
    bool isPlayerHackingComputer1 = false;
    bool isPlayerHackingComputer2 = false;
    bool isPlayerHackingComputer3 = false;

    if (auto player = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Player"); player)
    {
        myPlayerPosition = player->GetComponent<Transform>()->GetTranslation(true);
    }

    if (auto comp1 = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Computer1"); comp1)
    {
        CU::Vector3f computer1Position = comp1->GetComponent<Transform>()->GetTranslation(true);
        isPlayerHackingComputer1 = (myPlayerPosition - computer1Position).LengthSqr() < 6000.0f;
    }

    if (auto comp2 = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Computer2"); comp2)
    {
        CU::Vector3f computer2Position = comp2->GetComponent<Transform>()->GetTranslation(true);
        isPlayerHackingComputer2 = (myPlayerPosition - computer2Position).LengthSqr() < 6000.0f;
    }

    if (auto comp3 = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Computer3"); comp3)
    {
        CU::Vector3f computer3Position = comp3->GetComponent<Transform>()->GetTranslation(true);
        isPlayerHackingComputer3 = (myPlayerPosition - computer3Position).LengthSqr() < 6000.0f;
    }

    myIsPlayerHackingComputer = isPlayerHackingComputer1 || isPlayerHackingComputer2 || isPlayerHackingComputer3;
}
