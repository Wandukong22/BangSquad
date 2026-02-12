// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalMainWidget.h"

#include "PortalSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void UPortalMainWidget::InitializePortal(int32 MaxPlayers)
{
	if (!HBox_Slots || !SlotWidgetClass) return;

	HBox_Slots->ClearChildren();
	SlotWidgets.Empty();

	for (int32 i = 0; i < MaxPlayers; i++)
	{
		UPortalSlot* NewSlot = CreateWidget<UPortalSlot>(GetWorld(), SlotWidgetClass);
		if (NewSlot)
		{
			HBox_Slots->AddChildToHorizontalBox(NewSlot);
			NewSlot->SetActive(false);
			SlotWidgets.Add(NewSlot);
		}
	}

	if (Txt_Countdown)
	{
		Txt_Countdown->SetVisibility(ESlateVisibility::Hidden);
	}
}

/*

void UPortalMainWidget::UpdatePlayerCount(int32 CurrentCount, int32 MaxPlayers)
{
	//슬롯 상태 업데이트
	for (int32 i = 0; i < SlotWidgets.Num(); i++)
	{
		bool bIsActive = (i < CurrentCount);
		if (SlotWidgets[i])
		{
			SlotWidgets[i]->SetActive(bIsActive);
		}
	}

	//카운트다운
	if (CurrentCount >= MaxPlayers)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(CountdownTimerHandle))
		{
			CountdownTime = 5.f;
			if (Txt_Countdown)
			{
				Txt_Countdown->SetVisibility(ESlateVisibility::Visible);
				Txt_Countdown->SetText(FText::AsNumber(CountdownTime));
			}

			GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &UPortalMainWidget::OnCountdownTick, 1.f, true);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
		if (Txt_Countdown)
		{
			Txt_Countdown->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}*/

void UPortalMainWidget::UpdatePortalState(int32 CurrentCount, int32 MaxPlayers, int32 RemainingTime)
{
	//슬롯 개수 안맞으면 다시 생성
	if (SlotWidgets.Num() != MaxPlayers)
	{
		InitializePortal(MaxPlayers);
	}

	//활성화 상태 갱신
	for (int32 i = 0; i < SlotWidgets.Num(); i++)
	{
		if (SlotWidgets[i])
		{
			SlotWidgets[i]->SetActive(i < CurrentCount);
		}
	}

	//시간 표시
	if (Txt_Countdown)
	{
		if (RemainingTime >= 0 && CurrentCount >= MaxPlayers)
		{
			Txt_Countdown->SetVisibility(ESlateVisibility::Visible);
			Txt_Countdown->SetText(FText::AsNumber(RemainingTime));
		}
		else
		{
			Txt_Countdown->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UPortalMainWidget::OnCountdownTick()
{
	CountdownTime--;

	if (Txt_Countdown)
	{
		Txt_Countdown->SetText(FText::AsNumber(CountdownTime));
	}

	if (CountdownTime <= 0)
	{
		FinishCountdown();
	}
}

void UPortalMainWidget::FinishCountdown()
{
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
}
