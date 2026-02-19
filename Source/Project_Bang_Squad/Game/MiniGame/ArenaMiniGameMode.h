// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	FTimerHandle ArenaTimerHandle;
	void UpdateArenaTimer();
	
	int32 AlivePlayerCount = 0;
};
