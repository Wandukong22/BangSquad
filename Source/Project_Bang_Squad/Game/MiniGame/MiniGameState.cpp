// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameState.h"

#include "Net/UnrealNetwork.h"

void AMiniGameState::OnRep_CurrentPhase(EMiniGamePhase OldPhase)
{
	//TODO: 각 Phase에 맞는 UI 전환 호출
}

void AMiniGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMiniGameState, CurrentPhase);
	DOREPLIFETIME(AMiniGameState, Countdown);
}
