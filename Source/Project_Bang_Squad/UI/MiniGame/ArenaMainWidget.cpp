// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaMainWidget.h"

#include "CountdownWidget.h"
#include "MiniGameResultRow.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Project_Bang_Squad/Game/MiniGame/ArenaPlayerState.h"

void UArenaMainWidget::UpdateWaitingCountdown(int32 Count)
{
	if (CountdownWidget)
		CountdownWidget->UpdateCountdown(Count);
}

void UArenaMainWidget::UpdateSurvivingTimer(int32 RemainingTime)
{
	if (SurvivingTimerText)
	{
		FString Text = FString::Printf(TEXT("전장 축소까지 : %d초"), RemainingTime);
		SurvivingTimerText->SetText(FText::FromString(Text));
	}
}

void UArenaMainWidget::SetSurvivingTimerVisible(bool bVisible)
{
	if (SurvivingTimerText)
	{
		SurvivingTimerText->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UArenaMainWidget::ShowRankingBoard(const TArray<AArenaPlayerState*>& Players, const TArray<int32>& Ranks, const TArray<int32>& CoinRewards)
{
	if (!RankingContainer || !RankingRowClass) return;

	if (Players.Num() != Ranks.Num() || Players.Num() != CoinRewards.Num()) return;
	
	RankingContainer->ClearChildren();
	RankingContainer->SetVisibility(ESlateVisibility::Visible);

	for (int32 i = 0; i < Players.Num(); i++)
	{
		if (UMiniGameResultRow* Row = CreateWidget<UMiniGameResultRow>(GetWorld(), RankingRowClass))
		{
			Row->UpdateResultData(Ranks[i], Players[i], CoinRewards[i]);
			RankingContainer->AddChildToVerticalBox(Row);
		}
	}
	
	/*for (AArenaPlayerState* PS : SortedPlayers)
	{
		if (UMiniGameResultRow* Row = CreateWidget<UMiniGameResultRow>(GetWorld(), RankingRowClass))
		{
			int32 CurrentRank = PS->GetArenaRank();

			int32 RewardCoin = 0;
			switch (CurrentRank)
			{
			case 1: RewardCoin = 100; break;
			case 2: RewardCoin = 70;  break;
			case 3: RewardCoin = 40;  break;
			case 4: RewardCoin = 20;  break;
			}

			Row->UpdateResultData(CurrentRank, PS, RewardCoin);
			RankingContainer->AddChildToVerticalBox(Row);
		}
	}*/
}
