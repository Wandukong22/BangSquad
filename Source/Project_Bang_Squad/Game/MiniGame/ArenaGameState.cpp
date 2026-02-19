// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaGameState.h"

#include "Net/UnrealNetwork.h"

void AArenaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaGameState, CurrentPhase);
	DOREPLIFETIME(AArenaGameState, RemainingTime);
	DOREPLIFETIME(AArenaGameState, CurrentSinkingFloor);
}
