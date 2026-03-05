// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameResultRow.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Game/MiniGame/ArenaPlayerState.h"

void UMiniGameResultRow::UpdateResultData(int32 Rank, ABSPlayerState* PlayerState, int32 CoinReward, EStageIndex StageIndex)
{
	if (!PlayerState) return;

	/*if (Txt_Rank)
	{
		Txt_Rank->SetText(FText::AsNumber(Rank));
	}*/

	if (Txt_Nickname)
	{
		FString Nickname = PlayerState->GetPlayerName();
		int32 MaxLength = 8;

		if (Nickname.Len() > MaxLength)
		{
			Nickname = Nickname.Left(MaxLength) + TEXT("...");
		}
		
		//Txt_Nickname->SetText(FText::FromString(PlayerState->GetPlayerName()));
		Txt_Nickname->SetText(FText::FromString(Nickname));
	}

	if (Txt_CoinReward)
	{
		FString CoinText = FString::Printf(TEXT("+%d"), CoinReward);
		Txt_CoinReward->SetText(FText::FromString(CoinText));
	}

	if (Img_JobIcon)
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			if (UTexture2D* FoundTexture = GI->GetJobIcon(PlayerState->GetJob()))
			{
				//Img_JobIcon->SetBrushFromTexture(FoundTexture);
				if (UMaterialInstanceDynamic* DynamicMat = Img_JobIcon->GetDynamicMaterial())
				{
					// "TextureParameterName" 부분은 실제 머티리얼의 텍스처 파라미터 이름으로 변경해주세요.
					DynamicMat->SetTextureParameterValue(FName("ProfileImage"), FoundTexture);
				}
			}
		}
	}

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

	if (RowTextures.Contains(StageIndex))
	{
		if (UTexture2D* FoundTex = RowTextures[StageIndex])
		{
			Img_RowIcon->SetBrushFromTexture(FoundTex);
			Img_RowIcon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Img_RowIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
