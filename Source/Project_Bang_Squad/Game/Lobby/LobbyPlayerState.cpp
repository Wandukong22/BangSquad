// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerState.h"
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
	DOREPLIFETIME(ALobbyPlayerState, PreviewJob);
}

void ALobbyPlayerState::SetPreviewJob(EJobType NewJob)
{
	if (!HasAuthority() || PreviewJob == NewJob) return;

	//로그용
	const EJobType OldPreviewJob = PreviewJob;
	PreviewJob = NewJob;
	NotifyLobbyDataChanged();

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[PreviewJob][ServerCommit] Player=%s OldPreview=%d NewPreview=%d ConfirmedJob=%d"),
		*GetPlayerName(),
		static_cast<uint8>(OldPreviewJob),
		static_cast<uint8>(PreviewJob),
		static_cast<uint8>(GetJob())
	);
}

void ALobbyPlayerState::SetJob(EJobType NewJob)
{
	if (HasAuthority())
	{
		Super::SetJob(NewJob);
		ForceNetUpdate();
	}
}

void ALobbyPlayerState::SetIsReady(bool NewIsReady)
{
	if (HasAuthority())
	{
		bIsReady = NewIsReady;
		NotifyLobbyDataChanged();
	}
}

void ALobbyPlayerState::OnRep_LobbyData()
{
	NotifyLobbyDataChanged();
}

void ALobbyPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	NotifyLobbyDataChanged();
}

void ALobbyPlayerState::OnRep_JobType()
{
	Super::OnRep_JobType();
	NotifyLobbyDataChanged();
}

void ALobbyPlayerState::NotifyLobbyDataChanged()
{
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[PreviewJob][Replicated] Player=%s NetMode=%d PreviewJob=%d ConfirmedJob=%d"),
		*GetPlayerName(),
		GetWorld() ? static_cast<int32>(GetWorld()->GetNetMode()) : -1,
		static_cast<uint8>(PreviewJob),
		static_cast<uint8>(GetJob())
	);
	OnLobbyDataChanged.Broadcast();
}