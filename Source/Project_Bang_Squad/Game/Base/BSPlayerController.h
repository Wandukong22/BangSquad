// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
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

#pragma region Debug Codes
	virtual void SetupInputComponent() override;

	
	UFUNCTION(Server, Reliable)
	void ServerDebugMoveToMapHandle(EStageIndex Stage, EStageSection Section);

	void MoveToStage1Map();
	void MoveToStage1MiniGameMap();
	void MoveToStage1BossMap();
	void MoveToStage2Map();
	void MoveToStage2MiniGameMap();
	void MoveToStage2BossMap();
	void MoveToStage3Map();
	void MoveToStage3MiniGameMap();
	void MoveToStage3BossMap();
#pragma endregion

};
