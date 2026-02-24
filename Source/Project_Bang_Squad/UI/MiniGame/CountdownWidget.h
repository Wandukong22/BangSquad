// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CountdownWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountdownText;

	void UpdateCountdown(int32 Time);

protected:
	FTimerHandle HideTextTimerHandle;
	
	UFUNCTION()
	void HideGameStartText();
};
