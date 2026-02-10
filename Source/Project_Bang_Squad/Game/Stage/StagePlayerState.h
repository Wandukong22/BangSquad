// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Game/Base/BSPlayerState.h"
#include "StagePlayerState.generated.h"

UCLASS()
class PROJECT_BANG_SQUAD_API AStagePlayerState : public ABSPlayerState
{
	GENERATED_BODY()

public:
	//데스카운트
	void IncreaseDeathCount() { DeathCount++; }
	int32 GetDeathCount() const { return DeathCount; }

	// 부활 타이머
	//UPROPERTY(Replicated/*Using = OnRep_RespawnEndTime*/)
	//float RespawnEndTime = 0.f;

	//float GetRespawnEndTime() { return RespawnEndTime; }
	//void SetRespawnEndTime(float NewTime);

	//UFUNCTION()
	//void OnRep_RespawnEndTime();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(APlayerState* PlayerState) override;

	UPROPERTY(Replicated)
	int32 DeathCount = 0;

private:
	float MaxHeightReached = 0.f;
};
