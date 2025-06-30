#include <Enginepch.h>
#include "GameServer.h"
#include <iostream>

#include "NetworkEngine/NetworkDefines.hpp"
#include "NetworkEngine/BaseNetMessages/NetMessage.h"
#include "NetworkEngine/BaseNetMessages/GuaranteedNetMessage.h"
#include "NetworkEngine/BaseNetMessages/AckNetMessage.h"
#include "NetworkEngine/BaseNetMessages/RTTPingMessage.h"
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
#include <Utilities/CommonUtilities/VectorUtilities.hpp>
#include <GameEngine/DebugDrawer/DebugDrawer.h>

constexpr int objectLimit = 16;
constexpr float objectSpawnRate = 1.0f;
constexpr float playerAwarenessCircleRadius = 500.0f;
constexpr float nearTickRate = 10.0f;
constexpr float mediumTickRate = 5.0f;
constexpr float farTickRate = 1.0f;

void GameServer::StartServer()
{
    myComm.Init(true, false, "");
    myShouldReceive = true;

    InitializeGrid();
}

void GameServer::Update()
{
    for (auto& row : myGrid)
    {
        for (auto& column : row)
        {
            Engine::Get().GetDebugDrawer().DrawBoundingBox(column);
        }
    }

    std::chrono::duration<float> dt = std::chrono::system_clock::now() - myLastTickTimestamp;
    if (dt.count() > (1.0f / NetworkDefines::Server::tickRate))
    {
        myLastTickTimestamp = std::chrono::system_clock::now();

        if (myShouldReceive)
        {
            Receive();
        }

        UpdateNetworkStats();
        PingClients();
        UpdateGuaranteedMessages();
        UpdatePositions();
        UpdateObjectSpawn();
    }
}

void GameServer::Receive()
{
    for (size_t i = 0; i < NetworkDefines::Server::maxMessagesHandledPerTick; i++)
    {
        sockaddr_in otherAddress = {};
        NetBuffer receiveBuffer;

        int bytesReceived = myComm.ReceiveData(receiveBuffer, otherAddress);
        if (bytesReceived <= 0) break;

        myDataReceived += bytesReceived;
        NetMessage* receivedMessage = ReceiveMessage(receiveBuffer);

        if (receivedMessage)
        {
            receivedMessage->Deserialize(receiveBuffer);

            if (auto guaranteedMessage = dynamic_cast<GuaranteedNetMessage*>(receivedMessage))
            {
                if (!HandleGuaranteedMessage(guaranteedMessage, otherAddress))
                {
                    delete receivedMessage;
                    continue;
                }
            }
            else if (auto ackMessage = dynamic_cast<AckNetMessage*>(receivedMessage))
            {
                HandleAckMessage(ackMessage, otherAddress);
                delete receivedMessage;
                continue;
            }

            HandleMessage(receivedMessage, otherAddress);
            delete receivedMessage;
        }
    }
}

NetMessage* GameServer::ReceiveMessage(const NetBuffer& aBuffer) const
{
    NetMessageType receivedMessageType = static_cast<NetMessageType>(aBuffer.GetBuffer()[0]);
#ifdef DEBUG_RECEIVED_MESSAGE_TYPE
    printf("Received NetMessageType: %i\n", static_cast<int>(receivedMessageType));
#endif

    switch (receivedMessageType)
    {
    case NetMessageType::AckMessage:
        return new AckNetMessage();
    case NetMessageType::RTTPing:
        return new RTTPingMessage();
    case NetMessageType::RequestConnect:
        return new NetMessage_RequestConnect();
    case NetMessageType::Disconnect:
        return new NetMessage_Disconnect();
    case NetMessageType::Text:
        return new NetMessage_Text();
    case NetMessageType::RequestHandshake:
        return new NetMessage_RequestHandshake();
    case NetMessageType::Position:
        return new NetMessage_Position();
    default:
        return nullptr;
    }
}

void GameServer::HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress)
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
    case NetMessageType::Position:
        HandleMessage_Position(*static_cast<NetMessage_Position*>(aMessage), aAddress);
        break;
    default:
        break;
    }
}

bool GameServer::HandleGuaranteedMessage(GuaranteedNetMessage* aMessage, const sockaddr_in& aAddress)
{
    int id = aMessage->GetGuaranteedMessageID();
    assert(id != 0 && "Guaranteed Message ID should never be 0, something weird is happening!\n");

    AckNetMessage msg;
    msg.SetGuaranteedMessageID(id);
    NetBuffer sendBuffer;
    msg.Serialize(sendBuffer);
    if (auto clientInfo = GetClient(aAddress))
    {
        SendToClient(sendBuffer, *clientInfo);
    }
    else
    {
        printf("Couldn't find client to send ack to!\n");
    }

    AckMessageData ackData;
    ackData.myMessageID = id;
    ackData.mySenderAddress = aAddress;
    if (Utilities::VectorContains(myAcknowledgedMessageIDs, ackData))
    {
        // Message has already been acknowledged, so we send a new acknowledge but we do not act on the message.
#ifdef DEBUG_ACKNOWLEDGED_MESSAGE
        printf("Already acknowledged message with ID %i\n", id);
#endif
        return false;
    }
    else
    {
        // We add the message ID to our acknowledged IDs so that if we receive this ID again we won't act on it.
#ifdef DEBUG_ACKNOWLEDGED_MESSAGE
        printf("Acknowledged message with ID %i\n", id);
#endif
        myAcknowledgedMessageIDs.emplace_back(ackData);
    }

    return true;
}

void GameServer::HandleAckMessage(AckNetMessage* aMessage, const sockaddr_in& aAddress)
{
    int id = aMessage->GetGuaranteedMessageID();
    if (myGuaranteedMessageIDToData.contains(id))
    {
        if (auto clientInfo = GetClient(aAddress))
        {
            if (id == clientInfo->myLastPingMessageID)
            {
                UpdateClientPing(*clientInfo, id, true);
            }
        }

        ++myNrOfAcknowledges;
#ifdef DEBUG_ACKNOWLEDGED_MESSAGE
        printf("Received Acknowledge message with ID %i\n", id);
#endif
        myGuaranteedMessageIDToData.erase(id);
    }
}

void GameServer::AcceptHandshake(const NetBuffer& aBuffer, const sockaddr_in& aAddress)
{
    myComm.SendData(aBuffer, aAddress);
#ifdef DEBUG_ACCEPT_HANDSHAKE
    printf("Accepted handshake for adress [%i] : [%i]\n", aAddress.sin_addr.S_un.S_addr, aAddress.sin_port);
#endif
}

NetInfo& GameServer::AddClient(const sockaddr_in& aAddress, const std::string& aUsername)
{
    NetInfo& newClient = myClients.emplace_back();
    newClient.myAddress = aAddress;
    newClient.myUsername = aUsername;

#ifdef DEBUG_ADD_CLIENT
    printf("Added client %s : %i\n", aUsername.c_str(), aAddress.sin_addr.S_un.S_addr);
#endif
    return newClient;
}

void GameServer::RemoveClient(const NetInfo& aClientNetInfo)
{
    auto it = std::find_if(myClients.begin(), myClients.end(), [aClientNetInfo](const NetInfo& otherClientInfo) { return aClientNetInfo == otherClientInfo; });
    if (it == myClients.end()) return;

    myClients.erase(it);
#ifdef DEBUG_REMOVE_CLIENT
    printf("Removed client from clientlist!\n");
#endif
}

void GameServer::SendToClient(const NetBuffer& aBuffer, const NetInfo& aClientNetInfo)
{
#ifdef DEBUG_SENT_MESSAGE_TYPE
    printf("Sent NetMessageType: %i\n", aBuffer.GetBuffer()[0]);
#endif
    myDataSent += myComm.SendData(aBuffer, aClientNetInfo.myAddress);
}

void GameServer::SendToAllClients(const NetBuffer& aBuffer)
{
    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); ++clientIndex)
    {
        SendToClient(aBuffer, myClients[clientIndex]);
    }
}

void GameServer::SendToAllClientsExcluding(const NetBuffer& aBuffer, const NetInfo& aClientNetInfo)
{
    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); ++clientIndex)
    {
        if (myClients[clientIndex] == aClientNetInfo) continue;

        SendToClient(aBuffer, aClientNetInfo);
    }
}

bool GameServer::DoesClientExist(const sockaddr_in& aAddress) const
{
    if (myClients.empty()) return false;

    auto it = std::find_if(myClients.begin(), myClients.end(), [aAddress](const NetInfo& otherClientInfo) { return aAddress == otherClientInfo; });
    if (it != myClients.end())
    {
        return true;
    }

    return false;
}

const NetInfo* GameServer::GetClient(const sockaddr_in& aAddress) const
{
    auto it = std::find_if(myClients.begin(), myClients.end(), [aAddress](const NetInfo& otherClientInfo) { return aAddress == otherClientInfo; });
    if (it != myClients.end())
    {
        return &(*it);
    }

    return nullptr;
}

NetInfo* GameServer::GetClient(const sockaddr_in& aAddress)
{
    auto it = std::find_if(myClients.begin(), myClients.end(), [aAddress](const NetInfo& otherClientInfo) { return aAddress == otherClientInfo; });
    if (it != myClients.end())
    {
        return &(*it);
    }
    
    return nullptr;
}

const NetInfo* GameServer::GetClientByNetworkID(unsigned aNetworkID) const
{
    auto it = std::find_if(myClients.begin(), myClients.end(), [aNetworkID](const NetInfo& otherClientInfo) { return aNetworkID == otherClientInfo.myCharacterNetworkID; });
    if (it != myClients.end())
    {
        return &(*it);
    }

    return nullptr;
}

NetInfo* GameServer::GetClientByNetworkID(unsigned aNetworkID)
{
    auto it = std::find_if(myClients.begin(), myClients.end(), [aNetworkID](const NetInfo& otherClientInfo) { return aNetworkID == otherClientInfo.myCharacterNetworkID; });
    if (it != myClients.end())
    {
        return &(*it);
    }

    return nullptr;
}

void GameServer::UpdateClientPing(NetInfo& aClientNetInfo, int aMessageID, bool aShouldUpdateRTT)
{
    std::chrono::duration<float> rtt = std::chrono::system_clock::now() - aClientNetInfo.myLastPingTime;
    aClientNetInfo.myLastPingTime = std::chrono::system_clock::now();
    aClientNetInfo.myLastPingMessageID = aMessageID;

    if (aShouldUpdateRTT)
    {
        aClientNetInfo.myRTT = rtt.count();
    }
}

void GameServer::HandleMessage_RequestConnect(NetMessage_RequestConnect& aMessage, const sockaddr_in& aAddress)
{
    if (DoesClientExist(aAddress)) return;

    NetInfo& newInfo = AddClient(aAddress, aMessage.GetUsername());

    // Send connect accept.
    {
        NetMessage_AcceptConnect acceptConnectMsg;
        NetBuffer buffer;
        acceptConnectMsg.Serialize(buffer);
        SendToClient(buffer, newInfo);
    }

    // Create already existing objects for newly joined user.
    for (auto& object : myObjects)
    {
        NetMessage_CreateCharacter createCharacterMsg;
        int guaranteedMessageID = CreateNewGuaranteedMessage(&createCharacterMsg, newInfo);
        createCharacterMsg.SetIsPlayerCharacter(false);
        createCharacterMsg.SetIsControlledByClient(false);
        createCharacterMsg.SetNetworkID(object->GetNetworkID());
        createCharacterMsg.SetStartingPosition(object->GetComponent<Transform>()->GetTranslation(true));
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);
        SendToClient(buffer, newInfo);

        myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
    }

    // Create already existing characters for newly joined user.
    for (auto& player : myPlayers)
    {
        NetMessage_CreateCharacter createCharacterMsg;
        int guaranteedMessageID = CreateNewGuaranteedMessage(&createCharacterMsg, newInfo);
        createCharacterMsg.SetIsPlayerCharacter(true);
        createCharacterMsg.SetIsControlledByClient(false);
        createCharacterMsg.SetNetworkID(player->GetNetworkID());
        createCharacterMsg.SetStartingPosition(player->GetComponent<Transform>()->GetTranslation(true));
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);
        SendToClient(buffer, newInfo);

        myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
    }

    CreateNewPlayer(newInfo);
}

void GameServer::HandleMessage_Disconnect(NetMessage_Disconnect&, const sockaddr_in& aAddress)
{
    NetInfo* clientInfoPtr = GetClient(aAddress);
    if (!clientInfoPtr) return;

#ifdef DEBUG_CLIENT_DISCONNECTED
    printf("\n[User %s disconnected]\n", clientInfoPtr->myUsername.data());
#endif
    
    RemoveClient(*clientInfoPtr);

    NetMessage_Disconnect disconnectMsg;
    disconnectMsg.SetData(clientInfoPtr->myUsername.data());
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

void GameServer::HandleMessage_Position(NetMessage_Position& aMessage, const sockaddr_in& aAddress)
{
    if (auto clientInfo = GetClient(aAddress); clientInfo && aMessage.GetNetworkID() != clientInfo->myCharacterNetworkID)
    {
        printf("A client is trying to set the position of an object with a network ID that doesn't match their characters network ID. Either the game is broken or the player is cheating! :O");
        return;
    }

    auto it = std::find_if(myPlayers.begin(), myPlayers.end(), [aMessage](const std::shared_ptr<GameObject> playerObject) { return aMessage.GetNetworkID() == playerObject->GetNetworkID(); });
    if (it != myPlayers.end())
    {
        (*it)->GetComponent<Transform>()->SetTranslation(aMessage.GetPosition());
    }
}

void GameServer::CreateNewObject()
{
    Math::Vector3f startingPos;
    startingPos.x = static_cast<float>((std::rand() % 1000) - 500);
    startingPos.y = 0.0f;
    startingPos.z = static_cast<float>((std::rand() % 1000) - 500);
    
    int networkID = myCurrentNetworkID;
    myLastSpawnTimestamp = std::chrono::system_clock::now();
    ++myCurrentNetworkID;

    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); clientIndex++)
    {
        NetMessage_CreateCharacter createCharacterMsg;
        int guaranteedMessageID = CreateNewGuaranteedMessage(&createCharacterMsg, myClients[clientIndex]);
        createCharacterMsg.SetNetworkID(networkID);
        createCharacterMsg.SetIsPlayerCharacter(false);
        createCharacterMsg.SetIsControlledByClient(false);
        createCharacterMsg.SetStartingPosition(startingPos);
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);
        SendToClient(buffer, myClients[clientIndex]);

        myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
    }

    {
        std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
        go->SetNetworkID(networkID);
        go->AddComponent<Transform>(startingPos);
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
    }
}

void GameServer::DestroyObject(unsigned aNetworkID)
{
    auto it = std::find_if(myObjects.begin(), myObjects.end(), [aNetworkID](const std::shared_ptr<GameObject>& object) { return object->GetNetworkID() == aNetworkID; });
    if (it != myObjects.end() && *it != nullptr)
    {
        myObjects.erase(it);
    }

    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); clientIndex++)
    {
        NetMessage_RemoveCharacter removeCharacterMsg;
        int guaranteedMessageID = CreateNewGuaranteedMessage(&removeCharacterMsg, myClients[clientIndex]);
        removeCharacterMsg.SetNetworkID(aNetworkID);
        NetBuffer buffer;
        removeCharacterMsg.Serialize(buffer);
        SendToClient(buffer, myClients[clientIndex]);

        myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
    }
}

void GameServer::CreateNewPlayer(NetInfo& aClientNetInfo)
{
    Math::Vector3f startingPos;
    startingPos.x = static_cast<float>((std::rand() % 1000) - 500);
    startingPos.y = 0.0f;
    startingPos.z = static_cast<float>((std::rand() % 1000) - 500);

    int networkID = myCurrentNetworkID;
    ++myCurrentNetworkID;
    ++myCurrentlyActiveObjects;

    // Create the new player character on the controlling client.
    {
        NetMessage_CreateCharacter createCharacterMsg;
        int guaranteedMessageID = CreateNewGuaranteedMessage(&createCharacterMsg, aClientNetInfo);
        createCharacterMsg.SetNetworkID(networkID);
        createCharacterMsg.SetIsPlayerCharacter(true);
        createCharacterMsg.SetIsControlledByClient(true);
        createCharacterMsg.SetStartingPosition(startingPos);
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);

        SendToClient(buffer, aClientNetInfo);

        myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
    }

    // Create the new player character for all other clients.
    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); clientIndex++)
    {
        if (myClients[clientIndex] == aClientNetInfo) continue;

        NetMessage_CreateCharacter createCharacterMsg;
        int guaranteedMessageID = CreateNewGuaranteedMessage(&createCharacterMsg, myClients[clientIndex]);
        createCharacterMsg.SetNetworkID(networkID);
        createCharacterMsg.SetIsPlayerCharacter(true);
        createCharacterMsg.SetIsControlledByClient(false);
        createCharacterMsg.SetStartingPosition(startingPos);
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);
        SendToClient(buffer, myClients[clientIndex]);

        myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
    }

    {
        std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
        go->SetNetworkID(networkID);
        go->AddComponent<Transform>(startingPos);

        auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ColorGreen.json")->material);
        model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);

        aClientNetInfo.myCharacterNetworkID = networkID;
        myPlayers.push_back(go);

        Engine::Get().GetSceneHandler().Instantiate(go);
    }
}

void GameServer::UpdateNetworkStats()
{
    std::chrono::duration<float> elapsed_seconds = std::chrono::system_clock::now() - myLastDataTickTime;
    if (elapsed_seconds.count() > (1.0f / NetworkDefines::networkStatsUpdateRate))
    {
        myLastDataTickTime = std::chrono::system_clock::now();
        myAvgDataReceived = static_cast<int>(std::roundf(myDataReceived / elapsed_seconds.count()));
        myAvgDataSent = static_cast<int>(std::roundf(myDataSent / elapsed_seconds.count()));
        myDataReceived = 0;
        myDataSent = 0;
    }
}

void GameServer::PingClients()
{
    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); clientIndex++)
    {
        std::chrono::duration<float> timeSinceLastPing = std::chrono::system_clock::now() - myClients[clientIndex].myLastPingTime;
        if (timeSinceLastPing.count() > (1.0f / NetworkDefines::Server::pingClientsRate))
        {
            RTTPingMessage pingMsg;
            int guaranteedMessageID = CreateNewGuaranteedMessage(&pingMsg, myClients[clientIndex]);
            NetBuffer buffer;
            pingMsg.Serialize(buffer);
            SendToClient(buffer, myClients[clientIndex]);

            myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
            UpdateClientPing(myClients[clientIndex], guaranteedMessageID);
        }
    }
}

void GameServer::UpdateGuaranteedMessages()
{
    std::array<int, 100> messageIDsToRemove = {};
    int currentIndex = 0;

    for (auto& [guaranteedMessageID, guaranteedMessageData] : myGuaranteedMessageIDToData)
    {
        auto clientInfoPtr = GetClient(guaranteedMessageData.myRecipientAddress);
        if (!clientInfoPtr)
        {
            messageIDsToRemove[++currentIndex] = guaranteedMessageID;
            continue;
        }

        auto& clientInfo = *clientInfoPtr;

        std::chrono::duration<float> elapsedTime = std::chrono::system_clock::now() - guaranteedMessageData.myLastSentTimestamp;
        if (elapsedTime.count() > NetworkDefines::guaranteedMessageTimeout)
        {
            if (guaranteedMessageData.myAttempts > NetworkDefines::guaranteedMessageMaxTimeouts)
            {
                RemoveClient(clientInfo);
                messageIDsToRemove[++currentIndex] = guaranteedMessageID;
            }
            else
            {
#ifdef DEBUG_SEND_GUARANTEED_MESSAGE
                printf("Resending guaranteed message with ID %i attempt %i\n", guaranteedMessageID, guaranteedMessageData.myAttempts);
#endif

                if (guaranteedMessageID == clientInfo.myLastPingMessageID)
                {
                    UpdateClientPing(clientInfo, clientInfo.myLastPingMessageID);
                }

                ++myNrOfGuaranteedMessagesSent;
                SendToClient(guaranteedMessageData.myGuaranteedMessageBuffer, clientInfo);
                guaranteedMessageData.myLastSentTimestamp = std::chrono::system_clock::now();
                ++guaranteedMessageData.myAttempts;
            }
        }
    }

    for (auto& messageIDToRemove : messageIDsToRemove)
    {
        if (myGuaranteedMessageIDToData.contains(messageIDToRemove))
        {
            myGuaranteedMessageIDToData.erase(messageIDToRemove);
        }
    }
}

void GameServer::UpdateObjectSpawn()
{
    std::chrono::duration<float> timeSinceLastSpawn = std::chrono::system_clock::now() - myLastSpawnTimestamp;
    if (timeSinceLastSpawn.count() > objectSpawnRate)
    {
        if (myCurrentlyActiveObjects < objectLimit)
        {
            CreateNewObject();
            myCurrentlyActiveObjects++;
        }
    }
}

void GameServer::UpdatePositions()
{
    for (auto& clientNetInfo : myClients)
    {
        int clientNetworkID = clientNetInfo.myCharacterNetworkID;
        Math::Vector3f clientPos;
        if (auto go = Engine::Get().GetSceneHandler().FindGameObjectByNetworkID(clientNetworkID))
        {
            clientPos = go->GetComponent<Transform>()->GetTranslation(true);
        }
        Math::Vector2<int> clientCell = GetGridCell(clientPos);

        std::function<void(int)> posFunc([this, clientCell, clientNetInfo](int aCellDistance)
        {
                for (auto& object : myObjects)
                {
                    Math::Vector3f objectPos = object->GetComponent<Transform>()->GetTranslation(true);
                    //if ((clientPos - objectPos).LengthSqr() > playerAwarenessCircleRadius * playerAwarenessCircleRadius) continue;

                    Math::Vector2<int> objectCell = GetGridCell(objectPos);
                    Math::Vector2<int> cellDiff = clientCell - objectCell;
                    int cellDistance = abs(cellDiff.x) + abs(cellDiff.y);

                    if (cellDistance == aCellDistance)
                    {
                        NetMessage_Position newMsg;
                        newMsg.SetNetworkID(object->GetNetworkID());
                        newMsg.SetPosition(objectPos);
                        newMsg.SetTimestamp(std::chrono::system_clock::now());
                        NetBuffer buffer;
                        newMsg.Serialize(buffer);
                        SendToClient(buffer, clientNetInfo);
                    }
                }

                for (auto& playerObject : myPlayers)
                {
                    if (auto playerTransform = playerObject->GetComponent<Transform>())
                    {
                        auto playerObjectInfoPtr = GetClientByNetworkID(playerObject->GetNetworkID());
                        if (!playerObjectInfoPtr || *playerObjectInfoPtr == clientNetInfo) continue;

                        Math::Vector3f playerPos = playerTransform->GetTranslation(true);
                        //if ((clientPos - playerPos).LengthSqr() > playerAwarenessCircleRadius * playerAwarenessCircleRadius) continue;

                        Math::Vector2<int> playerCell = GetGridCell(playerPos);
                        Math::Vector2<int> cellDiff = clientCell - playerCell;
                        int cellDistance = abs(cellDiff.x) + abs(cellDiff.y);

                        if (cellDistance == aCellDistance)
                        {
                            NetMessage_Position newMsg;
                            newMsg.SetNetworkID(playerObject->GetNetworkID());
                            newMsg.SetPosition(playerPos);
                            newMsg.SetTimestamp(std::chrono::system_clock::now());
                            NetBuffer buffer;
                            newMsg.Serialize(buffer);
                            SendToClient(buffer, clientNetInfo);
                        }
                    }
                }
        });

        std::chrono::duration<float> timeSinceLastNearUpdate = std::chrono::system_clock::now() - clientNetInfo.myLastNearUpdateTimestamp;
        if (timeSinceLastNearUpdate.count() > (1.0f / nearTickRate))
        {
            clientNetInfo.myLastNearUpdateTimestamp = std::chrono::system_clock::now();
            posFunc(0);
        }

        std::chrono::duration<float> timeSinceLastMediumUpdate = std::chrono::system_clock::now() - clientNetInfo.myLastMediumUpdateTimestamp;
        if (timeSinceLastMediumUpdate.count() > (1.0f / mediumTickRate))
        {
            clientNetInfo.myLastMediumUpdateTimestamp = std::chrono::system_clock::now();
            posFunc(1);
        }

        std::chrono::duration<float> timeSinceLastFarUpdate = std::chrono::system_clock::now() - clientNetInfo.myLastFarUpdateTimestamp;
        if (timeSinceLastFarUpdate.count() > (1.0f / farTickRate))
        {
            clientNetInfo.myLastFarUpdateTimestamp = std::chrono::system_clock::now();
            posFunc(2);
        }
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

void GameServer::InitializeGrid()
{
    myGrid[0][0] = Math::AABB3D<float>({ -800.0f, -800.0f, 0.0f }, { 0.0f, 800.0f, 800.0f });
    myGrid[0][1] = Math::AABB3D<float>({ 0.0f, -800.0f, 0.0f }, { 800.0f, 800.0f, 800.0f });
    myGrid[1][0] = Math::AABB3D<float>({ -800.0f, -800.0f, -800.0f }, { 0.0f, 800.0f, 0.0f });
    myGrid[1][1] = Math::AABB3D<float>({ 0.0f, -800.0f, -800.0f }, { 800.0f, 800.0f, 0.0f });
}

Math::Vector2<int> GameServer::GetGridCell(const Math::Vector3f& aPosition) const
{
    for (int rowIndex = 0; rowIndex < static_cast<int>(myGrid.size()); rowIndex++)
    {
        for (int columnIndex = 0; columnIndex < static_cast<int>(myGrid[rowIndex].size()); columnIndex++)
        {
            if (myGrid[rowIndex][columnIndex].IsInside(aPosition))
            {
                return Math::Vector2<int>(rowIndex, columnIndex);
            }
        }
    }

    return Math::Vector2<int>();
}

int GameServer::CreateNewGuaranteedMessage(GuaranteedNetMessage* aGuaranteedMessage, const NetInfo& aClientNetInfo)
{
    int id = myCurrentGuaranteedMessageID;
    ++myCurrentGuaranteedMessageID;
    aGuaranteedMessage->SetGuaranteedMessageID(id);

    myGuaranteedMessageIDToData[id].myLastSentTimestamp = std::chrono::system_clock::now();
    myGuaranteedMessageIDToData[id].myAttempts = 1;
    myGuaranteedMessageIDToData[id].myRecipientAddress = aClientNetInfo.myAddress;

#ifdef DEBUG_CREATE_GUARANTEED_MESSAGE
    printf("Created guaranteed message with ID %i - NetMessageType: %i\n", id, static_cast<int>(aGuaranteedMessage->GetType()));
#endif
    ++myNrOfGuaranteedMessagesSent;
    return id;
}