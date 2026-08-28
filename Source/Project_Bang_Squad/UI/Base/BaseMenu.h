// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseMenu.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UBaseMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowWidget();
	void HideWidget();

	void StartUp();
	void Shutdown();
};
