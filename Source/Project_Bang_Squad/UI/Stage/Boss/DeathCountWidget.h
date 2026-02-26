// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathCountWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UDeathCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateDeathCount(int32 CurrentLives);
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Skeleton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_DeathCount;
	
};
