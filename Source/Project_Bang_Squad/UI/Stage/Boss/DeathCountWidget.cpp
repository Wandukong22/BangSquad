// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathCountWidget.h"

#include "Components/TextBlock.h"

void UDeathCountWidget::UpdateDeathCount(int32 CurrentLives)
{
	if (Txt_DeathCount)
	{
		FString CountString = FString::Printf(TEXT("%d"), CurrentLives);
		Txt_DeathCount->SetText(FText::FromString(CountString));
	}
}
