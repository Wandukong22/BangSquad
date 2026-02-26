// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RespawnCountWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API URespawnCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateRespawnCount(int32 CurrentLives);
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Respawn;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_RespawnCount;
	
};
