#include <Enginepch.h>
#include "GameClient.h"
#include <iostream>
#include <WinSock2.h>

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
#include <GameEngine/Input/InputHandler.h>
#include <Utilities/CommonUtilities/VectorUtilities.hpp>

#include "Controller.h"

void GameClient::ConnectClient(const char* aIP)
{
    myComm.Init(false, false, aIP);
    SendHandshakeRequest();
    myLastHandshakeRequestTime = std::chrono::system_clock::now();
    myShouldReceive = true;
    myAttemptToConnect = true;
    printf("\nWaiting for server...\n");
}

void GameClient::SetUsername(const char* aUsername)
{
    myUsername = aUsername;
}

void GameClient::Update()
{
    std::chrono::duration<float> timeSinceLastTick = std::chrono::system_clock::now() - myLastTickTimestamp;
    if (timeSinceLastTick.count() > (1.0f / NetworkDefines::Client::tickRate))
    {
        myLastTickTimestamp = std::chrono::system_clock::now();

        if (myAttemptToConnect)
        {
            TryConnect();
        }

        if (myShouldReceive)
        {
            Receive();
        }

        if (myHasEstablishedHandshake && myHasEstablishedConnection)
        {
            UpdateNetworkStats();
            PingServer();
            UpdateGuaranteedMessages();
            UpdateObjectPositions();
            SendPlayerCharacterPosition();
        }
    }
}

void GameClient::Receive()
{
    for (size_t i = 0; i < NetworkDefines::Client::maxMessagesHandledPerTick; i++)
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
                if (!HandleGuaranteedMessage(guaranteedMessage))
                {
                    delete receivedMessage;
                    continue;
                }
            }
            else if (auto ackMessage = dynamic_cast<AckNetMessage*>(receivedMessage))
            {
                HandleAckMessage(ackMessage);
                delete receivedMessage;
                continue;
            }

            HandleMessage(receivedMessage);
            delete receivedMessage;
        }
    }
}

void GameClient::Send(const NetBuffer& aBuffer)
{
#ifdef DEBUG_SENT_MESSAGE_TYPE
    printf("Sent NetMessageType: %i\n", aBuffer.GetBuffer()[0]);
#endif
    myDataSent += myComm.SendData(aBuffer);
}

NetMessage* GameClient::ReceiveMessage(const NetBuffer& aBuffer) const
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

bool GameClient::HandleGuaranteedMessage(GuaranteedNetMessage* aMessage)
{
    int id = aMessage->GetGuaranteedMessageID();
    assert(id != 0 && "Guaranteed Message ID should never be 0, something weird is happening!\n");

    AckNetMessage msg;
    msg.SetGuaranteedMessageID(id);
    NetBuffer sendBuffer;
    msg.Serialize(sendBuffer);
    Send(sendBuffer);

    if (Utilities::VectorContains(myAcknowledgedMessageIDs, id))
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
        myAcknowledgedMessageIDs.emplace_back(id);
    }

    return true;
}

void GameClient::HandleAckMessage(AckNetMessage* aMessage)
{
    int id = aMessage->GetGuaranteedMessageID();
    if (myGuaranteedMessageIDToData.contains(id))
    {
        std::chrono::duration<float> rtt = std::chrono::system_clock::now() - myLastPingTime;
        myRTT = rtt.count();
        myLastPingTime = std::chrono::system_clock::now();

        ++myNrOfAcknowledges;
#ifdef DEBUG_ACKNOWLEDGED_MESSAGE
        printf("Received Acknowledge message with ID %i\n", id);
#endif
        myGuaranteedMessageIDToData.erase(id);
    }
}

void GameClient::SendHandshakeRequest()
{
    NetMessage_RequestHandshake msg;
    NetBuffer sendBuffer;
    msg.Serialize(sendBuffer);
    Send(sendBuffer);
}

void GameClient::SendConnectionRequest(const std::string& aUsername)
{
#ifdef DEBUG_ATTEMPTING_CONNECT
    printf("Attempting to connect to server with username %s\n", aUsername.c_str());
#endif

    NetMessage_RequestConnect msg;
    msg.SetUsername(aUsername);
    NetBuffer sendBuffer;
    msg.Serialize(sendBuffer);
    Send(sendBuffer);
}

void GameClient::SendTextMessage(const std::string& aMessage)
{
    NetMessage_Text textMsg;
    textMsg.SetData(aMessage);
    NetBuffer sendBuffer;
    textMsg.Serialize(sendBuffer);
    Send(sendBuffer);
}

void GameClient::SendDisconnectMessage()
{
#ifdef DEBUG_ATTEMPTING_DISCONNECT
    printf("Disconnected\n");
#endif
    NetMessage_Disconnect disconnectMsg;
    NetBuffer sendBuffer;
    disconnectMsg.Serialize(sendBuffer);
    Send(sendBuffer);

    Disconnect();
}

void GameClient::SendPositionMessage(const Math::Vector3f& aPosition, unsigned aNetworkID)
{
    NetMessage_Position positionMsg;
    positionMsg.SetPosition(aPosition);
    positionMsg.SetNetworkID(aNetworkID);
    positionMsg.SetTimestamp(std::chrono::system_clock::now());
    NetBuffer sendBuffer;
    positionMsg.Serialize(sendBuffer);
    Send(sendBuffer);

#ifdef DEBUG_SEND_POSITION
    printf("Sent position x: %f, y: %f, z: %f\n", aPosition.x, aPosition.y, aPosition.z);
#endif
}

void GameClient::HandleMessage_AcceptHandshake()
{
    myHasEstablishedHandshake = true;
#ifdef DEBUG_ESTABLISHED_HANDSHAKE
    printf("\nSuccessfully established handshake!\n");
#endif
}

void GameClient::HandleMessage_AcceptConnect()
{
    myHasEstablishedConnection = true;
#ifdef DEBUG_ESTABLISHED_CONNECTION
    printf("\nSuccessfully established connection!\n");
#endif
}

void GameClient::HandleMessage_Disconnect(NetMessage_Disconnect& aMessage)
{
#ifdef DEBUG_CLIENT_DISCONNECTED
    printf("\n[%s] has left the game.", aMessage.GetData().data());
#endif
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

    if (aMessage.GetIsPlayerCharacter())
    {
        auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ColorGreen.json")->material);
        model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);

        if (aMessage.GetIsControlledByClient())
        {
            go->AddComponent<Controller>(250.0f, 1.0f);
            myPlayerCharacter = go;
        }
        //else
        //{
        //    model->SetActive(false);
        //}
    }
    else
    {
        go->AddComponent<BoxCollider>(Math::Vector3f(50.0f, 100.0f, 50.0f), Math::Vector3f(0.0f, 90.0f, 0.0f));

        auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ColorBlue.json")->material);
        model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);
        //model->SetActive(false);
    }

    Engine::Get().GetSceneHandler().Instantiate(go);
}

void GameClient::HandleMessage_RemoveCharacter(NetMessage_RemoveCharacter& aMessage)
{
    auto& sceneHandler = Engine::Get().GetSceneHandler();
    if (auto go = sceneHandler.FindGameObjectByNetworkID(aMessage.GetNetworkID()))
    {
        Engine::Get().GetSceneHandler().Destroy(go);
    }
    
    if (myObjectIDPositionHistory.contains(aMessage.GetNetworkID()))
    {
        myObjectIDPositionHistory.erase(aMessage.GetNetworkID());
    }
}

void GameClient::HandleMessage_Position(NetMessage_Position& aMessage)
{
    if (myShouldLerpPositions)
    {
        PositionData data;
        data.position = aMessage.GetPosition();
        data.serverTimestamp = aMessage.GetTimestamp();
        data.clientTimestamp = std::chrono::system_clock::now();
        myObjectIDPositionHistory[aMessage.GetNetworkID()].Push_back(data);
    }
    else
    {
        if (auto go = Engine::Get().GetSceneHandler().FindGameObjectByNetworkID(aMessage.GetNetworkID()))
        {
            go->GetComponent<Transform>()->SetTranslation(aMessage.GetPosition());

            //if (myPlayerCharacter)
            //{
            //    go->GetComponent<AnimatedModel>()->SetActive((myPlayerCharacter->GetComponent<Transform>()->GetTranslation(true) - aMessage.GetPosition()).LengthSqr() < 500.0f * 500.0f);
            //}
        }
    }
}

void GameClient::HandleMessage_Test(NetMessage_Test& aMessage)
{
    printf("%i\n", aMessage.GetInt());
}

void GameClient::TryConnect()
{
    if (!myHasEstablishedHandshake)
    {
        std::chrono::duration<float> elapsed_seconds = std::chrono::system_clock::now() - myLastHandshakeRequestTime;
        if (elapsed_seconds.count() > (1.0f / NetworkDefines::Client::handshakeRequestRate))
        {
            myLastHandshakeRequestTime = std::chrono::system_clock::now();
            SendHandshakeRequest();
        }
    }

    if (myHasEstablishedHandshake && !myHasEstablishedConnection)
    {
        std::chrono::duration<float> elapsed_seconds = std::chrono::system_clock::now() - myLastConnectRequestTime;
        if (elapsed_seconds.count() > (1.0f / NetworkDefines::Client::connectRequestRate))
        {
            myLastConnectRequestTime = std::chrono::system_clock::now();
            SendConnectionRequest(myUsername);
        }
    }
}

void GameClient::UpdateNetworkStats()
{
    std::chrono::duration<float> elapsed_seconds = std::chrono::system_clock::now() - myLastDataTickTime;
    if (elapsed_seconds.count() > NetworkDefines::networkStatsUpdateRate)
    {
        myLastDataTickTime = std::chrono::system_clock::now();
        myAvgDataReceived = static_cast<int>(std::roundf(myDataReceived / elapsed_seconds.count()));
        myAvgDataSent = static_cast<int>(std::roundf(myDataSent / elapsed_seconds.count()));
        myDataReceived = 0;
        myDataSent = 0;
    }
}

void GameClient::PingServer()
{
    std::chrono::duration<float> timeSinceLastPing = std::chrono::system_clock::now() - myLastPingTime;
    if (timeSinceLastPing.count() > (1.0f / NetworkDefines::Client::pingServerRate))
    {
        myLastPingTime = std::chrono::system_clock::now();

        RTTPingMessage pingMsg;
        int guaranteedMessageID = CreateNewGuaranteedMessage(&pingMsg);
        NetBuffer buffer;
        pingMsg.Serialize(buffer);
        Send(buffer);

        myGuaranteedMessageIDToData[guaranteedMessageID].myGuaranteedMessageBuffer = buffer;
    }
}

void GameClient::UpdateGuaranteedMessages()
{
    std::array<int, 100> messageIDsToRemove = {};
    int currentIndex = 0;

    for (auto& [guaranteedMessageID, guaranteedMessageData] : myGuaranteedMessageIDToData)
    {
        std::chrono::duration<float> elapsedTime = std::chrono::system_clock::now() - guaranteedMessageData.myLastSentTimestamp;
        if (elapsedTime.count() > NetworkDefines::guaranteedMessageTimeout)
        {
            if (guaranteedMessageData.myAttempts > NetworkDefines::guaranteedMessageMaxTimeouts)
            {
                SendDisconnectMessage();
                messageIDsToRemove[++currentIndex] = guaranteedMessageID;
            }
            else
            {
#ifdef DEBUG_SEND_GUARANTEED_MESSAGE
                printf("Resending guaranteed message with ID %i attempt %i\n", guaranteedMessageID, guaranteedMessageData.myAttempts);
#endif
                myLastPingTime = std::chrono::system_clock::now();
                ++myNrOfGuaranteedMessagesSent;
                Send(guaranteedMessageData.myGuaranteedMessageBuffer);
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

void GameClient::UpdateObjectPositions()
{
    if (!myShouldLerpPositions) return;

    std::array<int, 10> clientIDsToRemove = {};
    int currentIndex = 0;

    for (auto& [networkID, positionDataArray] : myObjectIDPositionHistory)
    {
        auto go = Engine::Get().GetSceneHandler().FindGameObjectByNetworkID(networkID);
        if (!go)
        {
            clientIDsToRemove[++currentIndex] = networkID;
            continue;
        }

        // Hide rendering of an object if we haven't received a position message for them in the last 3 seconds.
        //std::chrono::duration<float> timeSinceLastPosition = std::chrono::system_clock::now() - positionDataArray.Peek_Latest().serverTimestamp;
        //go->GetComponent<AnimatedModel>()->SetActive(timeSinceLastPosition.count() < 3.0f);

        if (positionDataArray.Size() >= 2)
        {
            PositionData lastPos = positionDataArray.Peek_Front();
            PositionData nextPos = positionDataArray.Peek_Next();

            std::chrono::duration<float> messageTimeDiff = std::chrono::system_clock::now() - nextPos.clientTimestamp;
            std::chrono::duration<float> timestampDiff = nextPos.serverTimestamp - lastPos.serverTimestamp;
            if (timestampDiff.count() == 0.0f) return;

            float t = messageTimeDiff.count() / timestampDiff.count();
            if (t >= 1.0f)
            {
                positionDataArray.Pop_Front();
            }

            Math::Vector3f lerpedPosition = Math::Vector3f::Lerp(lastPos.position, nextPos.position, t);
            go->GetComponent<Transform>()->SetTranslation(lerpedPosition);
        }
    }

    for (auto& clientIDToRemove : clientIDsToRemove)
    {
        if (myObjectIDPositionHistory.contains(clientIDToRemove))
        {
            myObjectIDPositionHistory.erase(clientIDToRemove);
        }
    }
}

void GameClient::SendPlayerCharacterPosition()
{
    std::chrono::duration<float> dt = std::chrono::system_clock::now() - myLastPositionSendTimestamp;
    if (dt.count() > (1.0f / NetworkDefines::Client::playerPositionSendTickRate))
    {
        myLastPositionSendTimestamp = std::chrono::system_clock::now();

        if (myPlayerCharacter)
        {
            if (auto transform = myPlayerCharacter->GetComponent<Transform>())
            {
                SendPositionMessage(myPlayerCharacter->GetComponent<Transform>()->GetTranslation(true), myPlayerCharacter->GetNetworkID());

                if (!Math::Vector3f::Equal(transform->GetTranslation(true), myLastPosition, 0.01f))
                {
                    myLastPosition = transform->GetTranslation(true);
                    SendPositionMessage(myPlayerCharacter->GetComponent<Transform>()->GetTranslation(true), myPlayerCharacter->GetNetworkID());
                }
            }
        }
    }
}

void GameClient::Disconnect()
{
    myHasEstablishedHandshake = false;
    myHasEstablishedConnection = false;
    myShouldReceive = false;
    myAttemptToConnect = false;
}

int GameClient::CreateNewGuaranteedMessage(GuaranteedNetMessage* aGuaranteedMessage)
{
    int id = myCurrentGuaranteedMessageID;
    ++myCurrentGuaranteedMessageID;
    aGuaranteedMessage->SetGuaranteedMessageID(id);

    myGuaranteedMessageIDToData[id].myLastSentTimestamp = std::chrono::system_clock::now();
    myGuaranteedMessageIDToData[id].myAttempts = 1;

#ifdef DEBUG_CREATE_GUARANTEED_MESSAGE
    printf("Created guaranteed message with ID %i - NetMessageType: %i\n", id, static_cast<int>(aGuaranteedMessage->GetType()));
#endif
    ++myNrOfGuaranteedMessagesSent;
    return id;
}