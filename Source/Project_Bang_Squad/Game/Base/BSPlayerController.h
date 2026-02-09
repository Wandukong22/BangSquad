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

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(Server, Reliable)
	void ServerSetNickName(const FString& NewName);
public:
	void RegisterManagedWidget(UUserWidget* InWidget);

#pragma region Development Codes
	virtual void SetupInputComponent() override;

	UFUNCTION(Server, Reliable)
	void ServerDebugMoveToStage1Boss();
#pragma endregion

};
