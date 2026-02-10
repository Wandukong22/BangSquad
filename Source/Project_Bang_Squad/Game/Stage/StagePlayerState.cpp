// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"

#include "Checkpoint.h"
#include "StageGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AStagePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStagePlayerState, DeathCount);
}

void AStagePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	AStagePlayerState* PS = Cast<AStagePlayerState>(PlayerState);
	if (PS)
	{
		PS->DeathCount = DeathCount;
	}
}

//void AStagePlayerState::OnRep_RespawnEndTime()
//{
//	OnRespawnTimeChanged.Broadcast(RespawnEndTime);
//}
