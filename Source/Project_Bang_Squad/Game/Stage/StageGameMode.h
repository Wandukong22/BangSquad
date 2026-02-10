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
	
protected:
	virtual FTransform GetRespawnTransform(AController* Controller) override;
	virtual float GetRespawnDelay(AController* Controller) const override;
};
