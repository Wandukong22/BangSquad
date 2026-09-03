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

bool ALobbyGameMode::TryConfirmJob(EJobType Job, ALobbyPlayerState* RequestingPS)
{
	//요청된 PlayerState가 null이면 false
	if (!RequestingPS) return false;

	//LobbyGameState 가져오기
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS) return false;

	//요청 플레이어의 확정 직업 가져와서 저장
	EJobType OldJob = RequestingPS->GetJob();

	//이미 같은 직업을 확정한 상태인지
	bool bIsMyConfirmedJob = (OldJob != EJobType::None && OldJob == Job);
	if (!bIsMyConfirmedJob)
	{
		// 내 직업이 아니라면, 빈자리인지 철저히 검사
		if (!GS->IsJobAvailable(Job))
		{
			return false; // 누군가 이미 가져갔음 -> 실패
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
			if (LobbyPS->bIsReady) ReadyCount++;
			else bAllReady = false;
		}
	}

	//이동
	if (bAllReady && GS->PlayerArray.Num() == PlayerCount)
	{
		GS->SetLobbyPhase(ELobbyPhase::SelectJob);
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
			GS->SetLobbyPhase(ELobbyPhase::GameStarting);
			GS->SkipVoteCount = 0; // 혹시 모르니 투표수 0으로 초기화
		}

		float VideoDuration = 29.0f; // 🎬 영상 길이에 맞춰 세팅하세요!

		// ForceStartGame을 호출하는 타이머 실행
		GetWorldTimerManager().SetTimer(VideoTravelTimerHandle, this, &ALobbyGameMode::ForceStartGame, VideoDuration,
		                                false);
	}
}


void ALobbyGameMode::RegisterSkipVote()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (GS)
	{
		GS->SkipVoteCount++;
		GS->OnRep_SkipVoteCount(); // 서버 컴퓨터의 UI도 즉시 갱신되도록 강제 호출

		// 만장일치 확인
		if (GS->SkipVoteCount >= GS->PlayerArray.Num())
		{
			ForceStartGame();
		}
	}
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
