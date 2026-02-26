// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnCountWidget.h"

#include "Components/TextBlock.h"

void URespawnCountWidget::UpdateRespawnCount(int32 CurrentLives)
{
	if (Txt_RespawnCount)
	{
		FString CountString = FString::Printf(TEXT("%d"), CurrentLives);
		Txt_RespawnCount->SetText(FText::FromString(CountString));
	}
}
