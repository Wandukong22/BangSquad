// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaMiniGameMode.h"

#include "ArenaGameState.h"
#include "ArenaPlayerController.h"
#include "ArenaPlayerState.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"

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

		if (AlivePlayerCount <= 1)
		{
			EndArena();
			/*GetWorldTimerManager().ClearTimer(ArenaTimerHandle);

			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				APlayerController* PC = Cast<APlayerController>(It->Get());
				if (PC)
				{
					AArenaPlayerState* ArenaPS = Cast<AArenaPlayerState>(PC->PlayerState);
					if (ArenaPS && ArenaPS->GetIsAlive() && PC->GetPawn())
					{
						PC->GetPawn()->SetCanBeDamaged(false);
						//TODO: UI 띄우기
						break;
					}
				}
			}

			//Stage3 Main으로 이동
			FTimerHandle ReturnTimerHandle;
			GetWorldTimerManager().SetTimer(ReturnTimerHandle, FTimerDelegate::CreateLambda([this]()
			{
				if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
				{
					GI->MoveToStage(EStageIndex::Stage3, EStageSection::Main);
				}
			}), 5.f, false);*/
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
	}

	GetWorldTimerManager().SetTimer(
		  ArenaTimerHandle,
		  this,
		  &AArenaMiniGameMode::TickArenaTimer,
		  1.f,
		  true
	  );
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
		}
		else
		{
			GetWorldTimerManager().ClearTimer(ArenaTimerHandle);
		}
	}
	else if (Phase == EArenaPattern::FloorSinking)
	{
		GS->SetCurrentSinkingFloor(GS->GetCurrentSinkingFloor() + 1);
		GS->SetCurrentPhase(EArenaPattern::Surviving);
		GS->SetRemainingTime(SurvivingDuration);
		BroadcastPhaseChanged(EArenaPattern::Surviving);
	}
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

	// N초 후 Stage3 Main으로 이동
	FTimerHandle ReturnHandle;
	GetWorldTimerManager().SetTimer(ReturnHandle, FTimerDelegate::CreateLambda([this]()
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			GI->MoveToStage(EStageIndex::Stage3, EStageSection::Main);
		}
	}), 5.f, false);
}

/*void AArenaMiniGameMode::UpdateArenaTimer()
{
	AArenaGameState* GS = GetGameState<AArenaGameState>();
	if (!GS) return;

	GS->SetRemainingTime(GS->GetRemainingTime() - 1);

	if (GS->GetRemainingTime() <= 0)
	{
		EArenaPattern CurrentPhase = GS->GetCurrentPhase();
		//페이즈 전환
		if (CurrentPhase == EArenaPattern::None)
		{
			GS->SetCurrentPhase(EArenaPattern::Surviving);
			GS->SetRemainingTime(20);
		}
		else if (CurrentPhase == EArenaPattern::Surviving)
		{
			GS->SetCurrentPhase(EArenaPattern::FloorSinking);
			GS->SetRemainingTime(5);
			//TODO: 바닥 가라앉는 이벤트?
			int32 TargetFloorNum = GS->GetCurrentSinkingFloor();

			/*
			for (TActorIterator<AArenaFloor> It(GetWorld()); It; ++It)
			{
				AArenaFloor* Floor = *It;
				if (Floor && Floor->FloorNumber == TargetFloorNum)
				{
					// 멀티캐스트 RPC나 RepNotify를 통해 클라이언트 측에서도 가라앉는 연출 실행
					Floor->Multicast_StartSinking(); 
				}
			}
			#1#
		}
		else if (CurrentPhase == EArenaPattern::FloorSinking)
		{
			GS->SetCurrentSinkingFloor(GS->GetCurrentSinkingFloor() + 1);
			if (GS->GetCurrentSinkingFloor() <= 2)
			{
				GS->SetCurrentPhase(EArenaPattern::Surviving);
				GS->SetRemainingTime(20);
			}
			else
			{
				GetWorldTimerManager().ClearTimer(ArenaTimerHandle);
			}
		}
	}
}*/
