// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArenaMainWidget.generated.h"

class UTextBlock;
class UCountdownWidget;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UArenaMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UCountdownWidget* CountdownWidget;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SurvivingTimerText;

	void UpdateWaitingCountdown(int32 Count);
	void UpdateSurvivingTimer(int32 RemainingTime);
	void SetSurvivingTimerVisible(bool bVisible);
};
