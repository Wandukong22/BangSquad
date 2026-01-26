// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Stage/StageGameState.h"

void AStageGameState::RegisterCheckpoint(int32 Index, ACheckpoint* Checkpoint)
{
	CheckpointMap.Add(Index, Checkpoint);
}
