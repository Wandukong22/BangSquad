// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Base/BSGameMode.h"
#include "MiniGameMode.generated.h"

enum class EJobType : uint8;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AMiniGameMode : public ABSGameMode
{
	GENERATED_BODY()

public:
	AMiniGameMode();

	//도착 처리
	void OnPlayerReachedGoal(AController* ReachedPlayer, EStageIndex StageIndex);

protected:
	virtual void BeginPlay() override;
	virtual FTransform GetRespawnTransform(AController* Controller) override;

private:
	//모두 도착했는지 체크
	void CheckAllPlayersFinished(EStageIndex StageIndex);

	void EndMiniGame(EStageIndex StageIndex);
	void TickCountdown();

	//도착한 플레이어 목록
	UPROPERTY()
	TArray<AController*> FinishedPlayers;

	FTimerHandle CountdownTimerHandle;
	FTimerHandle ReturnTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "BS|MiniGame")
	int32 WaitingDuration = 5;

	UPROPERTY(EditDefaultsOnly, Category = "BS|MiniGame")
	float ReturnDelay = 5.f;
};
