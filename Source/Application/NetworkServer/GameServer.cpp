#include <Enginepch.h>
#include "GameServer.h"
#include <iostream>

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
#include "RandomDirectionMovement.h"
#include "BounceAgainstWorldEdges.h"

void GameServer::Update()
{
    ServerBase::Update();

    double currentTime = Engine::Get().GetTimer().GetTimeSinceProgramStart();
    float dt = static_cast<float>(currentTime - myLastUpdateTimestamp);
    if (dt > (1.0f / myTickRate))
    {
        myLastUpdateTimestamp = currentTime;

        UpdatePositions();

        myCurrentTimeSinceLastSpawn += dt;
        if (myCurrentTimeSinceLastSpawn > myTimeBetweenObjectsSpawned)
        {
            if (myCurrentlyActiveObjects < myObjectLimit)
            {
                CreateNewObject();
                myCurrentlyActiveObjects++;
            }
        }
    }
}

NetMessage* GameServer::ReceiveMessage(const NetBuffer& aBuffer) const
{
    NetMessageType receivedMessageType = static_cast<NetMessageType>(aBuffer.GetBuffer()[0]);

    switch (receivedMessageType)
    {
    case NetMessageType::RequestConnect:
    {
        return new NetMessage_RequestConnect();
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
    case NetMessageType::RequestHandshake:
    {
        return new NetMessage_RequestHandshake();
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

void GameServer::HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress, const int)
{
    NetMessageType type = aMessage->GetType();
    switch (type)
    {
    case NetMessageType::RequestConnect:
        HandleMessage_RequestConnect(*static_cast<NetMessage_RequestConnect*>(aMessage), aAddress);
        break;
    case NetMessageType::Disconnect:
        HandleMessage_Disconnect(*static_cast<NetMessage_Disconnect*>(aMessage), aAddress);
        break;
    case NetMessageType::RequestHandshake:
        HandleMessage_HandshakeRequest(aAddress);
        break;
    default:
        break;
    }
}

void GameServer::HandleMessage_RequestConnect(NetMessage_RequestConnect& aMessage, const sockaddr_in& aAddress)
{
    if (DoesClientExist(aAddress)) return;

    NetInfo& newInfo = myClients.emplace_back();
    newInfo.address = aAddress;
    newInfo.username = aMessage.GetUsername();
    int index = GetClientIndex(newInfo.address);

    // Send connect accept.
    {
        NetMessage_AcceptConnect acceptConnectMsg;
        NetBuffer buffer;
        acceptConnectMsg.Serialize(buffer);
        SendToClient(buffer, index);
    }

    // Create already existing objects for newly joined user.
    for (auto& object : myObjects)
    {
        NetMessage_CreateCharacter createCharacterMsg;
        createCharacterMsg.SetNetworkID(object->GetNetworkID());
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);
        myComm.SendData(buffer, myClients[index].address);
    }
}

void GameServer::HandleMessage_Disconnect(NetMessage_Disconnect&, const sockaddr_in& aAddress)
{
    if (!DoesClientExist(aAddress)) return;

    int index = GetClientIndex(aAddress);
    const std::string& username = myClients[index].username;
    printf("\n[User %s disconnected]", username.data());
    
    myClients.erase(myClients.begin() + index);

    NetMessage_Disconnect disconnectMsg;
    disconnectMsg.SetData(username);
    NetBuffer buffer;
    disconnectMsg.Serialize(buffer);
    SendToAllClients(buffer);
}

void GameServer::HandleMessage_HandshakeRequest(const sockaddr_in& aAddress)
{
    if (DoesClientExist(aAddress)) return;

    NetMessage_AcceptHandshake msg;
    NetBuffer buffer;
    msg.Serialize(buffer);
    myComm.SendData(buffer, aAddress);
    printf("\nAccepted handshake for adress [%i] : [%i]", aAddress.sin_addr.S_un.S_addr, aAddress.sin_port);
}

void GameServer::CreateNewObject()
{
    //printf("\nStarted creating object!\n");

    Math::Vector3f startingPos;
    startingPos.x = static_cast<float>((std::rand() % 1000) - 500);
    startingPos.y = 0.0f;
    startingPos.z = static_cast<float>((std::rand() % 1000) - 500);

    NetMessage_CreateCharacter createCharacterMsg;
    createCharacterMsg.SetNetworkID(myCurrentNetworkID);
    createCharacterMsg.SetStartingPosition(startingPos);
    NetBuffer buffer;
    createCharacterMsg.Serialize(buffer);
    SendToAllClients(buffer);

    std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
    go->SetNetworkID(createCharacterMsg.GetNetworkID());
    go->AddComponent<Transform>(createCharacterMsg.GetPosition());
    go->AddComponent<RandomDirectionMovement>();
    go->AddComponent<BounceAgainstWorldEdges>();
    auto coll = go->AddComponent<BoxCollider>(Math::Vector3f(50.0f, 100.0f, 50.0f), Math::Vector3f(0.0f, 90.0f, 0.0f));
    unsigned id = go->GetNetworkID();
    coll->SetCollisionResponse([this, id] {
        if (auto gameObject = Engine::Get().GetSceneHandler().FindGameObjectByNetworkID(id))
        {
            Engine::Get().GetSceneHandler().Destroy(gameObject);
            DestroyObject(id);
            myCurrentlyActiveObjects--;
        }
        });


    auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ColorBlue.json")->material);
    model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);

    myObjects.push_back(go);

    Engine::Get().GetSceneHandler().Instantiate(go);

    myCurrentTimeSinceLastSpawn = 0.0f;
    myCurrentNetworkID++;
}

void GameServer::DestroyObject(unsigned aNetworkID)
{
    auto it = std::find_if(myObjects.begin(), myObjects.end(), [aNetworkID](const std::shared_ptr<GameObject>& object) { return object->GetNetworkID() == aNetworkID; });
    if (it != myObjects.end() && *it != nullptr)
    {
        myObjects.erase(it);
    }

    NetMessage_RemoveCharacter removeCharacterMsg;
    removeCharacterMsg.SetNetworkID(aNetworkID);
    NetBuffer buffer;
    removeCharacterMsg.Serialize(buffer);
    SendToAllClients(buffer);
}

void GameServer::UpdatePositions()
{
    for (auto& object : myObjects)
    {
        NetMessage_Position newMsg;
        newMsg.SetNetworkID(object->GetNetworkID());
        newMsg.SetPosition(object->GetComponent<Transform>()->GetTranslation());
        newMsg.SetTimestamp(Engine::Get().GetTimer().GetTimeSinceEpoch());
        NetBuffer buffer;
        newMsg.Serialize(buffer);
        SendToAllClients(buffer);
    }
}

void GameServer::SendTestMessage()
{
    static int increment = 0;
    printf("%i\n", increment);

    NetMessage_Test newMsg;
    newMsg.SetNetworkID(0);
    newMsg.SetInt(increment);
    NetBuffer buffer;
    newMsg.Serialize(buffer);
    SendToAllClients(buffer);
    ++increment;
}
