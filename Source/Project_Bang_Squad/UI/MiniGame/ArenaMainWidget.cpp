// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaMainWidget.h"

#include "CountdownWidget.h"
#include "Components/TextBlock.h"

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
