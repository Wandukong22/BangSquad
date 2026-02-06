// Fill out your copyright notice in the Description page of Project Settings.


#include "JobButton.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"

void UJobButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Btn_Click)
	{
		Btn_Click->OnClicked.AddDynamic(this, &UJobButton::OnButtonClicked);
	}
	UpdateButtonIcon();
	SetSelectedState(false);
}

void UJobButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	UpdateButtonIcon();
}

void UJobButton::SetSelectedState(bool bSelected)
{
	if (Img_Border)
	{
		Img_Border->SetVisibility(bSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UJobButton::OnButtonClicked()
{
	if (OnJobSelected.IsBound())
	{
		OnJobSelected.Broadcast(AssignedJob);
	}
}

void UJobButton::UpdateButtonIcon()
{
	if (!Img_JobIcon) return;
	if (AssignedJob == EJobType::None) return;

	if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
	{
		if (UTexture2D* FoundTexture = GI->GetJobIcon(AssignedJob))
		{
			Img_JobIcon->SetBrushFromTexture(FoundTexture);
		}
	}
}
