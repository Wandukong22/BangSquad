// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameState.h"

void AInGameState::RegisterCheckpoint(int32 Index, ACheckpoint* Checkpoint)
{
	if (Checkpoint)
	{
		CheckpointMap.Add(Index, Checkpoint);
	}
}
