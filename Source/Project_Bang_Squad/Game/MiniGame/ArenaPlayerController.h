// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArenaGameState.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerController.h"
#include "ArenaPlayerController.generated.h"

UCLASS()
class PROJECT_BANG_SQUAD_API AArenaPlayerController : public AStagePlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void Client_OnArenaPhaseChanged(EArenaPattern NewPhase);

	UFUNCTION(Client, Reliable)
	void Client_UpdateWaitingCountdown(int32 Count);

	UFUNCTION(Client, Reliable)
	void Client_UpdateSurvivingTimer(int32 RemainingTime);

	virtual void StartSpectating() override;

	UFUNCTION(Client, Reliable)
	void Client_ShowArenaResult(const TArray<class AArenaPlayerState*>& Players, const TArray<int32>& Ranks, const TArray<int32>& Rewards);
protected:
	virtual void BeginPlay() override;
	virtual void AcknowledgePossession(class APawn* P) override;

private:
	void HandleWaiting();
	void HandleSurviving();
	void HandleFloorSinking();
	void HandleFinished();
};
