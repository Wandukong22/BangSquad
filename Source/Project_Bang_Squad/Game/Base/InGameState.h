// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSGameState.h"
#include "InGameState.generated.h"

class ACheckpoint;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AInGameState : public ABSGameState
{
	GENERATED_BODY()

public:
	void RegisterCheckpoint(int32 Index, ACheckpoint* Checkpoint);

	UPROPERTY()
	TMap<int32, ACheckpoint*> CheckpointMap;
};
