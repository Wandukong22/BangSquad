// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Lobby/LobbyGameState.h"
#include "Net/UnrealNetwork.h"

ALobbyGameState::ALobbyGameState()
{
	bReplicates = true;
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyGameState, CurrentPhase);
	DOREPLIFETIME(ALobbyGameState, TakenJobs);
	DOREPLIFETIME(ALobbyGameState, SkipVoteCount);
}

void ALobbyGameState::SetLobbyPhase(ELobbyPhase NewPhase)
{
	if (HasAuthority())
	{
		CurrentPhase = NewPhase;
		OnRep_CurrentPhase(); //서버는 RepNotify 자동호출X라서 수동으로 함
	}
}

bool ALobbyGameState::IsJobAvailable(EJobType JobType) const
{
	return !TakenJobs.Contains(JobType);
}

void ALobbyGameState::AddTakenJob(EJobType JobType)
{
	if (HasAuthority() && IsJobAvailable(JobType))
	{
		TakenJobs.Add(JobType);
		OnRep_TakenJobs(); //서버에서도 UI 갱신
	}
}

void ALobbyGameState::RemoveTakenJob(EJobType JobType)
{
	if (HasAuthority())
	{
		if (TakenJobs.Contains(JobType))
		{
			TakenJobs.Remove(JobType);
			OnRep_TakenJobs();
		}
	}
}

void ALobbyGameState::OnRep_TakenJobs()
{
	OnTakenJobsChanged.Broadcast(TakenJobs);
}

void ALobbyGameState::OnRep_CurrentPhase()
{
	//모든 클라이언트(UI)에게 알림
	OnLobbyPhaseChanged.Broadcast(CurrentPhase);
}

void ALobbyGameState::OnRep_SkipVoteCount()
{
	// UI에 현재 투표수와 전체 플레이어 수를 전달
	OnSkipVoteChanged.Broadcast(SkipVoteCount, PlayerArray.Num());
}