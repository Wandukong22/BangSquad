// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/MiniGame/MiniGameWidget.h"

#include "CountdownWidget.h"
#include "MiniGameRankingRow.h"
#include "MiniGameResultRow.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"
#include "Project_Bang_Squad/Data/DataAsset/BSMapData.h"
#include "Project_Bang_Squad/Game/MiniGame/MiniGamePlayerState.h"
#include "Project_Bang_Squad/Game/MiniGame/MiniGameState.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"

void UMiniGameWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateRanking();
}

void UMiniGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Img_ResultTitle)
	{
		Img_ResultTitle->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMiniGameWidget::ShowResultBoard(EStageIndex Stage, const TArray<AMiniGamePlayerState*>& Players, const TArray<int32>& Ranks,
                                      const TArray<int32>& Rewards)
{
	if (!ResultContainer || !ResultRowClass) return;
	if (Players.Num() != Ranks.Num() || Players.Num() != Rewards.Num()) return;

	// 배경 이미지 세팅
	if (Img_ResultTitle)
	{
		Img_ResultTitle->SetVisibility(ESlateVisibility::Visible);
		
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			// (주의: 프로젝트의 GameInstance 또는 GameState 구현에 맞게 현재 스테이지 정보를 가져오세요)
			EStageIndex CurrentStage = GI->GetCurrentStage(); 
			UBSMapData* MapData = GI->GetMapData(); 
			if (MapData)
			{
				// 추가하신 헬퍼 함수를 통해 텍스처를 바로 받아옵니다.
			if (UTexture2D* ResultTex = MapData->GetMiniGameResultImage(Stage, EStageSection::MiniGame))
				{
					Img_ResultTitle->SetBrushFromTexture(ResultTex);
				}
			}
		}
	}
	
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
