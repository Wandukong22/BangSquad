// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerController.h"
#include "MiniGamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AMiniGamePlayerController : public AStagePlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void OnPhaseChanged(EMiniGamePhase NewPhase);

	UFUNCTION(Client, Reliable)
	void Client_UpdateCountdown(int32 Count);

protected:
	virtual void BeginPlay() override;
	virtual void AcknowledgePossession(class APawn* P) override;

private:
	void HandleWaiting();
	void HandlePlaying();
	void HandleFinished();
};
