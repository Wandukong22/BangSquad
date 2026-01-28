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
	if (!Img_JobIcon)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Img_JobIcon이 nullptr입니다! 블루프린트에서 이름 확인 필요!"));
		return;
	}
	if (AssignedJob == EJobType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ AssignedJob이 None입니다!"));
		return;
	}

	if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
	{
		if (UTexture2D* FoundTexture = GI->GetJobIcon(AssignedJob))
		{
			Img_JobIcon->SetBrushFromTexture(FoundTexture);
			UE_LOG(LogTemp, Log, TEXT("✅ 아이콘 설정 성공: %s"), *UEnum::GetValueAsString(AssignedJob));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ JobDataAsset에서 아이콘을 찾을 수 없음: %s"), *UEnum::GetValueAsString(AssignedJob));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ GameInstance를 찾을 수 없음!"));
	}
}
