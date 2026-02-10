// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Base/BSPlayerState.h"

#include "Net/UnrealNetwork.h"

void ABSPlayerState::OnRep_JobType()
{
	//TODO: UI 업데이트 로직
}

void ABSPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
}

void ABSPlayerState::SetJob(EJobType NewJob)
{
	if (HasAuthority())
	{
		JobType = NewJob;
		OnRep_JobType();
	}
}

void ABSPlayerState::SetRespawnEndTime(float NewTime)
{
	RespawnEndTime = NewTime;
}

void ABSPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//다음 Level의 PlayerState로 직업 정보 복사
	ABSPlayerState* PS = Cast<ABSPlayerState>(PlayerState);
	if (PS)
	{
		PS->JobType = JobType;
		PS->RespawnEndTime = RespawnEndTime;
	}
}

void ABSPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSPlayerState, JobType);
	DOREPLIFETIME(ABSPlayerState, RespawnEndTime);
}

void ABSPlayerState::Server_SetJob_Implementation(EJobType NewJob)
{
	JobType = NewJob;
}
