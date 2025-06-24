#include <Enginepch.h>
#include "GameServer.h"
#include <iostream>

#include "NetworkEngine/NetMessage.h"
#include "NetworkEngine/GuaranteedNetMessage.h"
#include "NetworkEngine/AckNetMessage.h"
#include "NetworkEngine/RTTPingMessage.h"
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

    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); clientIndex++)
    {
        std::chrono::duration<float> timeSinceLastPing = std::chrono::system_clock::now() - myClients[clientIndex].myLastPingTime;
        if (timeSinceLastPing.count() > myTimeBetweenPings)
        {

            RTTPingMessage pingMsg;
            int guaranteedMessageID = CreateNewGuaranteedMessage(&pingMsg, clientIndex);
            NetBuffer buffer;
            pingMsg.Serialize(buffer);
            SendToClient(buffer, clientIndex);

            myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
            UpdateClientPing(clientIndex, guaranteedMessageID);
        }
    }

    std::array<int, 100> messageIDsToRemove = {};
    int currentIndex = 0;

    for (auto& [guaranteedMessageID, guaranteedMessageData] : myGuaranteedMessageIDToData)
    {
        int clientIndex = GetClientIndex(guaranteedMessageData.myRecipientAddress);
        if (clientIndex == -1)
        {
            messageIDsToRemove[++currentIndex] = guaranteedMessageID;
            continue;
        }

        std::chrono::duration<float> elapsedTime = std::chrono::system_clock::now() - guaranteedMessageData.myLastSentTimestamp;
        if (elapsedTime.count() > myGuaranteedMessageTimeout)
        {
            if (guaranteedMessageData.myAttempts >= myGuaranteedMesssageMaxTimeouts)
            {
                RemoveClient(clientIndex);
                messageIDsToRemove[++currentIndex] = guaranteedMessageID;
            }
            else
            {
                if (guaranteedMessageID == GetClient(clientIndex).myLastPingMessageID)
                {
                    UpdateClientPing(clientIndex, GetClient(clientIndex).myLastPingMessageID);
                }
                
                printf("Sending guaranteed message with ID %i attempt %i\n", guaranteedMessageID, guaranteedMessageData.myAttempts);
                ++myNrOfGuaranteedMessagesSent;
                SendToClient(guaranteedMessageData.myGuaranteedMessageBuffer, clientIndex);
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
            myDataReceived += bytesReceived;
            NetMessage* receivedMessage = ReceiveMessage(receiveBuffer);

            if (receivedMessage)
            {
                receivedMessage->Deserialize(receiveBuffer);

                if (auto guaranteedMessage = dynamic_cast<GuaranteedNetMessage*>(receivedMessage))
                {
                    int id = guaranteedMessage->GetGuaranteedMessageID();

                    AckNetMessage msg;
                    msg.SetGuaranteedMessageID(id);
                    NetBuffer sendBuffer;
                    msg.Serialize(sendBuffer);
                    if (DoesClientExist(otherAddress))
                    {
                        SendToClient(sendBuffer, GetClientIndex(otherAddress));
                    }

                    if (Utilities::VectorContains(myAcknowledgedMessageIDs, id))
                    {
                        // Message has already been acknowledged, so we send a new acknowledge but we do not act on the message.
                        printf("Already acknowledged message with ID %i\n", id);
                        delete receivedMessage;
                        continue;
                    }
                    else
                    {
                        // We add the message ID to our acknowledged IDs so that if we receive this ID again we won't act on it.
                        printf("Acknowledged message with ID %i\n", id);
                        myAcknowledgedMessageIDs.emplace_back(id);
                    }
                }
                else if (auto ackMessage = dynamic_cast<AckNetMessage*>(receivedMessage))
                {
                    int id = ackMessage->GetGuaranteedMessageID();
                    if (myGuaranteedMessageIDToData.contains(id))
                    {
                        if (id == GetClient(GetClientIndex(otherAddress)).myLastPingMessageID)
                        {
                            UpdateClientPing(GetClientIndex(otherAddress), id, true);
                        }

                        ++myNrOfAcknowledges;
                        printf("Received Acknowledge message with ID %i\n", id);
                        myGuaranteedMessageIDToData.erase(id);
                    }

                    delete receivedMessage;
                    continue;
                }

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
    printf("Accepted handshake for adress [%i] : [%i]\n", aAddress.sin_addr.S_un.S_addr, aAddress.sin_port);
}

const NetInfo& GameServer::AddClient(const sockaddr_in& aAddress, const std::string& aUsername)
{
    NetInfo& newClient = myClients.emplace_back();
    newClient.address = aAddress;
    newClient.username = aUsername;

    printf("Added client %s : %i\n", aUsername.c_str(), aAddress.sin_addr.S_un.S_addr);
    return newClient;
}

void GameServer::RemoveClient(int aClientIndex)
{
    myClients.erase(myClients.begin() + aClientIndex);
    printf("Disconnected client!\n");
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

void GameServer::UpdateClientPing(int aClientIndex, int aMessageID, bool aShouldUpdateRTT)
{
    std::chrono::duration<float> rtt = std::chrono::system_clock::now() - myClients[aClientIndex].myLastPingTime;
    myClients[aClientIndex].myLastPingTime = std::chrono::system_clock::now();
    myClients[aClientIndex].myLastPingMessageID = aMessageID;

    if (aShouldUpdateRTT)
    {
        myClients[aClientIndex].myRTT = rtt.count();
    }
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
        int guaranteedMessageID = CreateNewGuaranteedMessage(&createCharacterMsg, index);
        createCharacterMsg.SetNetworkID(object->GetNetworkID());
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);
        SendToClient(buffer, index);

        myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
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
    
    int networkID = myCurrentNetworkID;

    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); clientIndex++)
    {
        NetMessage_CreateCharacter createCharacterMsg;
        int guaranteedMessageID = CreateNewGuaranteedMessage(&createCharacterMsg, clientIndex);
        createCharacterMsg.SetNetworkID(networkID);
        createCharacterMsg.SetStartingPosition(startingPos);
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);
        SendToClient(buffer, clientIndex);

        myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
    }

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

    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); clientIndex++)
    {
        NetMessage_RemoveCharacter removeCharacterMsg;
        int guaranteedMessageID = CreateNewGuaranteedMessage(&removeCharacterMsg, clientIndex);
        removeCharacterMsg.SetNetworkID(aNetworkID);
        NetBuffer buffer;
        removeCharacterMsg.Serialize(buffer);
        SendToClient(buffer, clientIndex);

        myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
    }
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

int GameServer::CreateNewGuaranteedMessage(GuaranteedNetMessage* aGuaranteedMessage, int aClientIndex)
{
    int id = myGuaranteedMessageID;
    ++myGuaranteedMessageID;
    aGuaranteedMessage->SetGuaranteedMessageID(id);

    myGuaranteedMessageIDToData[id].myLastSentTimestamp = std::chrono::system_clock::now();
    myGuaranteedMessageIDToData[id].myAttempts = 1;
    myGuaranteedMessageIDToData[id].myRecipientAddress = GetClient(aClientIndex).address;

    printf("Created guaranteed message with ID %i\n", id);
    ++myNrOfGuaranteedMessagesSent;
    return id;
}