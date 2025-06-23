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

void GameServer::StartServer()
{
    myComm.Init(true, false, "");
    myShouldReceive = true;
}

void GameServer::Update()
{
    std::chrono::duration<float> elapsed_seconds = std::chrono::system_clock::now() - myLastDataTickTime;
    if (elapsed_seconds.count() > myDataTickRate)
    {
        myLastDataTickTime = std::chrono::system_clock::now();
        myAvgDataReceived = static_cast<int>(std::roundf(myDataReceived / elapsed_seconds.count()));
        myAvgDataSent = static_cast<int>(std::roundf(myDataSent / elapsed_seconds.count()));
        myDataReceived = 0;
        myDataSent = 0;
    }

    if (myShouldReceive)
    {
        Receive();
    }

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

void GameServer::Receive()
{
    for (int i = 0; i < myMessagesHandledPerTick; i++)
    {
        sockaddr_in otherAddress = {};
        NetBuffer receiveBuffer;
        int bytesReceived = myComm.ReceiveData(receiveBuffer, otherAddress);
        if (bytesReceived > 0)
        {
            NetMessage* receivedMessage = ReceiveMessage(receiveBuffer);

            if (receivedMessage)
            {
                receivedMessage->Deserialize(receiveBuffer);
                HandleMessage(receivedMessage, otherAddress, bytesReceived);
                delete receivedMessage;
            }
        }
        else
        {
            break;
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

void GameServer::AcceptHandshake(const NetBuffer& aBuffer, const sockaddr_in& aAddress)
{
    myComm.SendData(aBuffer, aAddress);
    printf("\nAccepted handshake for adress [%i] : [%i]", aAddress.sin_addr.S_un.S_addr, aAddress.sin_port);
}

const NetInfo& GameServer::AddClient(const sockaddr_in& aAddress, const std::string& aUsername)
{
    NetInfo& newClient = myClients.emplace_back();
    newClient.address = aAddress;
    newClient.username = aUsername;

    printf("Added client %s : %i", aUsername.c_str(), aAddress.sin_addr.S_un.S_addr);
    return newClient;
}

void GameServer::RemoveClient(int aClientIndex)
{
    myClients.erase(myClients.begin() + aClientIndex);
}

void GameServer::SendToClient(const NetBuffer& aBuffer, int aClientIndex)
{
    assert(aClientIndex >= 0 && aClientIndex < static_cast<int>(myClients.size()));

    myDataSent += myComm.SendData(aBuffer, myClients[aClientIndex].address);
}

void GameServer::SendToAllClients(const NetBuffer& aBuffer)
{
    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); ++clientIndex)
    {
        SendToClient(aBuffer, clientIndex);
    }
}

void GameServer::SendToAllClientsExcluding(const NetBuffer& aBuffer, const int aClientIndex)
{
    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); ++clientIndex)
    {
        if (clientIndex == aClientIndex) continue;

        SendToClient(aBuffer, clientIndex);
    }
}

bool GameServer::DoesClientExist(const sockaddr_in& aAddress) const
{
    if (myClients.empty()) return false;

    for (int i = 0; i < myClients.size(); i++)
    {
        if (myClients[i] == aAddress)
        {
            return true;
        }
    }

    return false;
}

const int GameServer::GetClientIndex(const sockaddr_in& aAddress) const
{
    for (int i = 0; i < myClients.size(); i++)
    {
        if (myClients[i] == aAddress)
        {
            return i;
        }
    }

    return -1;
}

const NetInfo& GameServer::GetClient(int aClientIndex) const
{
    assert(aClientIndex >= 0 && aClientIndex < static_cast<int>(myClients.size()));

    return myClients[aClientIndex];
}

void GameServer::HandleMessage_RequestConnect(NetMessage_RequestConnect& aMessage, const sockaddr_in& aAddress)
{
    if (DoesClientExist(aAddress)) return;

    const NetInfo& newInfo = AddClient(aAddress, aMessage.GetUsername());
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
        SendToClient(buffer, index);
    }
}

void GameServer::HandleMessage_Disconnect(NetMessage_Disconnect&, const sockaddr_in& aAddress)
{
    if (!DoesClientExist(aAddress)) return;

    int index = GetClientIndex(aAddress);
    const std::string& username = GetClient(index).username;
    printf("\n[User %s disconnected]", username.data());
    
    RemoveClient(index);

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
    AcceptHandshake(buffer, aAddress);
}

void GameServer::CreateNewObject()
{
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
