// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Base/BSPlayerState.h"
#include "MiniGamePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AMiniGamePlayerState : public ABSPlayerState
{
	GENERATED_BODY()

public:
	AMiniGamePlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(APlayerState* PlayerState) override;

	//순위
	UPROPERTY(Replicated)
	int32 MiniGameRank = 0;
	void SetMiniGameRank(int32 NewRank);

	//체크포인트
	UPROPERTY(Replicated)
	int32 MiniGameCheckpointIndex = 0;

	void UpdateMiniGameCheckpoint(int32 NewIndex);
	int32 GetMiniGameCheckpoint() { return MiniGameCheckpointIndex; }

	//점수 계산
	UFUNCTION()
	float GetMiniGameProgressScore() const;
};
