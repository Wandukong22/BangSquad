// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_Bang_Squad/UI/Stage/StageMainWidget.h"
#include "ArenaMainWidget.generated.h"

class UMiniGameResultRow;
class UVerticalBox;
class AArenaPlayerState;
class UTextBlock;
class UCountdownWidget;

UCLASS()
class PROJECT_BANG_SQUAD_API UArenaMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UCountdownWidget* CountdownWidget;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SurvivingTimerText;

	UPROPERTY(meta = (BindWidget))
	UPlayerRow* MyInfoRow;
	
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PlayerListContainer;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UPlayerRow> PlayerRowClass;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_ResultTitle;

	void UpdateWaitingCountdown(int32 Count);
	void UpdateSurvivingTimer(int32 RemainingTime);
	void SetSurvivingTimerVisible(bool bVisible);
	void ShowFloorSinkingText();

	void ShowRankingBoard(EStageIndex Stage, const TArray<AArenaPlayerState*>& Players, const TArray<int32>& Ranks, const TArray<int32>& CoinRewards);


	void UpdatePartyList();
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* RankingContainer;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UMiniGameResultRow> RankingRowClass;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

private:
	int32 CachedPlayerCount = 0;
	// 현재 연결된 캐릭터 캐싱 (중복 바인딩 방지)
	TWeakObjectPtr<ABaseCharacter> CachedCharacter;
};
