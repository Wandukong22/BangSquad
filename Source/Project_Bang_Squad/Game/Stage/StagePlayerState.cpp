// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"

#include "Net/UnrealNetwork.h"

void AStagePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStagePlayerState, Job);
	DOREPLIFETIME(AStagePlayerState, RespawnEndTime);
	DOREPLIFETIME(AStagePlayerState, MiniGameCheckpointIndex);
	DOREPLIFETIME(AStagePlayerState, StageCheckpointIndex);
	DOREPLIFETIME(AStagePlayerState, DeathCount);
}

void AStagePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	AStagePlayerState* PS = Cast<AStagePlayerState>(PlayerState);
	if (PS)
	{
		PS->Job = Job;
		PS->RespawnEndTime = RespawnEndTime;
		PS->StageCheckpointIndex = StageCheckpointIndex;
		PS->DeathCount = DeathCount;
		
		//TODO: 체력 넣을거면 여기다가
	}
}

void AStagePlayerState::SetRespawnEndTime(float NewTime)
{
	RespawnEndTime = NewTime;
}

void AStagePlayerState::SetJob(EJobType NewJob)
{
	Job = NewJob;
}

void AStagePlayerState::UpdateMiniGameCheckpoint(int32 NewIndex)
{
	if (NewIndex > MiniGameCheckpointIndex)
	{
		MiniGameCheckpointIndex = NewIndex;

		//TODO: 체크포인트 저장 UI는 여기서
	}
}

void AStagePlayerState::UpdateStageCheckpoint(int32 NewIndex)
{
	if (NewIndex > StageCheckpointIndex)
	{
		StageCheckpointIndex = NewIndex;
		
		//TODO: 체크포인트 저장 UI는 여기서
	}
}
