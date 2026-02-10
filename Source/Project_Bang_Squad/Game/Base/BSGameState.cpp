// Fill out your copyright notice in the Description page of Project Settings.


#include "BSGameState.h"

#include "BSPlayerState.h"

TArray<ABSPlayerState*> ABSGameState::GetAllBSPlayerStates() const
{
	TArray<ABSPlayerState*> Result;
	for (APlayerState* PS : PlayerArray)
	{
		if (ABSPlayerState* BSPS = Cast<ABSPlayerState>(PS))
		{
			Result.Add(BSPS);
		}
	}
	return Result;
}

int32 ABSGameState::GetAlivePlayerCount() const
{
	int32 Count = 0;
	for (APlayerState* PS : PlayerArray)
	{
		if (ABSPlayerState* BSPS = Cast<ABSPlayerState>(PS))
		{
			if (APawn* Pawn = BSPS->GetPawn()) Count++;
		}
	}
	return Count;
}

ABSPlayerState* ABSGameState::FindPlayerByJob(EJobType Job) const
{
	for (APlayerState* PS : PlayerArray)
	{
		if (ABSPlayerState* BSPS = Cast<ABSPlayerState>(PS))
		{
			if (BSPS->GetJob() == Job)
			{
				return BSPS;
			}
		}
	}
	return nullptr;
}
