// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "StageGameMode.generated.h"

class URespawnWidget;
enum class EJobType : uint8;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AStageGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AStageGameMode();
	
	//실제 소환
	void SpawnPlayerCharacter(AController* Controller, EJobType MyJob);

	void RequestRespawn(AController* Controller);

	void ExecuteRespawn(AController* Controller);

	//스테이지 이동 함수
	UFUNCTION(BlueprintCallable, Category = "BS|Stage")
	void ClearStageAndMove(EStageIndex NextStage, EStageSection NextSection = EStageSection::Main);

	bool IsMiniGameMap() const;

protected:
	void RespawnPlayerElapsed(AController* DeadController);

	//부활 위치 계산
	FTransform GetRespawnTransform(AController* Controller);

	virtual void PostLogin(APlayerController* NewPlayer) override;
};
