#include <Enginepch.h>
#include "GameClient.h"
#include <iostream>
#include <WinSock2.h>

#include "NetworkEngine/NetMessage.h"
#include "NetworkShared/NetMessages/NetMessage_Text.h"
#include "NetworkShared/NetMessages/NetMessage_Connect.h"
#include "NetworkShared/NetMessages/NetMessage_Disconnect.h"
#include "NetworkShared/NetMessages/NetMessage_RequestHandshake.h"
#include "NetworkShared/NetMessages/NetMessage_AcceptHandshake.h"
#include "NetworkShared/NetMessages/NetMessage_CreateCharacter.h"
#include "NetworkShared/NetMessages/NetMessage_Position.h"

#include <GameEngine/Engine.h>
#include <Time/Timer.h>
#include <AssetManager.h>
#include <SceneHandler/SceneHandler.h>
#include <GameEngine/ComponentSystem/GameObject.h>
#include <GameEngine/ComponentSystem/Components/Transform.h>
#include <GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h>
#include <GameEngine/ComponentSystem/Components/Graphics/Model.h>

#include "Controller.h"

GameClient::GameClient()
{
    StartReceive(this, "10.250.224.90");
    printf("\nWaiting for server...");
}

void GameClient::SendInput(std::string aMessage)
{
    if (!myHasEstablishedConnection)
    {
        SendConnectMessage(aMessage);
        myHasEstablishedConnection = true;
    }
    else
    {
        if (aMessage == "quit")
        {
            SendDisconnectMessage();
        }
        else
        {
            SendTextMessage(aMessage);
        }
    }
}

void GameClient::Update()
{
    if (!myHasEstablishedConnection) return;

    //float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
    //if (dt > (1.0f / 60.0f))
    //{
    //    
    //}

    if (!myPlayer) return;
    if (auto transform = myPlayer->GetComponent<Transform>())
    {
        CU::Vector3f currentPos = transform->GetTranslation();
        if (myLastPosition != currentPos)
        {
            SendPositionMessage(currentPos);
            myLastPosition = currentPos;
        }
    }
}

void GameClient::SendHandshakeRequest() const
{
    NetMessage_RequestHandshake msg;
    NetBuffer sendBuffer;
    msg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

void GameClient::SendTextMessage(const std::string& aMessage) const
{
    NetMessage_Text textMsg;
    textMsg.SetData(aMessage);
    NetBuffer sendBuffer;
    textMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

void GameClient::SendConnectMessage(const std::string& aUsername) const
{
    NetMessage_Connect connectMsg;
    connectMsg.SetUsername(aUsername);
    NetBuffer sendBuffer;
    connectMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

void GameClient::SendDisconnectMessage() const
{
    NetMessage_Disconnect disconnectMsg;
    NetBuffer sendBuffer;
    disconnectMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

void GameClient::SendPositionMessage(const CU::Vector3f& aPosition) const
{
    NetMessage_Position positionMsg;
    positionMsg.SetPosition(aPosition);
    NetBuffer sendBuffer;
    positionMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());

    printf("\nSent position x: %f, y: %f, z: %f", aPosition.x, aPosition.y, aPosition.z);
}

NetMessage* GameClient::ReceiveMessage(const NetBuffer& aBuffer) const
{
    NetMessageType receivedMessageType = static_cast<NetMessageType>(aBuffer.GetBuffer()[0]);

    switch (receivedMessageType)
    {
    case NetMessageType::Connect:
    {
        return new NetMessage_Connect();
        break;
    }
    case NetMessageType::Disconnect:
    {
        return new NetMessage_Disconnect();
        break;
    }
    case NetMessageType::Text:
    {
        return new NetMessage_Text();
        break;
    }
    case NetMessageType::HandshakeAccept:
    {
        return new NetMessage_AcceptHandshake();
        break;
    }
    case NetMessageType::CreateCharacter:
    {
        return new NetMessage_CreateCharacter();
        break;
    }
    case NetMessageType::Position:
    {
        return new NetMessage_Position();
        break;
    }
    default:
        return nullptr;
        break;
    }
}

void GameClient::HandleMessage(NetMessage* aMessage)
{
    NetMessageType type = aMessage->GetType();
    switch (type)
    {
    case NetMessageType::Connect:
        HandleMessage_Connect(*static_cast<NetMessage_Connect*>(aMessage));
        break;
    case NetMessageType::Disconnect:
        HandleMessage_Disconnect(*static_cast<NetMessage_Disconnect*>(aMessage));
        break;
    case NetMessageType::Text:
        HandleMessage_Text(*static_cast<NetMessage_Text*>(aMessage));
        break;
    case NetMessageType::HandshakeAccept:
    {
        HandleMessage_HandshakeAccept();
        break;
    }
    case NetMessageType::CreateCharacter:
        HandleMessage_CreateCharacter(*static_cast<NetMessage_CreateCharacter*>(aMessage));
        break;
    case NetMessageType::Position:
        HandleMessage_Position(*static_cast<NetMessage_Position*>(aMessage));
        break;
    default:
        break;
    }
}

void GameClient::HandleMessage_Connect(NetMessage_Connect& aMessage)
{
    printf("\n[%s] has joined the game!", aMessage.GetUsername().data());
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
    go->AddComponent<Transform>();
    auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ColorGreen.json")->material);
    model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);

    if (aMessage.GetIsClient())
    {
        go->AddComponent<Controller>(200.0f, 1.0f);
        myPlayer = go;
    }
    else
    {
        myRemotePlayers.push_back(go);
    }

    Engine::GetInstance().GetSceneHandler().Instantiate(go);
}

void GameClient::HandleMessage_Position(NetMessage_Position& aMessage)
{
    unsigned id = aMessage.GetNetworkID();
    CU::Vector3f pos = aMessage.GetPosition();
    if (auto go = Engine::GetInstance().GetSceneHandler().FindGameObjectByNetworkID(id))
    {
        go->GetComponent<Transform>()->SetTranslation(pos);
    }
}