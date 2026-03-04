// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameWidget.generated.h"

class UImage;
class AMiniGamePlayerState;
class UMiniGameResultRow;
class UCountdownWidget;
class UMiniGameRankingRow;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UMiniGameWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* RankingListContainer;

	UPROPERTY(meta = (BindWidget))
	UCountdownWidget* CountdownWidget;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UMiniGameRankingRow> RankingRowClass;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ResultContainer;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UMiniGameResultRow> ResultRowClass;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_ResultTitle;
	
	void ShowResultBoard(const TArray<AMiniGamePlayerState*>& Players, const TArray<int32>& Ranks, const TArray<int32>& Rewards);

	void UpdateRanking();
};
