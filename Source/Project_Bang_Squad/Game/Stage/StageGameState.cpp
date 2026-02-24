// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Stage/StageGameState.h"

#include "Net/UnrealNetwork.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"

void AStageGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStageGameState, CurrentStageCheckpointIndex);
}

void AStageGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			if (GI->GetSavedCheckpointIndex() > 0)
			{
				CurrentStageCheckpointIndex = GI->GetSavedCheckpointIndex();
			}
		}
	}
}

//void AStageGameState::RegisterCheckpoint(int32 Index, ACheckpoint* Checkpoint)
//{
//	CheckpointMap.Add(Index, Checkpoint);
//}

void AStageGameState::UpdateStageCheckpoint(int32 NewIndex)
{
	if (NewIndex > CurrentStageCheckpointIndex)
	{
		CurrentStageCheckpointIndex = NewIndex;

		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			GI->SetSavedCheckpointIndex(NewIndex);
		}
	}
}