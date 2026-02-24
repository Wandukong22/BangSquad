// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaPlayerController.h"

#include "ArenaPlayerState.h"
#include "Project_Bang_Squad/UI/MiniGame/ArenaMainWidget.h"

void AArenaPlayerController::Local_OnArenaPhaseChanged(EArenaPattern NewPhase)
{
	switch (NewPhase)
	{
	case EArenaPattern::Waiting:      HandleWaiting();      break;
	case EArenaPattern::Surviving:    HandleSurviving();    break;
	case EArenaPattern::FloorSinking: HandleFloorSinking(); break;
	case EArenaPattern::Finished:     HandleFinished();     break;
	}
}

void AArenaPlayerController::Client_UpdateWaitingCountdown_Implementation(int32 Count)
{
	if (UArenaMainWidget* ArenaMainWidget = Cast<UArenaMainWidget>(GameWidget))
	{
		ArenaMainWidget->UpdateWaitingCountdown(Count);
	}
}

void AArenaPlayerController::Client_UpdateSurvivingTimer_Implementation(int32 RemainingTime)
{
	if (UArenaMainWidget* ArenaMainWidget = Cast<UArenaMainWidget>(GameWidget))
	{
		if (RemainingTime < 0)
		{
			ArenaMainWidget->SetSurvivingTimerVisible(false);
		}
		else
		{
			ArenaMainWidget->SetSurvivingTimerVisible(true);
			ArenaMainWidget->UpdateSurvivingTimer(RemainingTime);
		}
	}
}

void AArenaPlayerController::StartSpectating()
{
	//부활 없이 관전만
	ViewNextPlayer();
}

void AArenaPlayerController::Client_ShowArenaResult_Implementation(const TArray<class AArenaPlayerState*>& Players,
	const TArray<int32>& Ranks, const TArray<int32>& Rewards)
{
	if (UArenaMainWidget* ArenaMainWidget = Cast<UArenaMainWidget>(GameWidget))
	{
		ArenaMainWidget->ShowRankingBoard(Players, Ranks, Rewards);
	}
}

void AArenaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		HandleWaiting();
	}
}

void AArenaPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	if (AArenaGameState* GS = GetWorld()->GetGameState<AArenaGameState>())
	{
		Local_OnArenaPhaseChanged(GS->GetCurrentPhase());
	}
	else
	{
		HandleWaiting();
	}
}

void AArenaPlayerController::HandleWaiting()
{
	SetGameInputEnabled(false);
}

void AArenaPlayerController::HandleSurviving()
{
	SetGameInputEnabled(true);
}

void AArenaPlayerController::HandleFloorSinking()
{
	if (UArenaMainWidget* ArenaMainWidget = Cast<UArenaMainWidget>(GameWidget))
	{
		ArenaMainWidget->SetSurvivingTimerVisible(false);
	}
}

void AArenaPlayerController::HandleFinished()
{
	SetGameInputEnabled(false);
}
