// Fill out your copyright notice in the Description page of Project Settings.


#include "CountdownWidget.h"

#include "Components/TextBlock.h"

void UCountdownWidget::UpdateCountdown(int32 Time)
{
	if (!CountdownText) return;

	if (Time > 0)
	{
		CountdownText->SetVisibility(ESlateVisibility::Visible);
		CountdownText->SetText(FText::AsNumber(Time));
	}
	else if (Time == 0)
	{
		CountdownText->SetText(FText::FromString(TEXT("Game Start")));

		GetWorld()->GetTimerManager().SetTimer(
			HideTextTimerHandle,
			this,
			&UCountdownWidget::HideGameStartText,
			2.f,
			false
		);
		//CountdownText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCountdownWidget::HideGameStartText()
{
	if (CountdownText)
	{
		CountdownText->SetVisibility(ESlateVisibility::Hidden);
	}
}
