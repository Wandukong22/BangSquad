// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameResultRow.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Game/MiniGame/ArenaPlayerState.h"

void UMiniGameResultRow::UpdateResultData(int32 Rank, AArenaPlayerState* PlayerState, int32 CoinReward)
{
	if (!PlayerState) return;

	if (Txt_Rank)
	{
		Txt_Rank->SetText(FText::AsNumber(Rank));
	}

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
				Img_JobIcon->SetBrushFromTexture(FoundTexture);
			}
		}
	}
}
