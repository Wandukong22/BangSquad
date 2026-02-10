// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/MiniGame/MiniGameMode.h"

#include "EngineUtils.h"
#include "MiniGamePlayerController.h"
#include "MiniGamePlayerState.h"
#include "GameFramework/Character.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Game/Stage/Checkpoint.h"
#include "Project_Bang_Squad/Game/Stage/StageGameState.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerController.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"


AMiniGameMode::AMiniGameMode()
{
	PlayerStateClass = AMiniGamePlayerState::StaticClass();
	PlayerControllerClass = AMiniGamePlayerController::StaticClass();
	GameStateClass = AStageGameState::StaticClass();

	BaseRespawnTime = 5.f;
}

void AMiniGameMode::OnPlayerReachedGoal(AController* ReachedPlayer)
{
	if (FinishedPlayers.Contains(ReachedPlayer)) return;

	FinishedPlayers.Add(ReachedPlayer);
	int32 Rank = FinishedPlayers.Num();

	//PlayerState에 순위 고정
	if (ReachedPlayer)
	{
		if (AMiniGamePlayerState* PS = ReachedPlayer->GetPlayerState<AMiniGamePlayerState>())
		{
			PS->SetMiniGameRank(Rank);
		}
	}
	//보상 로직

	//미니게임 종료
	CheckAllPlayersFinished();
}

FTransform AMiniGameMode::GetRespawnTransform(AController* Controller)
{
	UWorld* World = GetWorld();
	if (!World) return FTransform::Identity;

	if (Controller)
	{
		AMiniGamePlayerState* PS = Controller->GetPlayerState<AMiniGamePlayerState>();
		// 체크포인트 인덱스가 0보다 크면 해당 체크포인트를 찾음
		if (PS && PS->GetMiniGameCheckpoint() > 0)
		{
			int32 TargetIndex = PS->GetMiniGameCheckpoint();

			// 월드에 있는 체크포인트 액터 중 내 인덱스와 같은 것 검색
			for (TActorIterator<ACheckpoint> It(World); It; ++It)
			{
				ACheckpoint* Checkpoint = *It;
				if (Checkpoint && Checkpoint->GetCheckpointIndex() == TargetIndex)
				{
					return Checkpoint->GetActorTransform();
				}
			}
		}
	}

	return Super::GetRespawnTransform(Controller);
}

void AMiniGameMode::CheckAllPlayersFinished()
{
	int32 TotalPlayers = GetNumPlayers();
	if (TotalPlayers <= 0) return;

	if (FinishedPlayers.Num() >= 1)
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			GI->MoveToStage(EStageIndex::Stage1, EStageSection::Main);
		}
	}
}
