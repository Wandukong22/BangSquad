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
	DOREPLIFETIME(ALobbyGameState, CurrentLobbyPhase);
	DOREPLIFETIME(ALobbyGameState, SkipVoteCount);
}

void ALobbyGameState::SetCurrentLobbyPhase(ELobbyPhase NewPhase)
{
	if (HasAuthority())
	{
		CurrentLobbyPhase = NewPhase;
		NotifyLobbyPhaseChanged();
	}
}

void ALobbyGameState::OnRep_CurrentLobbyPhase()
{
	//모든 클라이언트(UI)에게 알림
	NotifyLobbyPhaseChanged();
}

void ALobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	OnLobbyRosterChanged.Broadcast();
}

void ALobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	OnLobbyRosterChanged.Broadcast();
}

void ALobbyGameState::NotifyLobbyPhaseChanged()
{
	OnLobbyPhaseChanged.Broadcast(CurrentLobbyPhase);
}

void ALobbyGameState::OnRep_SkipVoteCount()
{
	// UI에 현재 투표수와 전체 플레이어 수를 전달
	OnSkipVoteChanged.Broadcast(SkipVoteCount, PlayerArray.Num());
}
