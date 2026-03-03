// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/MiniGame/MiniGameMode.h"

#include "EngineUtils.h"
#include "MiniGamePlayerController.h"
#include "MiniGamePlayerState.h"
#include "MiniGameState.h"
#include "GameFramework/Character.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Data/DataAsset/CoinRewardDataAsset.h"
#include "Project_Bang_Squad/Game/Stage/Checkpoint.h"
#include "Project_Bang_Squad/Game/Stage/StageGameState.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerController.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"


AMiniGameMode::AMiniGameMode()
{
	PlayerStateClass = AMiniGamePlayerState::StaticClass();
	PlayerControllerClass = AMiniGamePlayerController::StaticClass();
	GameStateClass = AMiniGameState::StaticClass();

	BaseRespawnTime = 5.f;
}

void AMiniGameMode::OnPlayerReachedGoal(AController* ReachedPlayer, EStageIndex StageIndex)
{
	AMiniGameState* GS = GetGameState<AMiniGameState>();
	if (!GS || GS->GetCurrentPhase() != EMiniGamePhase::Playing) return;
	if (!ReachedPlayer || FinishedPlayers.Contains(ReachedPlayer)) return;

	FinishedPlayers.Add(ReachedPlayer);

	//PlayerState에 순위 고정
	if (AMiniGamePlayerState* PS = ReachedPlayer->GetPlayerState<AMiniGamePlayerState>())
	{
		PS->SetMiniGameRank(FinishedPlayers.Num());
	}
	
	//미니게임 종료
	CheckAllPlayersFinished(StageIndex);
}

void AMiniGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(
		CountdownTimerHandle,
		this,
		&AMiniGameMode::TickCountdown,
		1.f,
		true
	);
}

void AMiniGameMode::EndMiniGame(EStageIndex StageIndex)
{
	AMiniGameState* GS = GetGameState<AMiniGameState>();
	if (!GS) return;

	GS->SetCurrentPhase(EMiniGamePhase::Finished);

	// 모든 플레이어에게 Finished 알림
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AMiniGamePlayerController* PC = Cast<AMiniGamePlayerController>(It->Get()))
		{
			PC->OnPhaseChanged(EMiniGamePhase::Finished);
		}
	}

	//순위 및 보상
	TArray<APlayerController*> Players;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = Cast<APlayerController>(*It))
		{
			Players.Add(PC);
		}
	}

	// 순위 정렬
	Players.Sort([](const APlayerController& A, const APlayerController& B)
	{
	   AMiniGamePlayerState* APS = A.GetPlayerState<AMiniGamePlayerState>();
	   AMiniGamePlayerState* BPS = B.GetPlayerState<AMiniGamePlayerState>();
       
	   int32 RankA = APS ? APS->GetMiniGameRank() : 0;
	   int32 RankB = BPS ? BPS->GetMiniGameRank() : 0;

	   // 둘 다 순위가 있다면 순위가 높은(숫자가 작은) 사람이 우선
	   if (RankA > 0 && RankB > 0) return RankA < RankB;
	   if (RankA > 0) return true;
	   if (RankB > 0) return false;

	   // 둘 다 완주하지 못했다면 진행도 점수로 비교
	   float ScoreA = APS ? APS->GetMiniGameProgressScore() : 0.f;
	   float ScoreB = BPS ? BPS->GetMiniGameProgressScore() : 0.f;
	   return ScoreA > ScoreB;
	});

	// 미도착자들에게 나머지 순위 부여 및 최종 보상 리스트 생성
	TArray<APlayerController*> RankedList;
	for (int32 i = 0; i < Players.Num(); i++)
	{
		APlayerController* PC = Players[i];
		RankedList.Add(PC);

		if (AMiniGamePlayerState* PS = PC->GetPlayerState<AMiniGamePlayerState>())
		{
			if (PS->GetMiniGameRank() == 0)
			{
				PS->SetMiniGameRank(i + 1); // 1등부터 순차 부여
			}
		}
	}

	// 보상 지급 (함수가 존재한다면 주석 해제하여 사용)
	GiveMiniGameReward(RankedList);

	TArray<AMiniGamePlayerState*> PlayerStateList;
	TArray<int32> Ranks;
	TArray<int32> Rewards;

	for (APlayerController* PC : RankedList)
	{
		AMiniGamePlayerState* PS = PC->GetPlayerState<AMiniGamePlayerState>();
		PlayerStateList.Add(PS);

		int32 Rank = PS ? PS->GetMiniGameRank() : 0;
		Ranks.Add(Rank);

		int32 Coin = 0;
		if (RewardDataAsset && RewardDataAsset->RewardMap.Contains(CurrentStageIndex))
		{
			const TArray<int32>& MiniGameRewards = RewardDataAsset->RewardMap[CurrentStageIndex].MiniGameRankRewards;
			
			if (MiniGameRewards.IsValidIndex(Rank - 1))
			{
				Coin = MiniGameRewards[Rank - 1];
			}
		}
		Rewards.Add(Coin);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AMiniGamePlayerController* PC = Cast<AMiniGamePlayerController>(It->Get()))
		{
			PC->Client_ShowMiniGameResult(PlayerStateList, Ranks, Rewards);
		}
	}

	// N초 후 다음 스테이지 Main으로 이동
	GetWorldTimerManager().SetTimer(ReturnTimerHandle, FTimerDelegate::CreateLambda([this, StageIndex]()
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			GI->MoveToStage(StageIndex, EStageSection::Main);
		}
	}), ReturnDelay, false);
}

void AMiniGameMode::TickCountdown()
{
	AMiniGameState* GS = GetGameState<AMiniGameState>();
	if (!GS || GS->GetCurrentPhase() != EMiniGamePhase::Waiting) return;

	int32 Current = GS->GetCountdown() - 1;
	GS->SetCountdown(Current);

	//클라이언트에 카운트다운 값 전달
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AMiniGamePlayerController* PC = Cast<AMiniGamePlayerController>(It->Get()))
		{
			PC->Client_UpdateCountdown(Current);
		}
	}
	
	if (Current <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

		GS->SetCurrentPhase(EMiniGamePhase::Playing);

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AMiniGamePlayerController* PC = Cast<AMiniGamePlayerController>(It->Get()))
			{
				PC->OnPhaseChanged(EMiniGamePhase::Playing);
			}
		}
	}
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
	if (FinishedPlayers.Num() >= 1)
	{
		EndMiniGame(StageIndex);
	}
	
	//int32 TotalPlayers = GetNumPlayers();
	//if (TotalPlayers <= 0) return;
	//
	//if (FinishedPlayers.Num() >= 1)
	//{
	//	/*// 부모님(ABSGameMode)에게 줄 '플레이어 컨트롤러 리스트' 만들기
	//	TArray<APlayerController*> RankedList;
	//
	//	// FinishedPlayers는 이미 도착순(1등, 2등..)으로 저장되어 있음
	//	for (AController* Ctrl : FinishedPlayers)
	//	{
	//		if (APlayerController* PC = Cast<APlayerController>(Ctrl))
	//		{
	//			RankedList.Add(PC);
	//		}
	//	}*/
	//
	//	TArray<APlayerController*> Players;
	//
	//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	//	{
	//		if (APlayerController* PC = Cast<APlayerController>(*It))
	//		{
	//			Players.Add(PC);
	//		}
	//	}
	//
	//	Players.Sort([](const APlayerController& A, const APlayerController& B)
	//	{
	//		AMiniGamePlayerState* APS = A.GetPlayerState<AMiniGamePlayerState>();
	//		AMiniGamePlayerState* BPS = B.GetPlayerState<AMiniGamePlayerState>();
	//
	//		float ScoreA = APS ? APS->GetMiniGameProgressScore() : 0.f;
	//		float ScoreB = BPS ? BPS->GetMiniGameProgressScore() : 0.f;
	//
	//		return ScoreA > ScoreB;
	//	});
	//
	//	TArray<APlayerController*> RankedList;
	//	for (int32 i = 0; i < Players.Num(); i++)
	//	{
	//		APlayerController* PC = Players[i];
	//		RankedList.Add(PC);
	//
	//		if (AMiniGamePlayerState* PS = PC->GetPlayerState<AMiniGamePlayerState>())
	//		{
	//			if (PS->GetMiniGameRank() == 0)
	//			{
	//				PS->SetMiniGameRank(i + 1);
	//			}
	//		}
	//	}
	//	GiveMiniGameReward(RankedList);
	//
	//	// =================================================================
	//	if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
	//	{
	//		GI->MoveToStage(StageIndex, EStageSection::Main);
	//	}
	//}
}
