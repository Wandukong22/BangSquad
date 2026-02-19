// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaPlayerState.h"

#include "Net/UnrealNetwork.h"

void AArenaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaPlayerState, bIsAlive);
	DOREPLIFETIME(AArenaPlayerState, ArenaRank);
}
