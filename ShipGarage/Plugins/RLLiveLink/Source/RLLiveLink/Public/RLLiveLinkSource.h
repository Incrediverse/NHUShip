// Copyright 2022 The Reallusion Authors. All Rights Reserved.

#pragma once
#include "ILiveLinkSource.h"
#include "HAL/ThreadSafeBool.h"
/// for Json
#include "JsonGlobals.h"
#include "Policies/JsonPrintPolicy.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonTypes.h"
#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonSerializerMacros.h"
/// Tcp Connection
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4SubnetMask.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Interfaces/IPv4/IPv4Subnet.h"
#include "Interfaces/Steam/SteamEndpoint.h"
#include "Interfaces/INetworkingModule.h"
#include "Common/TcpSocketBuilder.h"
#include "Common/TcpListener.h"
#include "Runtime/Launch/Resources/Version.h"

class ILiveLinkClient;
class RLLIVELINK_API FRLLiveLinkSource : public ILiveLinkSource, public FRunnable
{
public:
    FRLLiveLinkSource( uint32 uPort );
    virtual ~FRLLiveLinkSource();
    bool InitExpressionNames( const TSharedPtr<FJsonObject>& kJsonRoot, const FString& strFieldName, TArray< FName >& kExpNames );
    bool InitCustomExpressionNames( const TSharedPtr<FJsonObject>& pJsonRoot );
    bool InitVisemeNames( const TSharedPtr<FJsonObject>& pJsonRoot );
    bool InitBoneMap( const TSharedPtr<FJsonObject>& pJsonRoot );
    bool InitReParentMap( const TSharedPtr<FJsonObject>& pJsonRoot );

    // Begin ILiveLinkSource Interface
    virtual void ReceiveClient( ILiveLinkClient* pInClient, FGuid kInSourceGuid ) override;

#if ENGINE_MINOR_VERSION > 22 || ENGINE_MAJOR_VERSION >= 5
    virtual bool IsSourceStillValid() const override;
#else
    virtual bool IsSourceStillValid() override;
#endif

    virtual bool RequestSourceShutdown() override;

    virtual FText GetSourceType() const override { return m_kSourceType; };
    virtual FText GetSourceMachineName() const override { return m_kSourceMachineName; }
    virtual FText GetSourceStatus() const override { return m_kSourceStatus; }
    // End ILiveLinkSource Interface

    // Begin FRunnable Interface
    virtual bool Init() override { return true; }
    virtual uint32 Run() override;
    void Start();
    virtual void Stop() override;
    virtual void Exit() override {}
    // End FRunnable Interface

    void HandleReceivedData( TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> spReceivedData );

private:
    void ProcessStickyData( TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> spReceivedData );
    void ProcessAvatarData( const TSharedPtr<FJsonObject>& spDataRoot, const FName& kSubjectName, int nProductVersion );
    void ProcessPropData( const TSharedPtr<FJsonObject>& spDataRoot );
    void ProcessCameraData( const TSharedPtr<FJsonObject>& spDataRoot );
    void ProcessLightData( const TSharedPtr<FJsonObject>& spDataRoot );

    void ResetEncounteredSubjectsMap();
    void RemoveUnusedSubjects();
    void ClearAllSubjects();

private:
    ILiveLinkClient* m_pClient;

    // Our identifier in LiveLink
    FGuid m_kSourceGuid;

    FText m_kSourceType;
    FText m_kSourceMachineName;
    FText m_kSourceStatus;

    FIPv4Address m_kDeviceIPAddr;
    uint32       m_uDevicePort;

    // Tcp Server
    FSocket* m_pListenerSocket;
    FSocket* m_pConnectionSocket;
    FIPv4Endpoint m_kRemoteAddressForConnection;

    // Subsystem associated to Socket
    ISocketSubsystem* m_pSocketSubsystem;

    // Threadsafe Bool for terminating the main thread loop
    FThreadSafeBool m_bStopping;

    // Thread to run socket operations on
    FRunnableThread* m_pThread;

    // Name of the sockets thread
    FString m_strThreadName;

    // Time to wait between attempted receives
    FTimespan m_kWaitTime;

    // List of subjects we've already encountered
    TMap<FName, bool> m_kEncounteredSubjects;

    // Buffer to receive socket data into
    TArray<uint8> m_kRecvBuffer;

    // for time code
    uint32 m_uFps = -1;
    int m_nFrameIndex = -1;

    // frame counter for data
    int m_nFrameCounter;

    // Socket TCP ????????????
    int m_nDataSizeInHeader;           ///< ??????????????????header???????????????????????????
    TArray<uint8> m_kIncompleteData;   ///< ??????????????????header??????????????????????????????????????????????????????????????????????????????????????????
    TArray<uint8> m_kCompleteData;     ///< ?????????????????????header??????????????????????????????????????????

    // iClone ?????????????????????MorphTarget name
    TArray< FName > m_kExpressionNames;

    // iClone ???Custom??????????????????MorphTarget name
    TArray< FName > m_kCustomExpressionNames;
    TArray< FName > m_kHeadExpressionNames;
    TArray< FName > m_kLeftEyeExpressionNames;
    TArray< FName > m_kRightEyeExpressionNames;
    TArray< FName > m_kBonesExpressionNames;

    // iClone ???Viseme????????????MorphTarget name
    TArray< FName > m_kVisemeNames;

    // iClone ???Old Viseme????????????MorphTarget name ( CC ??????????????? )
    TArray< FName > m_kOldVisemeNames;

    // iClone ???Old Viseme????????????MorphTarget name ( CC ??????????????? ) ex:IC8 Heidi
    TArray< FName > m_kIC8OldVisemeNames;

    // iClone ???CC4 Viseme????????????MorphTarget name ( CC ??????????????? )
    TArray< FName > m_kCC4VisemeNames;

    // IC Motion Bone -> Unreal standard bone
    TMap< FString, FString > m_kBoneMap;

    // ??????Bone ????????????????????????????????????Bone
    TMap< FString, FString > m_kReParentMap;
};