// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API ABSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	//관리할 위젯 목록
	UPROPERTY()
	TArray<UUserWidget*> ManagedWidgets;

	//Seamless Travel 시작 시 호출
	virtual void SeamlessTravelFrom(class APlayerController* OldPC) override;
public:
	void RegisterManagedWidget(UUserWidget* InWidget);
};
