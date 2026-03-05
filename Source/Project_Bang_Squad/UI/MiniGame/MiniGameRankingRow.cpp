// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/MiniGame/MiniGameRankingRow.h"

#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "Project_Bang_Squad/Game/MiniGame/MiniGamePlayerState.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"

void UMiniGameRankingRow::UpdateData(int32 Rank, class AMiniGamePlayerState* PlayerState)
{
	if (!PlayerState) return;

	if (TargetPlayerState.IsValid())
	{
		if (ABSPlayerState* BSPS = Cast<ABSPlayerState>(TargetPlayerState.Get()))
		{
			BSPS->OnRespawnTimeChanged.RemoveDynamic(this, &UMiniGameRankingRow::UpdateDeathState);
		}
	}
	TargetPlayerState = PlayerState;
	if (Img_Rank)
	{
		int32 TexIndex = Rank - 1; 

		// 배열 범위 안에 있고, 텍스처가 할당되어 있는지 확인
		if (RankTextures.IsValidIndex(TexIndex) && RankTextures[TexIndex])
		{
			Img_Rank->SetBrushFromTexture(RankTextures[TexIndex]);
			Img_Rank->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Img_Rank->SetVisibility(ESlateVisibility::Hidden); // 예외 처리
		}
	}
	/*if (Txt_Rank)
	{
		FText RankText = FText();
		switch (Rank)
		{
		case 1: RankText = FText::FromString("1st"); break;
		case 2: RankText = FText::FromString("2nd"); break;
		case 3: RankText = FText::FromString("3rd"); break;
		case 4: RankText = FText::FromString("4th"); break;
		default: RankText = FText::FromString("??");
		}
		Txt_Rank->SetText(RankText);
	}*/
	if (Img_JobIcon)
	{
		EJobType CurrentJob = PlayerState->GetJob();

		// 맵에 해당 직업 아이콘이 등록되어 있는지 확인
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			if (UTexture2D* FoundTexture = GI->GetJobIcon(CurrentJob))
			{
				Img_JobIcon->SetBrushFromTexture(FoundTexture);
				Img_JobIcon->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}

	if (ABSPlayerState* BSPS = Cast<ABSPlayerState>(TargetPlayerState.Get()))
	{
		BSPS->OnRespawnTimeChanged.AddDynamic(this, &UMiniGameRankingRow::UpdateDeathState);
		UpdateDeathState(BSPS->GetRespawnEndTime());
	}
}

void UMiniGameRankingRow::UpdateDeathState(float NewRespawnTime)
{
	if (!GetWorld() || !GetWorld()->GetGameState()) return;

	float RemainTime = NewRespawnTime - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	if (RemainTime > 0.f) // 죽음 상태
	{
		HandleOnDead();
		
		if (!GetWorld()->GetTimerManager().IsTimerActive(RespawnTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &UMiniGameRankingRow::RefreshRespawnTimer, 0.1f, true);
		}
		RefreshRespawnTimer();
	}
	else // 부활
	{
		if (Overlay_Death)
			Overlay_Death->SetVisibility(ESlateVisibility::Hidden);
		if (Img_JobIcon)
			Img_JobIcon->SetColorAndOpacity(FLinearColor::White);
		
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
	}
}

void UMiniGameRankingRow::RefreshRespawnTimer()
{
	if (!TargetPlayerState.IsValid()) return;
	if (!GetWorld() || !GetWorld()->GetGameState()) return;

	if (ABSPlayerState* BSPS = Cast<ABSPlayerState>(TargetPlayerState.Get()))
	{
		float RemainTime = BSPS->GetRespawnEndTime() - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
		
		if (RemainTime <= 0.f)
		{
			UpdateDeathState(0.f);
			return;
		}

		if (Txt_RespawnTime)
		{
			Txt_RespawnTime->SetText(FText::AsNumber(FMath::CeilToInt(RemainTime)));
		}
	}
}

void UMiniGameRankingRow::HandleOnDead()
{
	if (Overlay_Death)
		Overlay_Death->SetVisibility(ESlateVisibility::Visible);
	if (Img_JobIcon)
		Img_JobIcon->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.f));
}
