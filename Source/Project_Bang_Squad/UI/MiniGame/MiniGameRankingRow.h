// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameRankingRow.generated.h"

enum class EJobType : uint8;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UMiniGameRankingRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Rank;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_JobIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BS|UI")
	TMap<EJobType, UTexture2D*> JobIconMap;

	//데이터 채우는 함수
	void UpdateData(int32 Rank, class AStagePlayerState* PlayerState);
};
