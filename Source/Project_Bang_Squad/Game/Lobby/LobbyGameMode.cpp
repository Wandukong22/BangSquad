// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Lobby/LobbyGameMode.h"

#include "LobbyGameState.h"
#include "LobbyPlayerController.h"
#include "LobbyPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Project_Bang_Squad/Core/BSGameFlowSubsystem.h"

ALobbyGameMode::ALobbyGameMode()
{
	PlayerStateClass = ALobbyPlayerState::StaticClass();
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
	GameStateClass = ALobbyGameState::StaticClass();
}

EJobClaimResult ALobbyGameMode::TryClaimJob(EJobType Job, ALobbyPlayerState* RequestingPS)
{
	EJobType OldJob = EJobType::None;
	
	auto LogJobClaim = [&](EJobClaimResult Result)
	{
		const FString PlayerName = RequestingPS
			? RequestingPS->GetPlayerName()
			: TEXT("Unknown");

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[JobClaim] Player=%s Requested=%s Previous=%s Result=%s"),
			*PlayerName,
			*UEnum::GetValueAsString(Job),
			*UEnum::GetValueAsString(OldJob),
			*UEnum::GetValueAsString(Result)
		);
	};
	
	//요청자 확인
	if (!RequestingPS)
	{
		LogJobClaim(EJobClaimResult::InvalidPlayer);
		return EJobClaimResult::InvalidPlayer;
	}

	//LobbyGameState 확인
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS)
	{
		LogJobClaim(EJobClaimResult::InternalError);
		return EJobClaimResult::InternalError;
	}

	//Phase 확인
	if (GS->GetCurrentLobbyPhase() != ELobbyPhase::SelectJob)
	{
		LogJobClaim(EJobClaimResult::InvalidPhase);
		return EJobClaimResult::InvalidPhase;
	}

	//요청 직업 확인
	if (!IsPlayableJob(Job))
	{
		LogJobClaim(EJobClaimResult::InvalidJob);
		return EJobClaimResult::InvalidJob;
	}

	//요청 플레이어의 확정 직업 가져와서 저장
	OldJob = RequestingPS->GetJob();

	//이미 같은 직업을 확정한 상태인지
	bool bIsMyConfirmedJob = (OldJob != EJobType::None && OldJob == Job);
	if (!bIsMyConfirmedJob)
	{
		// 내 직업이 아니라면, 빈자리인지 철저히 검사
		if (!GS->IsJobAvailable(Job))
		{
			LogJobClaim(EJobClaimResult::AlreadyTaken);
			return EJobClaimResult::AlreadyTaken; // 누군가 이미 가져갔음 -> 실패
		}
	}

	if (OldJob != EJobType::None && OldJob != Job)
	{
		GS->RemoveTakenJob(OldJob);
	}

	//성공
	GS->AddTakenJob(Job);

	//플레이어 상태 업데이트
	RequestingPS->SetJob(Job);

	CheckConfirmedJob();

	LogJobClaim(EJobClaimResult::Success);
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
	if (bAllReady && GS->PlayerArray.Num() == PlayerCount)
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

	////모두 직업 확정 완료
	//if (ConfirmedCount == TotalPlayers)
	//{
	//	//상태 변경
	//	if (GS)
	//		GS->SetLobbyPhase(ELobbyPhase::GameStarting);

	//	//상태 동기화 시간 벌기 위해 1초 뒤 이동
	//	FTimerHandle TravelTimer;
	//	GetWorldTimerManager().SetTimer(TravelTimer, [this, WeakThis = TWeakObjectPtr<ALobbyGameMode>(this)]()
	//	{
	//		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
	//		{
	//			GI->MoveToStage(EStageIndex::Stage1, EStageSection::Main);
	//		}
	//	}, 1.f, false);
	//}

	if (ConfirmedCount == TotalPlayers)
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

bool ALobbyGameMode::IsPlayableJob(EJobType Job) const
{
	return (Job == EJobType::Titan ||
		Job == EJobType::Striker ||
		Job == EJobType::Mage ||
		Job == EJobType::Paladin);
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
