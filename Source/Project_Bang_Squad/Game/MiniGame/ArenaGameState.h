// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Base/BSGameState.h"
#include "ArenaGameState.generated.h"

UENUM(BlueprintType)
enum class EArenaPattern : uint8
{
	Waiting,
	Surviving,
	FloorSinking,
	Finished,
};

UCLASS()
class PROJECT_BANG_SQUAD_API AArenaGameState : public ABSGameState
{
	GENERATED_BODY()
public:
	void SetCurrentPhase(EArenaPattern InPhase) { CurrentPhase = InPhase; }
	EArenaPattern GetCurrentPhase() const { return CurrentPhase; }
	
	void SetRemainingTime(int32 InRemainingTime) { RemainingTime = InRemainingTime; }
	int32 GetRemainingTime() const { return RemainingTime; }

	void SetCurrentSinkingFloor(int32 InFloor) { CurrentSinkingFloor = InFloor; }
	int32 GetCurrentSinkingFloor() const { return CurrentSinkingFloor; }
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentPhase(EArenaPattern OldPhase);
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase)
	EArenaPattern CurrentPhase = EArenaPattern::Waiting;

	UPROPERTY(Replicated)
	int32 RemainingTime = 5;

	//현재 가라앉을 바닥 번호
	UPROPERTY(Replicated)
	int32 CurrentSinkingFloor = 1;
};
