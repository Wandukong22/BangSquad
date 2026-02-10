// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PortalSlot.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UPortalSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetActive(bool bIsActive);

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_User_Icon;
};
