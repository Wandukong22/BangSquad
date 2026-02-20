// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Base/InGameState.h"
#include "MiniGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AMiniGameState : public AInGameState
{
	GENERATED_BODY()

public:
	void SetCurrentPhase(EMiniGamePhase InPhase) { CurrentPhase = InPhase; }
	EMiniGamePhase GetCurrentPhase() const { return CurrentPhase; }

	void SetCountdown(int32 InCountdown) { Countdown = InCountdown; }
	int32 GetCountdown() const { return Countdown; }
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase)
	EMiniGamePhase CurrentPhase = EMiniGamePhase::Waiting;

	UPROPERTY(Replicated)
	int32 Countdown = 5;

	UFUNCTION()
	void OnRep_CurrentPhase(EMiniGamePhase OldPhase);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
