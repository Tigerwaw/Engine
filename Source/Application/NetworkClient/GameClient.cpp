#include <Enginepch.h>
#include "GameClient.h"
#include <iostream>
#include <WinSock2.h>

#include "NetworkEngine/NetMessage.h"
#include "NetworkShared/NetMessages/NetMessage_Text.h"
#include "NetworkShared/NetMessages/NetMessage_RequestConnect.h"
#include "NetworkShared/NetMessages/NetMessage_AcceptConnect.h"
#include "NetworkShared/NetMessages/NetMessage_Disconnect.h"
#include "NetworkShared/NetMessages/NetMessage_RequestHandshake.h"
#include "NetworkShared/NetMessages/NetMessage_AcceptHandshake.h"
#include "NetworkShared/NetMessages/NetMessage_CreateCharacter.h"
#include "NetworkShared/NetMessages/NetMessage_RemoveCharacter.h"
#include "NetworkShared/NetMessages/NetMessage_Position.h"
#include "NetworkShared/NetMessages/NetMessage_Test.h"

#include <GameEngine/Engine.h>
#include <Time/Timer.h>
#include <AssetManager.h>
#include <SceneHandler/SceneHandler.h>
#include <GameEngine/ComponentSystem/GameObject.h>
#include <GameEngine/ComponentSystem/Components/Transform.h>
#include <GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h>
#include <GameEngine/ComponentSystem/Components/Graphics/Model.h>
#include <GameEngine/ComponentSystem/Components/Physics/Colliders/BoxCollider.h>
#include <GameEngine/Input/InputHandler.h>

#include "Controller.h"

void GameClient::Update()
{
    if (Engine::Get().GetInputHandler().GetBinaryAction("ToggleLerp"))
    {
        myShouldLerpPositions = !myShouldLerpPositions;
    }

    ClientBase::Update();
    if (!myHasEstablishedConnection)
    {
        SendConnectionRequest("");
        return;
    }

    if (myShouldLerpPositions)
    {
        double currentTime = Engine::Get().GetTimer().GetTimeSinceEpoch();

        for (auto& [clientID, positionDataArray] : myObjectIDPositionHistory)
        {
            if (positionDataArray.Size() >= 2)
            {
                PositionData lastPos = positionDataArray.Peek_Front();
                PositionData nextPos = positionDataArray.Peek_Next();

                float messageTimeDiff = static_cast<float>(currentTime - nextPos.clientTimestamp);
                float timestampDiff = static_cast<float>(nextPos.serverTimestamp - lastPos.serverTimestamp);
                if (timestampDiff == 0.0f) return;

                float t = messageTimeDiff / timestampDiff;
                if (t >= 1.0f)
                {
                    positionDataArray.Pop_Front();
                }

                Math::Vector3f lerpedPosition = Math::Vector3f::Lerp(lastPos.position, nextPos.position, t);
                if (auto go = Engine::Get().GetSceneHandler().FindGameObjectByNetworkID(clientID))
                {
                    go->GetComponent<Transform>()->SetTranslation(lerpedPosition);
                }
            }
        }
    }
}

void GameClient::SendHandshakeRequest() const
{
    NetMessage_RequestHandshake msg;
    NetBuffer sendBuffer;
    msg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer);
}

void GameClient::SendConnectionRequest(const std::string& aUsername) const
{
    NetMessage_RequestConnect msg;
    msg.SetUsername(aUsername);
    NetBuffer sendBuffer;
    msg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer);
}

void GameClient::SendTextMessage(const std::string& aMessage) const
{
    NetMessage_Text textMsg;
    textMsg.SetData(aMessage);
    NetBuffer sendBuffer;
    textMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer);
}

void GameClient::SendDisconnectMessage() const
{
    NetMessage_Disconnect disconnectMsg;
    NetBuffer sendBuffer;
    disconnectMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer);
}

void GameClient::SendPositionMessage(const Math::Vector3f& aPosition) const
{
    NetMessage_Position positionMsg;
    positionMsg.SetPosition(aPosition);
    NetBuffer sendBuffer;
    positionMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer);

    printf("\nSent position x: %f, y: %f, z: %f", aPosition.x, aPosition.y, aPosition.z);
}

NetMessage* GameClient::ReceiveMessage(const NetBuffer& aBuffer) const
{
    NetMessageType receivedMessageType = static_cast<NetMessageType>(aBuffer.GetBuffer()[0]);
    //printf("NetMessageType: %i\n", static_cast<int>(receivedMessageType));

    switch (receivedMessageType)
    {
    case NetMessageType::RequestConnect:
        return new NetMessage_RequestConnect();
    case NetMessageType::AcceptConnect:
        return new NetMessage_AcceptConnect();
    case NetMessageType::Disconnect:
        return new NetMessage_Disconnect();
    case NetMessageType::Text:
        return new NetMessage_Text();
    case NetMessageType::AcceptHandshake:
        return new NetMessage_AcceptHandshake();
    case NetMessageType::CreateCharacter:
        return new NetMessage_CreateCharacter();
    case NetMessageType::RemoveCharacter:
        return new NetMessage_RemoveCharacter();
    case NetMessageType::Position:
        return new NetMessage_Position();
    case NetMessageType::Test:
        return new NetMessage_Test();
    default:
        return nullptr;
    }
}

void GameClient::HandleMessage(NetMessage* aMessage)
{
    NetMessageType type = aMessage->GetType();
    switch (type)
    {
    case NetMessageType::AcceptConnect:
        HandleMessage_AcceptConnect();
        break;
    case NetMessageType::Disconnect:
        HandleMessage_Disconnect(*static_cast<NetMessage_Disconnect*>(aMessage));
        break;
    case NetMessageType::Text:
        HandleMessage_Text(*static_cast<NetMessage_Text*>(aMessage));
        break;
    case NetMessageType::AcceptHandshake:
        HandleMessage_AcceptHandshake();
        break;
    case NetMessageType::CreateCharacter:
        HandleMessage_CreateCharacter(*static_cast<NetMessage_CreateCharacter*>(aMessage));
        break;
    case NetMessageType::RemoveCharacter:
        HandleMessage_RemoveCharacter(*static_cast<NetMessage_RemoveCharacter*>(aMessage));
        break;
    case NetMessageType::Position:
        HandleMessage_Position(*static_cast<NetMessage_Position*>(aMessage));
        break;
    case NetMessageType::Test:
        HandleMessage_Test(*static_cast<NetMessage_Test*>(aMessage));
        break;
    }
}

void GameClient::HandleMessage_Disconnect(NetMessage_Disconnect& aMessage)
{
    printf("\n[%s] has left the game.", aMessage.GetData().data());
}

void GameClient::HandleMessage_Text(NetMessage_Text& aMessage)
{
    printf("\n%s", aMessage.GetData().data());
}

void GameClient::HandleMessage_CreateCharacter(NetMessage_CreateCharacter& aMessage)
{
    std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
    go->SetNetworkID(aMessage.GetNetworkID());
    go->AddComponent<Transform>(aMessage.GetPosition());
    go->AddComponent<BoxCollider>(Math::Vector3f(50.0f, 100.0f, 50.0f), Math::Vector3f(0.0f, 90.0f, 0.0f));

    auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ColorBlue.json")->material);
    model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);

    Engine::Get().GetSceneHandler().Instantiate(go);
}

void GameClient::HandleMessage_RemoveCharacter(NetMessage_RemoveCharacter& aMessage)
{
    auto& sceneHandler = Engine::Get().GetSceneHandler();
    if (auto go = sceneHandler.FindGameObjectByNetworkID(aMessage.GetNetworkID()))
    {
        Engine::Get().GetSceneHandler().Destroy(go);
    }
}

void GameClient::HandleMessage_Position(NetMessage_Position& aMessage)
{
    if (myShouldLerpPositions)
    {
        PositionData data;
        data.position = aMessage.GetPosition();
        data.serverTimestamp = aMessage.GetTimestamp();
        data.clientTimestamp = Engine::Get().GetTimer().GetTimeSinceEpoch();
        myObjectIDPositionHistory[aMessage.GetNetworkID()].Push_back(data);
    }
    else
    {
        if (auto go = Engine::Get().GetSceneHandler().FindGameObjectByNetworkID(aMessage.GetNetworkID()))
        {
            go->GetComponent<Transform>()->SetTranslation(aMessage.GetPosition());
        }
    }
}

void GameClient::HandleMessage_Test(NetMessage_Test& aMessage)
{
    printf("%i\n", aMessage.GetInt());
}