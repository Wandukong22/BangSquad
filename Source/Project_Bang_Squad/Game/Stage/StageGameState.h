// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "StageGameState.generated.h"

class ACheckpoint;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AStageGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<int32, ACheckpoint*> CheckpointMap;

	void RegisterCheckpoint(int32 Index, ACheckpoint* Checkpoint);
	
	
};
