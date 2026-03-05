// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "MiniGameResultRow.generated.h"

class ABSPlayerState;
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
	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* Txt_Rank;
	UPROPERTY(meta = (BindWidget))
	UImage* Img_JobIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Nickname;
	UPROPERTY(meta = (BindWidget))
	UImage* Img_CoinIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_CoinReward;

	//순위 아이콘들
	UPROPERTY(meta = (BindWidget))
	UImage* Img_Rank;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_RowIcon;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TMap<EStageIndex, UTexture2D*> RowTextures;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TArray<UTexture2D*> RankTextures;
	
	void UpdateResultData(int32 Rank, ABSPlayerState* PlayerState, int32 CoinReward, EStageIndex StageIndex);
};
