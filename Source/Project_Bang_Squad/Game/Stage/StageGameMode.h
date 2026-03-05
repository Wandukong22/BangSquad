// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "Project_Bang_Squad/Game/Base/BSGameMode.h"
#include "StageGameMode.generated.h"

class URespawnWidget;
enum class EJobType : uint8;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AStageGameMode : public ABSGameMode
{
	GENERATED_BODY()

public:
	AStageGameMode();

	// 보스 사망 시 호출 (내부에서 스테이지 체크)
	UFUNCTION(BlueprintCallable, Category = "BS|Ending")
	void OnBossDefeated();
	
protected:
	virtual FTransform GetRespawnTransform(AController* Controller) override;
	virtual float GetRespawnDelay(AController* Controller) override;
	//virtual void RequestRespawn(AController* Controller) override;

	void StartEndingVideo();
	void ReturnToMainMenu();

	FTimerHandle EndingTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "BS|Ending")
	float EndingVideoDuration = 28.0f;
};
