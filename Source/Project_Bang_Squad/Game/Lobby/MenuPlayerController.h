// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Base/BSPlayerController.h"
#include "MenuPlayerController.generated.h"

class UMainMenu;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AMenuPlayerController : public ABSPlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY()
	TObjectPtr<UMainMenu> MainMenuWidget = nullptr;
};
