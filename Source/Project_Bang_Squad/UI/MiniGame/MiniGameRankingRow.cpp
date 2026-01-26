// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/MiniGame/MiniGameRankingRow.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"

void UMiniGameRankingRow::UpdateData(int32 Rank, class AStagePlayerState* PlayerState)
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
		if (JobIconMap.Contains(CurrentJob))
		{
			// 찾았으면 설정
			Img_JobIcon->SetBrushFromTexture(JobIconMap[CurrentJob]);
			Img_JobIcon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			// 없으면 숨기거나 기본 이미지
			// Img_JobIcon->SetVisibility(ESlateVisibility::Hidden); 
		}
	}
}
