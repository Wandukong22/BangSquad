// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerController.h"
#include "MiniGamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AMiniGamePlayerController : public AStagePlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<class UMiniGameWidget> MiniGameWidgetClass;

protected:
	virtual void CreateGameWidget() override;
};
