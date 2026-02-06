// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Character/StageBoss/StageBossPlayerState.h"

#include "Net/UnrealNetwork.h"

void AStageBossPlayerState::AddQTECount()
{
	PersonalQTECount++;

	if (HasAuthority())
	{
		OnPersonalQTEChanged.Broadcast(PersonalQTECount);
	}
}

void AStageBossPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStageBossPlayerState, PersonalQTECount);
}

void AStageBossPlayerState::OnRep_PersonalQTECount()
{
	OnPersonalQTEChanged.Broadcast(PersonalQTECount);
}
