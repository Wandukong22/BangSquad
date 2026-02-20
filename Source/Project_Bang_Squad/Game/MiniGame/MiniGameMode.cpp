// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/MiniGame/MiniGameMode.h"

#include "EngineUtils.h"
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
	PlayerControllerClass = AStagePlayerController::StaticClass();
	GameStateClass = AStageGameState::StaticClass();

	BaseRespawnTime = 5.f;
}

void AMiniGameMode::OnPlayerReachedGoal(AController* ReachedPlayer, EStageIndex StageIndex)
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
	CheckAllPlayersFinished(StageIndex);
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

void AMiniGameMode::CheckAllPlayersFinished(EStageIndex StageIndex)
{
	int32 TotalPlayers = GetNumPlayers();
	if (TotalPlayers <= 0) return;

	if (FinishedPlayers.Num() >= 1)
	{
		/*// 부모님(ABSGameMode)에게 줄 '플레이어 컨트롤러 리스트' 만들기
		TArray<APlayerController*> RankedList;

		// FinishedPlayers는 이미 도착순(1등, 2등..)으로 저장되어 있음
		for (AController* Ctrl : FinishedPlayers)
		{
			if (APlayerController* PC = Cast<APlayerController>(Ctrl))
			{
				RankedList.Add(PC);
			}
		}*/
		
		TArray<APlayerController*> Players;

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = Cast<APlayerController>(*It))
			{
				Players.Add(PC);
			}
		}

		Players.Sort([](const APlayerController& A, const APlayerController& B)
		{
			AMiniGamePlayerState* APS = A.GetPlayerState<AMiniGamePlayerState>();
			AMiniGamePlayerState* BPS = B.GetPlayerState<AMiniGamePlayerState>();

			float ScoreA = APS ? APS->GetMiniGameProgressScore() : 0.f;
			float ScoreB = BPS ? BPS->GetMiniGameProgressScore() : 0.f;

			return ScoreA > ScoreB;
		});

		TArray<APlayerController*> RankedList;
		for (int32 i = 0; i < Players.Num(); i++)
		{
			APlayerController* PC = Players[i];
			RankedList.Add(PC);

			if (AMiniGamePlayerState* PS = PC->GetPlayerState<AMiniGamePlayerState>())
			{
				if (PS->GetMiniGameRank() == 0)
				{
					PS->SetMiniGameRank(i + 1);
				}
			}
		}
		GiveMiniGameReward(RankedList);

		// =================================================================
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			GI->MoveToStage(StageIndex, EStageSection::Main);
		}
	}
}
