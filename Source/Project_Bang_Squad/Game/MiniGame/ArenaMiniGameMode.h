// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArenaGameState.h"
#include "Project_Bang_Squad/Game/Base/BSGameMode.h"
#include "ArenaMiniGameMode.generated.h"

UCLASS()
class PROJECT_BANG_SQUAD_API AArenaMiniGameMode : public ABSGameMode
{
	GENERATED_BODY()
	
public:
	AArenaMiniGameMode();
	
	//캐릭터 죽을 때 호출
	void OnPlayerDied(AController* VictimController);
protected:
	virtual void BeginPlay() override;

private:
	void TickWaitingCountdown();
	void TickArenaTimer();
	void EndArena();
	void BroadcastPhaseChanged(EArenaPattern NewPhase);

	int32 AlivePlayerCount = 0;
	FTimerHandle WaitingTimerHandle;
	FTimerHandle ArenaTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "BS|Arena")
	int32 WaitingDuration = 5;
	UPROPERTY(EditDefaultsOnly, Category = "BS|Arena")
	int32 SurvivingDuration = 20;
	UPROPERTY(EditDefaultsOnly, Category = "BS|Arena")
	int32 FloorSinkingDuration = 5;
	//가라앉는 바닥 개수
	UPROPERTY(EditDefaultsOnly, Category = "BS|Arena")
	int32 MaxSinkingFloors = 2;
	
	//Guard flag
	bool bArenaEnded = false;
};
