// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameWidget.generated.h"

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

public:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* RankingListContainer;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UMiniGameRankingRow> RankingRowClass;

	void UpdateRanking();
};
