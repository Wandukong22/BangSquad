#pragma once

#include "CoreMinimal.h"
#include "BSSessionTypes.generated.h"

//세션 작업 상태
UENUM()
enum class EBSSessionState : uint8
{
	Idle,
	Creating,
	Finding,
	Joining,
	InSession,
	Leaving,
	Starting
};

UENUM()
enum class EBSSessionError : uint8
{
	None,

	// 초기화 및 환경
	OnlineSubsystemUnavailable,
	SessionInterfaceUnavailable,
	LocalPlayerUnavailable,

	// 요청 상태
	InvalidRequest,
	OperationAlreadyInProgress,
	RequestNotStarted,

	// 현재 세션 상태
	AlreadyInSession,
	NotInSession,

	// 검색 결과
	SearchResultNotFound,
	SearchResultExpired,

	// 참가 결과
	SessionIsFull,
	SessionDoesNotExist,
	AlreadyJoinedSession,
	CouldNotResolveConnectString,

	// 비동기 작업 실패
	CreateFailed,
	FindFailed,
	JoinFailed,
	LeaveFailed,
	StartFailed,

	Unknown
};

USTRUCT()
struct FBSCreateSessionRequest
{
	GENERATED_BODY()

	FString RoomName;
	FString HostName;
	int32 MaxPlayers = 4;
};

USTRUCT()
struct FBSSessionSummary
{
	GENERATED_BODY()

	//결과 ID
	FGuid ResultId;
	FString RoomName;
	FString HostName;
	int32 CurrentPlayerCount = 0;
	int32 MaxPlayerCount = 0;
	int32 PingInMs = 0;
	
};
