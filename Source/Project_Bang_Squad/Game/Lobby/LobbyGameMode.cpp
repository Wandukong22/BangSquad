// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Lobby/LobbyGameMode.h"

#include "LobbyGameState.h"
#include "LobbyPlayerController.h"
#include "LobbyPlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/GameStateBase.h"
#include "Project_Bang_Squad/Core/BSGameFlowSubsystem.h"
#include "Project_Bang_Squad/Online/BSSessionSubsystem.h"

ALobbyGameMode::ALobbyGameMode()
{
	PlayerStateClass = ALobbyPlayerState::StaticClass();
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
	GameStateClass = ALobbyGameState::StaticClass();
}

EJobClaimResult ALobbyGameMode::TryClaimJob(EJobType Job, ALobbyPlayerController* Requester)
{
	
	/*EJobType OldJob = EJobType::None;
	auto LogJobClaim = [&](EJobClaimResult Result)
	{
		int32 OwnerCount = 0;

		if (const ALobbyGameState* CurrentGS = GetGameState<ALobbyGameState>())
		{
			for (const APlayerState* PlayerState : CurrentGS->PlayerArray)
			{
				const ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PlayerState);

				if (LobbyPS && LobbyPS->GetJob() == Job)
				{
					++OwnerCount;
				}
			}
		}
		
		const FString PlayerName = RequestingPS
			? RequestingPS->GetPlayerName()
			: TEXT("Unknown");

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[JobClaim] Player=%s Requested=%s Previous=%s Result=%s OwnerCount=%d"),
			*PlayerName,
			*UEnum::GetValueAsString(Job),
			*UEnum::GetValueAsString(OldJob),
			*UEnum::GetValueAsString(Result),
			OwnerCount
		);

		if (OwnerCount > 1)
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("[JobClaimInvariantViolation] Job=%s OwnerCount=%d"),
				*UEnum::GetValueAsString(Job),
				OwnerCount
			);
		}
	};*/
	if (!Requester) return EJobClaimResult::InvalidPlayer;

	//요청자 확인
	ALobbyPlayerState* LobbyPS = Requester->GetPlayerState<ALobbyPlayerState>();
	if (!LobbyPS) return EJobClaimResult::InvalidPlayer;

	//LobbyGameState 확인
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS) return EJobClaimResult::InternalError;
	
	//Phase 확인
	if (GS->GetCurrentLobbyPhase() != ELobbyPhase::SelectJob) return EJobClaimResult::InvalidPhase;
	
	//요청 직업 확인
	if (!IsPlayableJob(Job)) return EJobClaimResult::InvalidJob;
	
	//이미 같은 직업을 확정한 상태인지
	if (!IsJobAvailable(Job, LobbyPS)) return EJobClaimResult::AlreadyTaken;
	
	//플레이어 상태 업데이트
	LobbyPS->SetJob(Job);

	CheckConfirmedJob();

	return EJobClaimResult::Success;
}

bool ALobbyGameMode::TryPreviewJob(ALobbyPlayerController* Requester, EJobType NewJob)
{
	//GameState 확인
	ALobbyGameState* LobbyGameState = GetGameState<ALobbyGameState>();

	//PlayerController 확인
	if (!Requester)
	{
		LogActionRejected(TEXT("PreviewJob"), Requester, LobbyGameState, TEXT("InvalidRequester"));
		return false;
	}

	if (!LobbyGameState)
	{
		LogActionRejected(TEXT("PreviewJob"), Requester, LobbyGameState, TEXT("MissingGameState"));
		return false;
	}

	//현재 Phase가 PreviewJob인지 확인
	if (LobbyGameState->GetCurrentLobbyPhase() != ELobbyPhase::PreviewJob)
	{
		LogActionRejected(TEXT("PreviewJob"), Requester, LobbyGameState, TEXT("InvalidPhase"));
		return false;
	}

	//플레이 가능한 직업인지 확인
	if (!IsPlayableJob(NewJob))
	{
		LogActionRejected(TEXT("PreviewJob"), Requester, LobbyGameState, TEXT("InvalidJob"));
		return false;
	}

	//Requester의 LobbyPlayerState 획득
	ALobbyPlayerState* LobbyPlayerState = Requester->GetPlayerState<ALobbyPlayerState>();
	if (!LobbyPlayerState)
	{
		LogActionRejected(TEXT("PreviewJob"), Requester, LobbyGameState, TEXT("MissingPlayerState"));
		return false;
	}

	//PreviewJob 변경
	LobbyPlayerState->SetPreviewJob(NewJob);
	//미리보기 캐릭터 생성
	SpawnPlayerCharacter(Requester, NewJob);
	return true;
}

bool ALobbyGameMode::TryToggleReady(ALobbyPlayerController* Requester)
{
	//LobbyGameState 확인
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();

	//Requester 확인
	if (!Requester)
	{
		LogActionRejected(TEXT("ToggleReady"), Requester, GS, TEXT("InvalidRequester"));
		return false;
	}

	if (!GS)
	{
		LogActionRejected(TEXT("ToggleReady"), Requester, GS, TEXT("MissingGameState"));
		return false;
	}
	
	//현재 Phase가 PreviewJob인지 확인
	if (GS->GetCurrentLobbyPhase() != ELobbyPhase::PreviewJob)
	{
		LogActionRejected(TEXT("ToggleReady"), Requester, GS, TEXT("InvalidPhase"));
		return false;
	}
	
	//Requester의 LobbyPlayerState 획득
	ALobbyPlayerState* PS = Requester->GetPlayerState<ALobbyPlayerState>();
	if (!PS)
	{
		LogActionRejected(TEXT("ToggleReady"), Requester, GS, TEXT("MissingPlayerState"));
		return false;
	}
	
	//SetIsReady(!LobbyPlayerState->bIsReady) 실행
	PS->SetIsReady(!PS->GetIsReady());
	
	//CheckAllReady() 실행
	CheckAllReady();
	return true;
}

void ALobbyGameMode::CheckAllReady()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS) return;

	if (GS->PlayerArray.Num() == 0) return;

	bool bAllReady = true;
	int32 ReadyCount = 0;

	for (APlayerState* PS : GS->PlayerArray)
	{
		ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS);
		if (LobbyPS)
		{
			if (LobbyPS->GetIsReady()) ReadyCount++;
			else bAllReady = false;
		}
	}

	//이동
	if (bAllReady && GS->PlayerArray.Num() == RequiredPlayerCount)
	{
		GS->SetCurrentLobbyPhase(ELobbyPhase::SelectJob);
	}
}

void ALobbyGameMode::CheckConfirmedJob()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS) return;

	if (GS->PlayerArray.Num() == 0) return;

	int32 ConfirmedCount = 0;
	int32 TotalPlayers = GS->PlayerArray.Num();

	for (APlayerState* PS : GS->PlayerArray)
	{
		ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS);
		if (LobbyPS && LobbyPS->GetJob() != EJobType::None)
		{
			ConfirmedCount++;
		}
	}

	if (ConfirmedCount == RequiredPlayerCount && TotalPlayers == RequiredPlayerCount)
	{
		if (GS)
		{
			GS->SetCurrentLobbyPhase(ELobbyPhase::GameStarting);
			GS->SkipVoteCount = 0; // 혹시 모르니 투표수 0으로 초기화
		}

		float VideoDuration = 29.0f; // 🎬 영상 길이에 맞춰 세팅하세요!

		// ForceStartGame을 호출하는 타이머 실행
		GetWorldTimerManager().SetTimer(VideoTravelTimerHandle, this, &ALobbyGameMode::ForceStartGame, VideoDuration,
		                                false);
	}
}


bool ALobbyGameMode::TryRegisterSkipVote(ALobbyPlayerController* Requester)
{
	//LobbyGameState 확인
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();

	if (!Requester)
	{
		LogActionRejected(TEXT("SkipVideo"), Requester, GS, TEXT("InvalidRequester"));
		return false;
	}

	if (!GS)
	{
		LogActionRejected(TEXT("SkipVideo"), Requester, GS, TEXT("MissingGameState"));
		return false;
	}

	//현재 Phase가 GameStarting인지 확인
	if (GS->GetCurrentLobbyPhase() != ELobbyPhase::GameStarting)
	{
		LogActionRejected(TEXT("SkipVideo"), Requester, GS, TEXT("InvalidPhase"));
		return false;
	}

	if (GS->PlayerArray.Num() <= 0)
	{
		LogActionRejected(TEXT("SkipVideo"), Requester, GS, TEXT("NoPlayers"));
		return false;
	}
	
	//통과하면 SkipVoteCount 증가
	GS->SkipVoteCount++;
	
	//서버 UI 갱신 및 복제 처리
	GS->OnRep_SkipVoteCount();
	
	//전체 인원 투표 완료 시 ForceStartGame()
	if (GS->SkipVoteCount >= GS->PlayerArray.Num())
	{
		ForceStartGame();
	}
	return true;
}

void ALobbyGameMode::ForceStartGame()
{
	// 돌고 있던 영상 대기 타이머 강제 취소
	GetWorldTimerManager().ClearTimer(VideoTravelTimerHandle);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UBSGameFlowSubsystem* GameFlowSubsystem = GI->GetSubsystem<UBSGameFlowSubsystem>())
		{
			GameFlowSubsystem->ServerTravelToStage(EStageIndex::Stage1, EStageSection::Main);
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	FString LeavingPlayerName = TEXT("Unknown");
	EJobType ReleasedJob = EJobType::None;

	if (Exiting)
	{
		if (const ALobbyPlayerState* PS = Exiting->GetPlayerState<ALobbyPlayerState>())
		{
			LeavingPlayerName = PS->GetPlayerName();
			ReleasedJob = PS->GetJob();
		}
	}
	APlayerState* LeavingPlayerState = Exiting ? Exiting->PlayerState : nullptr;
	
	Super::Logout(Exiting);
	
	int32 RemainingOwnerCount = 0;

	if (const ALobbyGameState* GS = GetGameState<ALobbyGameState>())
	{
		for (const APlayerState* PlayerState : GS->PlayerArray)
		{
			const ALobbyPlayerState* LobbyPS =
				Cast<ALobbyPlayerState>(PlayerState);

			if (ReleasedJob != EJobType::None &&
				PlayerState != LeavingPlayerState &&
				LobbyPS &&
				LobbyPS->GetJob() == ReleasedJob)
			{
				++RemainingOwnerCount;
			}
		}
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[JobRelease] Player=%s ReleasedJob=%s RemainingOwnerCount=%d"),
		*LeavingPlayerName,
		*UEnum::GetValueAsString(ReleasedJob),
		RemainingOwnerCount
	);
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UBSGameInstance* GameInstance = GetGameInstance<UBSGameInstance>())
	{
		if (UBSSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UBSSessionSubsystem>())
		{
			RequiredPlayerCount = SessionSubsystem->GetMaxPlayerNum();
			if (RequiredPlayerCount <= 0 || RequiredPlayerCount > 4) RequiredPlayerCount = 4;
		}
	}

	//NetDriver 확인
	if (UWorld* World = GetWorld())
	{
		UNetDriver* NetDriver = World->GetNetDriver();
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[BSSession][LobbyGameMode][Host] NetMode=%d, NetDriver=%s"),
			static_cast<int32>(World->GetNetMode()),
			NetDriver ? *NetDriver->GetName() : TEXT("NULL")
		);

		IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(World);
		if (OnlineSubsystem)
		{
			const int32 Port = GetPortFromNetDriver(OnlineSubsystem->GetInstanceName());

			UE_LOG(
				LogTemp,
				Warning,
				TEXT("[BSSession][LobbyGameMode][DEBUG] Instance=%s, GetPortFromNetDriver=%d"),
				*OnlineSubsystem->GetInstanceName().ToString(),
				Port
			);
		}
		
		UE_LOG(LogTemp, Warning,
			TEXT("[Lobby] URL=%s"),
			*World->URL.ToString());

		UE_LOG(LogTemp, Warning,
			TEXT("[Lobby] Has listen=%s"),
			World->URL.HasOption(TEXT("listen"))
				? TEXT("true")
				: TEXT("false"));
	}
}

bool ALobbyGameMode::IsPlayableJob(EJobType Job) const
{
	return (Job == EJobType::Titan ||
		Job == EJobType::Striker ||
		Job == EJobType::Mage ||
		Job == EJobType::Paladin);
}

bool ALobbyGameMode::IsJobAvailable(EJobType RequestedJob, const ALobbyPlayerState* RequestingPlayerState) const
{
	//LobbyGameState 획득
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS) return false;
	
	//PlayerArray를 순회
	for (APlayerState* PlayerState : GS->PlayerArray)
	{
		//각 항목을 ALobbyPlayerState로 변환
		ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState);
		if (!LobbyPlayerState) continue;

		//변환 실패 항목과 요청자 자신의 PlayerState는 건너뜀
		if (LobbyPlayerState == RequestingPlayerState) continue;
		
		//다른 플레이어의 GetJob()이 RequestedJob과 같으면 즉시 false.
		if (LobbyPlayerState->GetJob() == RequestedJob) return false;
	}
	//끝까지 없으면 true
	return true;
}

void ALobbyGameMode::LogActionRejected(const TCHAR* Action, ALobbyPlayerController* Requester,
                                       const ALobbyGameState* GS, const TCHAR* Reason) const
{
	FString PlayerName = TEXT("Unknown");

	if (Requester)
	{
		if (const ALobbyPlayerState* PS = Requester->GetPlayerState<ALobbyPlayerState>())
		{
			PlayerName = PS->GetPlayerName();
		}
	}

	const FString PhaseName = GS
		? UEnum::GetValueAsString(GS->GetCurrentLobbyPhase())
		: TEXT("Unknown");

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[LobbyAction] Action=%s Player=%s Phase=%s Result=Rejected Reason=%s"),
		Action,
		*PlayerName,
		*PhaseName,
		Reason
	);
}
