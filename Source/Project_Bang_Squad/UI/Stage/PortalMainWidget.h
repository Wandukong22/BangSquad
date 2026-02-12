// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PortalMainWidget.generated.h"

class UPortalSlot;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UPortalMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//최대 인원수만큼 슬롯 갱신
	UFUNCTION()
	void InitializePortal(int32 MaxPlayers);
	
	//현재 인원수에 따라 슬롯 색 변경 및 타이머 시작 체크
	//UFUNCTION()
	//void UpdatePlayerCount(int32 CurrentCount, int32 MaxPlayers);

	UFUNCTION()
	void UpdatePortalState(int32 CurrentCount, int32 MaxPlayers, int32 RemainingTime);

protected:
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HBox_Slots;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Countdown;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UPortalSlot> SlotWidgetClass;

private:
	UPROPERTY()
	TArray<UPortalSlot*> SlotWidgets;

	FTimerHandle CountdownTimerHandle;
	int32 CountdownTime = 5;

	void OnCountdownTick();
	void FinishCountdown();
};
