// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaGameState.h"

#include "ArenaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AArenaGameState::SetCurrentPhase(EArenaPattern InPhase)
{
	CurrentPhase = InPhase;

	if (HasAuthority())
	{
		OnRep_CurrentPhase(CurrentPhase);
	}
}

void AArenaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaGameState, CurrentPhase);
	DOREPLIFETIME(AArenaGameState, RemainingTime);
	DOREPLIFETIME(AArenaGameState, CurrentSinkingFloor);
}

void AArenaGameState::OnRep_CurrentPhase(EArenaPattern OldPhase)
{
	if (AArenaPlayerController* PC = Cast<AArenaPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		PC->Local_OnArenaPhaseChanged(CurrentPhase);
	}
}
