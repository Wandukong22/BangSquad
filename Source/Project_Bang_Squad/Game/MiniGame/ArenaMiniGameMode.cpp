// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaMiniGameMode.h"

#include "ArenaGameState.h"
#include "ArenaPlayerController.h"
#include "ArenaPlayerState.h"
#include "EngineUtils.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/MapPuzzle/ArenaFloor.h"

AArenaMiniGameMode::AArenaMiniGameMode()
{
	PlayerStateClass = AArenaPlayerState::StaticClass();
	GameStateClass = AArenaGameState::StaticClass();
	PlayerControllerClass = AArenaPlayerController::StaticClass();
}

void AArenaMiniGameMode::OnPlayerDied(AController* VictimController)
{
	if (!VictimController) return;

	AArenaPlayerState* PS = VictimController->GetPlayerState<AArenaPlayerState>();
	if (PS && PS->GetIsAlive())
	{
		PS->SetIsAlive(false);
		PS->SetArenaRank(AlivePlayerCount);
		AlivePlayerCount--;

		if (AlivePlayerCount <= 1 && !bArenaEnded)
		{
			bArenaEnded = true;
			EndArena();
		}
	}
}

void AArenaMiniGameMode::BeginPlay()
{
	Super::BeginPlay();

	AlivePlayerCount = GetNumPlayers();

	GetWorldTimerManager().SetTimer(WaitingTimerHandle, this, &AArenaMiniGameMode::TickWaitingCountdown, 1.f, true);
}

void AArenaMiniGameMode::TickWaitingCountdown()
{
	AArenaGameState* GS = GetGameState<AArenaGameState>();
	if (!GS || GS->GetCurrentPhase() != EArenaPattern::Waiting) return;

	int32 Current = GS->GetRemainingTime() - 1;
	GS->SetRemainingTime(Current);
	
	//클라이언트에 카운트다운 숫자 전달
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AArenaPlayerController* PC = Cast<AArenaPlayerController>(It->Get()))
		{
			PC->Client_UpdateWaitingCountdown(Current);
		}
	}

	if (Current <= 0)
	{
		GetWorldTimerManager().ClearTimer(WaitingTimerHandle);

		GS->SetCurrentPhase(EArenaPattern::Surviving);
		GS->SetRemainingTime(SurvivingDuration);
		BroadcastPhaseChanged(EArenaPattern::Surviving);


		GetWorldTimerManager().SetTimer(
			ArenaTimerHandle,
			this,
			&AArenaMiniGameMode::TickArenaTimer,
			1.f,
			true
		);
	}
}

void AArenaMiniGameMode::TickArenaTimer()
{
	AArenaGameState* GS = GetGameState<AArenaGameState>();
	if (!GS) return;

	EArenaPattern Phase = GS->GetCurrentPhase();


	if (Phase == EArenaPattern::Finished || Phase == EArenaPattern::Waiting)
	{
		GetWorldTimerManager().ClearTimer(ArenaTimerHandle);
		return;
	}

	int32 Current = GS->GetRemainingTime() - 1;
	GS->SetRemainingTime(Current);


	if (Phase == EArenaPattern::Surviving)
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AArenaPlayerController* PC = Cast<AArenaPlayerController>(It->Get()))
			{
				PC->Client_UpdateSurvivingTimer(Current);
			}
		}
	}

	if (Current > 0) return;

	if (Phase == EArenaPattern::Surviving)
	{
		int32 NextFloor = GS->GetCurrentSinkingFloor();

		if (NextFloor <= MaxSinkingFloors)
		{
			GS->SetCurrentPhase(EArenaPattern::FloorSinking);
			GS->SetRemainingTime(FloorSinkingDuration);
			BroadcastPhaseChanged(EArenaPattern::FloorSinking);

			for (TActorIterator<AArenaFloor> It(GetWorld()); It; ++It)
			{
				if (It->GetFloorNumber() == NextFloor)
				{
					It->Multicast_StartSinking();
					break;
				}
			}
		}
		else
		{
			GetWorldTimerManager().ClearTimer(ArenaTimerHandle);

			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				if (AArenaPlayerController* PC = Cast<AArenaPlayerController>(It->Get()))
				{
					PC->Client_UpdateSurvivingTimer(-1);
				}
			}
		}
	}
	else if (Phase == EArenaPattern::FloorSinking)
	{
		int32 NextFloor = GS->GetCurrentSinkingFloor() + 1;
		GS->SetCurrentSinkingFloor(NextFloor);

		GS->SetCurrentPhase(EArenaPattern::Surviving);

		if (NextFloor <= MaxSinkingFloors)
		{
			GS->SetRemainingTime(SurvivingDuration);
		}
		else
		{
			GS->SetRemainingTime(-1);
		}
		BroadcastPhaseChanged(EArenaPattern::Surviving);
	}
}

void AArenaMiniGameMode::EndArena()
{
	GetWorldTimerManager().ClearTimer(ArenaTimerHandle);
	GetWorldTimerManager().ClearTimer(WaitingTimerHandle);

	AArenaGameState* GS = GetGameState<AArenaGameState>();
	if (!GS) return;

	GS->SetCurrentPhase(EArenaPattern::Finished);
	BroadcastPhaseChanged(EArenaPattern::Finished);
	
	// 마지막 생존자에게 1등 부여 + 데미지 막기
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = Cast<APlayerController>(It->Get());
		if (!PC) continue;

		AArenaPlayerState* ArenaPS = Cast<AArenaPlayerState>(PC->PlayerState);
		if (ArenaPS && ArenaPS->GetIsAlive())
		{
			ArenaPS->SetArenaRank(1);
			if (PC->GetPawn()) PC->GetPawn()->SetCanBeDamaged(false);
		}
	}

	//서버 기준 전체 플레이어 수집 및 정렬
	TArray<AArenaPlayerState*> PlayerList;
	if (GS)
	{
		for (APlayerState* PS : GS->PlayerArray)
		{
			if (AArenaPlayerState* ArenaPS = Cast<AArenaPlayerState>(PS))
			{
				PlayerList.Add(ArenaPS);
			}
		}
	}
	PlayerList.Sort([](const AArenaPlayerState& A, const AArenaPlayerState& B)
	{
		return A.GetArenaRank() < B.GetArenaRank();
	});

	//순위 및 코인 보상 데이터 분리
	TArray<int32> Ranks;
	TArray<int32> Rewards;
	for (AArenaPlayerState* ArenaPS : PlayerList)
	{
		int32 CurrentRank = ArenaPS->GetArenaRank();
		Ranks.Add(CurrentRank);

		int32 RewardCoin = 0;
		switch (CurrentRank)
		{
		case 1: RewardCoin = 100; break;
		case 2: RewardCoin = 70;  break;
		case 3: RewardCoin = 40;  break;
		case 4: RewardCoin = 20;  break;
		default: break;
		}
		Rewards.Add(RewardCoin);
	}

	//모든 클라이언트에게 확정된 배열 쏘기
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AArenaPlayerController* PC = Cast<AArenaPlayerController>(It->Get()))
		{
			PC->Client_ShowArenaResult(PlayerList, Ranks, Rewards);
		}
	}
	
	// 5초 후 Stage3 Main으로 이동
	FTimerHandle ReturnHandle;
	GetWorldTimerManager().SetTimer(ReturnHandle, FTimerDelegate::CreateLambda([this]()
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			GI->MoveToStage(EStageIndex::Stage3, EStageSection::Main);
		}
	}), 5.f, false);
}

void AArenaMiniGameMode::BroadcastPhaseChanged(EArenaPattern NewPhase)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AArenaPlayerController* PC = Cast<AArenaPlayerController>(It->Get()))
		{
			PC->Client_OnArenaPhaseChanged(NewPhase);
		}
	}
}
