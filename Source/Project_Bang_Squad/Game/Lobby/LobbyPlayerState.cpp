// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerState.h"

#include "LobbyPlayerController.h"
#include "Net/UnrealNetwork.h"

ALobbyPlayerState::ALobbyPlayerState()
{
	bReplicates = true; //복제 설정

	//업데이트 빈도 관련
	SetNetUpdateFrequency(100.0f); //1초에 약 100번까지 상태 변화 체크
	SetMinNetUpdateFrequency(60.0f); //최소 빈도
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerState, bIsReady);
	DOREPLIFETIME(ALobbyPlayerState, bIsConfirmedJob);
}

void ALobbyPlayerState::SetJob(EJobType NewJob)
{
	if (HasAuthority())
	{
		Super::SetJob(NewJob);
		OnRep_UpdateUI();
		ForceNetUpdate();
	}
}

void ALobbyPlayerState::SetIsReady(bool NewIsReady)
{
	if (HasAuthority())
	{
		bIsReady = NewIsReady;
		OnRep_UpdateUI();
	}
}

void ALobbyPlayerState::SetIsConfirmedJob(bool NewIsConfirmedJob)
{
	if (HasAuthority())
	{
		bIsConfirmedJob = NewIsConfirmedJob;
		OnRep_UpdateUI();

		//변경사항을 모든 클라이언트에게 즉시 전송
		ForceNetUpdate();
	}
}

void ALobbyPlayerState::OnRep_UpdateUI()
{
	OnLobbyDataChanged.Broadcast();
	RefreshUI();
}

void ALobbyPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	RefreshUI();
}

void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();
	RefreshUI();
}

void ALobbyPlayerState::Destroyed()
{
	Super::Destroyed();
	RefreshUI();
}

void ALobbyPlayerState::OnRep_JobType()
{
	Super::OnRep_JobType();

	OnRep_UpdateUI();
}

void ALobbyPlayerState::RefreshUI()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (ALobbyPlayerController* LobbyPC = Cast<ALobbyPlayerController>(PC))
		{
			LobbyPC->RefreshLobbyUI();
		}
	}
}
