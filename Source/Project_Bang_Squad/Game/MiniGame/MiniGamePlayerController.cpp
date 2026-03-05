// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGamePlayerController.h"

#include "MiniGameState.h"
#include "Project_Bang_Squad/UI/MiniGame/CountdownWidget.h"
#include "Project_Bang_Squad/UI/MiniGame/MiniGameWidget.h"

void AMiniGamePlayerController::OnPhaseChanged_Implementation(EMiniGamePhase NewPhase)
{
	switch (NewPhase)
	{
		case EMiniGamePhase::Waiting: HandleWaiting(); break;
		case EMiniGamePhase::Playing: HandlePlaying(); break;
		case EMiniGamePhase::Finished: HandleFinished(); break;
	}
}

void AMiniGamePlayerController::Client_UpdateCountdown_Implementation(int32 Count)
{
	if (UMiniGameWidget* MiniWidget = Cast<UMiniGameWidget>(GameWidget))
	{
		if (MiniWidget->CountdownWidget)
		{
			MiniWidget->CountdownWidget->UpdateCountdown(Count);
		}
	}
}

void AMiniGamePlayerController::Client_ShowMiniGameResult_Implementation(EStageIndex Stage,
	const TArray<AMiniGamePlayerState*>& Players, const TArray<int32>& Ranks, const TArray<int32>& Rewards)
{
	if (UMiniGameWidget* MiniWidget = Cast<UMiniGameWidget>(GameWidget))
	{
		MiniWidget->ShowResultBoard(Stage, Players, Ranks, Rewards);
	}
}

void AMiniGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		HandleWaiting();
	}
}

void AMiniGamePlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	if (AMiniGameState* GS = GetWorld()->GetGameState<AMiniGameState>())
	{
		OnPhaseChanged(GS->GetCurrentPhase());
	}
	else
	{
		HandleWaiting();
	}
}

void AMiniGamePlayerController::HandleWaiting()
{
	SetGameInputEnabled(false);
}

void AMiniGamePlayerController::HandlePlaying()
{
	SetGameInputEnabled(true);

	if (UMiniGameWidget* MiniWidget = Cast<UMiniGameWidget>(GameWidget))
	{
		if (MiniWidget->CountdownWidget)
		{
			MiniWidget->CountdownWidget->UpdateCountdown(0);
		}
	}
}

void AMiniGamePlayerController::HandleFinished()
{
	SetGameInputEnabled(false);
}
