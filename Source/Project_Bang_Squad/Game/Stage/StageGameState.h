// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Net/UnrealNetwork.h"
#include "StageGameState.generated.h"

class ACheckpoint;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AStageGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//현재 스테이지 체크포인트 인덱스 (공유 체크포인트)
	UPROPERTY(Replicated)
	int32 CurrentStageCheckpointIndex = 0;
public:
	UPROPERTY()
	TMap<int32, ACheckpoint*> CheckpointMap;

	void RegisterCheckpoint(int32 Index, ACheckpoint* Checkpoint);

	int32 GetStageCheckpointIndex() const { return CurrentStageCheckpointIndex; }
	void SetStageCheckpointIndex(int32 NewIndex) { CurrentStageCheckpointIndex = NewIndex; }

	//체크포인트 갱신 함수
	void UpdateStageCheckpoint(int32 NewIndex);
	
};
