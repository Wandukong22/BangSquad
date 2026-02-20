// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGamePlayerController.h"

void AMiniGamePlayerController::OnPhaseChanged_Implementation(EMiniGamePhase NewPhase)
{
	switch (NewPhase)
	{
		case EMiniGamePhase::Waiting: HandleWaiting(); break;
		case EMiniGamePhase::Playing: HandlePlaying(); break;
		case EMiniGamePhase::Finished: HandleFinished(); break;
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

void AMiniGamePlayerController::HandleWaiting()
{
	SetGameInputEnabled(false);
}

void AMiniGamePlayerController::HandlePlaying()
{
	SetGameInputEnabled(true);
}

void AMiniGamePlayerController::HandleFinished()
{
	SetGameInputEnabled(false);
}
