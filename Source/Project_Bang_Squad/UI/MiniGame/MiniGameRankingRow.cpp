// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/MiniGame/MiniGameRankingRow.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Project_Bang_Squad/Game/MiniGame/MiniGamePlayerState.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"

void UMiniGameRankingRow::UpdateData(int32 Rank, class AMiniGamePlayerState* PlayerState)
{
	if (!PlayerState) return;

	if (Txt_Rank)
	{
		Txt_Rank->SetText(FText::AsNumber(Rank));
	}
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
}
