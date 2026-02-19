// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Base/BSPlayerState.h"
#include "ArenaPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AArenaPlayerState : public ABSPlayerState
{
	GENERATED_BODY()
public:
	void SetIsAlive(bool InIsAlive) { bIsAlive = InIsAlive; }
	bool GetIsAlive() const { return bIsAlive; }

	void SetArenaRank(int32 InArenaRank) { ArenaRank = InArenaRank; }
	int32 GetArenaRank() const { return ArenaRank; }
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	bool bIsAlive = true;

	UPROPERTY(Replicated)
	int32 ArenaRank = 0;
};
