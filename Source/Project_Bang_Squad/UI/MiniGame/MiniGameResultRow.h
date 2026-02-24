// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameResultRow.generated.h"

class AArenaPlayerState;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UMiniGameResultRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Rank;
	UPROPERTY(meta = (BindWidget))
	UImage* Img_JobIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Nickname;
	UPROPERTY(meta = (BindWidget))
	UImage* Img_CoinIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_CoinReward;

	void UpdateResultData(int32 Rank, AArenaPlayerState* PlayerState, int32 CoinReward);
};
