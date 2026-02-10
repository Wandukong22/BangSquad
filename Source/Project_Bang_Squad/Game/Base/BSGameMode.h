// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "Project_Bang_Squad/Game/Interface/RespawnInterface.h"
#include "BSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API ABSGameMode : public AGameModeBase, public IRespawnInterface
{
	GENERATED_BODY()

public:
	ABSGameMode();

	virtual void RequestRespawn(AController* Controller) override;
	virtual void SpawnPlayerCharacter(AController* Controller, EJobType JobType);

protected:
	virtual FTransform GetRespawnTransform(AController* Controller);
	//virtual void RespawnPlayerElapsed(AController* DeadController);
	virtual void ExecuteRespawn(AController* Controller);

	UBSGameInstance* GetBSGameInstance() const;

	UPROPERTY(EditAnywhere, Category = "BS|Spawn")
	float BaseRespawnTime = 3.f;
	virtual float GetRespawnDelay(AController* Controller) const { return BaseRespawnTime; }
};
