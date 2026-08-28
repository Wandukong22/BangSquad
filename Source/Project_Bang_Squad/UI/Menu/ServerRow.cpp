// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/Menu/ServerRow.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UServerRow::NativeConstruct()
{
	Super::NativeConstruct();
	if (RowButton) RowButton->OnClicked.AddDynamic(this, &UServerRow::HandleRowButtonClicked);
}

void UServerRow::SetUp(const FBSSessionSummary& Summary)
{
	ResultId = Summary.ResultId;

	if (RoomNameText) RoomNameText->SetText(FText::FromString(Summary.RoomName));
	if (HostNameText) HostNameText->SetText(FText::FromString(Summary.HostName));
	if (PlayerCountText)
	{
		const FString PlayerCount = FString::Printf(
			TEXT("%d/%d"), Summary.CurrentPlayerCount, Summary.MaxPlayerCount);
		PlayerCountText->SetText(FText::FromString(PlayerCount));
	}
	if (PingText)
	{
		const FString Ping = FString::Printf(TEXT("%d ms"), Summary.PingInMs);
		PingText->SetText(FText::FromString(Ping));
	}
	SetSelected(false);
}

void UServerRow::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;
	if (SelectionBorder)
	{
		SelectionBorder->SetVisibility(bSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UServerRow::HandleRowButtonClicked()
{
	if (!ResultId.IsValid()) return;
	OnServerRowSelected.Broadcast(ResultId);
}
