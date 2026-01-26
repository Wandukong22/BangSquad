// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "StagePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AStagePlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	EJobType Job = EJobType::None;

	UPROPERTY(Replicated)
	float RespawnEndTime = 0.f;

	UPROPERTY(Replicated)
	int32 DeathCount = 0;

	virtual void CopyProperties(APlayerState* PlayerState) override;

	virtual void Tick(float DeltaSeconds) override;
public:
	AStagePlayerState();
	
	EJobType GetJob() { return Job; }
	float GetRespawnEndTime() { return RespawnEndTime; }
	void SetRespawnEndTime(float NewTime);
	void SetJob(EJobType NewJob);

	//MiniGame용
	UPROPERTY(Replicated)
	int32 MiniGameCheckpointIndex = 0;
	UPROPERTY(Replicated)
	int32 StageCheckpointIndex = 0;

	//체크포인트 갱신 함수
	void UpdateMiniGameCheckpoint(int32 NewIndex);
	void UpdateStageCheckpoint(int32 NewIndex);

	//현재 체크포인트 가져오기
	int32 GetMiniGameCheckpoint() { return MiniGameCheckpointIndex; }
	int32 GetStageCheckpoint() { return StageCheckpointIndex; }

	void SetStageCheckpoint(int32 NewIndex) { StageCheckpointIndex = NewIndex; }

	void IncreaseDeathCount() { DeathCount++; }
	int32 GetDeathCount() const { return DeathCount; }

	//순위 고정용 변수
	UPROPERTY(Replicated)
	int32 MiniGameRank = 0;

	//순위 설정
	void SetMiniGameRank(int32 NewRank);
	
	UFUNCTION()
	float GetMiniGameProgressScore() const;

private:
	float MaxHeightReached = 0.f;
};
