// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaMainWidget.h"

#include "CountdownWidget.h"
#include "MiniGameResultRow.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Data/DataAsset/BSMapData.h"
#include "Project_Bang_Squad/Game/MiniGame/ArenaGameState.h"
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

void UArenaMainWidget::ShowFloorSinkingText()
{
	if (SurvivingTimerText)
	{
		SurvivingTimerText->SetText(FText::FromString(TEXT("전장 축소중")));
		SurvivingTimerText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UArenaMainWidget::ShowRankingBoard(EStageIndex Stage, const TArray<AArenaPlayerState*>& Players, const TArray<int32>& Ranks, const TArray<int32>& CoinRewards)
{
	if (!RankingContainer || !RankingRowClass) return;
	if (Players.Num() != Ranks.Num() || Players.Num() != CoinRewards.Num()) return;

	// 배경 이미지 세팅 추가
	if (Img_ResultTitle)
	{
		Img_ResultTitle->SetVisibility(ESlateVisibility::Visible);
		
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			if (UBSMapData* MapData = GI->GetMapData())
			{
				if (UTexture2D* ResultTex = MapData->GetMiniGameResultImage(Stage, EStageSection::MiniGame))
				{
					Img_ResultTitle->SetBrushFromTexture(ResultTex);
				}
			}
		}
	}
	
	RankingContainer->ClearChildren();
	RankingContainer->SetVisibility(ESlateVisibility::Visible);

	for (int32 i = 0; i < Players.Num(); i++)
	{
		if (UMiniGameResultRow* Row = CreateWidget<UMiniGameResultRow>(GetWorld(), RankingRowClass))
		{
			Row->UpdateResultData(Ranks[i], Players[i], CoinRewards[i], Stage);
			RankingContainer->AddChildToVerticalBox(Row);
		}
	}
}

void UArenaMainWidget::UpdatePartyList()
{
	if (!PlayerListContainer || !PlayerRowClass) return;

	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
	if (!GS) return;

	APlayerState* MyPS = GetOwningPlayerState();
	int32 CurrentChildIndex = 0;

	// 4. 플레이어 목록 순회하며 생성
	for (APlayerState* PS : GS->PlayerArray)
	{
		if (!PS) continue;
		if (PS == MyPS)
		{
			//내 정보 업데이트
			if (MyInfoRow)
			{
				MyInfoRow->SetTargetPlayerState(PS);
				MyInfoRow->SetWidgetMode(ERowMode::Stage);
				MyInfoRow->SetVisibility(ESlateVisibility::Visible);
			}
			continue;
		}
		// 팀원 위젯 처리
		UPlayerRow* Row = nullptr;
		
		if (CurrentChildIndex < PlayerListContainer->GetChildrenCount())
		{
			Row = Cast<UPlayerRow>(PlayerListContainer->GetChildAt(CurrentChildIndex));
		}
		if (!Row)
		{
			Row = CreateWidget<UPlayerRow>(this, PlayerRowClass);
			if (Row)
			{
				PlayerListContainer->AddChild(Row);
			}
		}

		if (Row)
		{
			//누구 정보인지 타겟 설정
			Row->SetTargetPlayerState(PS);
			CurrentChildIndex++;
		}
	}
}

void UArenaMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CachedPlayerCount = 0;
	if (Img_ResultTitle)
	{
		Img_ResultTitle->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UArenaMainWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>())
	{
		bool bNeedUpdate = false;

		//전체 플레이어 수 변경 감지
		if (GS->PlayerArray.Num() != CachedPlayerCount)
		{
			bNeedUpdate = true;
		}
		else
		{
			//1초마다 UI 개수 정합성 체크
			static float TimeAccumulator = 0.f;
			TimeAccumulator += DeltaTime;
			
			if (TimeAccumulator > 1.f)
			{
				TimeAccumulator = 0.f;
				bNeedUpdate = true;
			}
		}

		if (bNeedUpdate)
		{
			UpdatePartyList();
			CachedPlayerCount = GS->PlayerArray.Num();
		}
	}
}
