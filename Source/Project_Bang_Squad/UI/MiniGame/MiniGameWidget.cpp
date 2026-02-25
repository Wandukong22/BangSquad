// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/MiniGame/MiniGameWidget.h"

#include "CountdownWidget.h"
#include "MiniGameRankingRow.h"
#include "MiniGameResultRow.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"
#include "Project_Bang_Squad/Game/MiniGame/MiniGamePlayerState.h"
#include "Project_Bang_Squad/Game/MiniGame/MiniGameState.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"

void UMiniGameWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateRanking();
}

void UMiniGameWidget::ShowResultBoard(const TArray<AMiniGamePlayerState*>& Players, const TArray<int32>& Ranks,
	const TArray<int32>& Rewards)
{
	if (!ResultContainer || !ResultRowClass) return;
	if (Players.Num() != Ranks.Num() || Players.Num() != Rewards.Num()) return;
	
	ResultContainer->ClearChildren();
	ResultContainer->SetVisibility(ESlateVisibility::Visible);

	for (int32 i = 0; i < Players.Num(); ++i)
	{
        if (UMiniGameResultRow* Row = CreateWidget<UMiniGameResultRow>(GetWorld(), ResultRowClass))
        {
        	Row->UpdateResultData(Ranks[i], Players[i], Rewards[i]);
        	ResultContainer->AddChildToVerticalBox(Row);
        }
	}
}

void UMiniGameWidget::UpdateRanking()
{
	if (!RankingListContainer || !RankingRowClass) return;

	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
	if (!GS) return;

	TArray<AMiniGamePlayerState*> SortedPlayers;
	for (APlayerState* PS : GS->PlayerArray)
	{
		if (AMiniGamePlayerState* MiniPS = Cast<AMiniGamePlayerState>(PS))
		{
			SortedPlayers.Add(MiniPS);
		}
	}

	SortedPlayers.Sort([](const AMiniGamePlayerState& A, const AMiniGamePlayerState& B)
	{
		return A.GetMiniGameProgressScore() > B.GetMiniGameProgressScore();
	});

	int32 PlayerCount = SortedPlayers.Num();

	while (RankingListContainer->GetChildrenCount() < PlayerCount)
	{
		UMiniGameRankingRow* NewRow = CreateWidget<UMiniGameRankingRow>(this, RankingRowClass);
		if (NewRow)
		{
			RankingListContainer->AddChild(NewRow);
		}
	}

	for (int32 i = 0; i < RankingListContainer->GetChildrenCount(); ++i)
	{
		UMiniGameRankingRow* Row = Cast<UMiniGameRankingRow>(RankingListContainer->GetChildAt(i));
		if (!Row) continue;

		if (i < PlayerCount)
		{
			Row->SetVisibility(ESlateVisibility::Visible);
			// i는 0부터 시작하므로 순위는 i + 1
			Row->UpdateData(i + 1, SortedPlayers[i]);
		}
		else
		{
			// 플레이어가 나갔거나 해서 남는 위젯은 숨김
			Row->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
