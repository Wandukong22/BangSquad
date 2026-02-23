// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArenaMainWidget.generated.h"

class UMiniGameResultRow;
class UVerticalBox;
class AArenaPlayerState;
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

	void ShowRankingBoard(const TArray<AArenaPlayerState*>& Players, const TArray<int32>& Ranks, const TArray<int32>& CoinRewards);

protected:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* RankingContainer;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UMiniGameResultRow> RankingRowClass;

	//UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	
};
