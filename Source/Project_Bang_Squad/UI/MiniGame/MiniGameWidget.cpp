// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/MiniGame/MiniGameWidget.h"

#include "MiniGameRankingRow.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"

void UMiniGameWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateRanking();
}

void UMiniGameWidget::UpdateRanking()
{
	if (!RankingListContainer || !RankingRowClass) return;

	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
	if (!GS) return;

	TArray<AStagePlayerState*> SortedPlayers;
	for (APlayerState* PS : GS->PlayerArray)
	{
		if (AStagePlayerState* StagePS = Cast<AStagePlayerState>(PS))
		{
			SortedPlayers.Add(StagePS);
		}
	}

	SortedPlayers.Sort([](const AStagePlayerState& A, const AStagePlayerState& B)
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
