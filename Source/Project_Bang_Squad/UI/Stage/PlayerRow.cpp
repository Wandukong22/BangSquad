// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerRow.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Project_Bang_Squad/Character/Component/HealthComponent.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"


void UPlayerRow::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (CurrentMode == ERowMode::Stage && TargetPlayerState.IsValid())
	{
		UpdateStageInfo();
	}
}

void UPlayerRow::NativeConstruct()
{
	Super::NativeConstruct();
	if (Img_Profile)
	{
		ProfileMaterial = Img_Profile->GetDynamicMaterial();
	}
}

void UPlayerRow::SetWidgetMode(ERowMode NewMode)
{
	CurrentMode = NewMode;

	if (CurrentMode == ERowMode::Lobby)
	{
		if (Txt_ReadyState)
			Txt_ReadyState->SetVisibility(ESlateVisibility::Visible);
		if (PB_HpBar)
			PB_HpBar->SetVisibility(ESlateVisibility::Collapsed);
		if (Img_HpBarFrame)
			Img_HpBarFrame->SetVisibility(ESlateVisibility::Collapsed);
		if (Overlay_Death)
			Overlay_Death->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		if (Txt_ReadyState)
			Txt_ReadyState->SetVisibility(ESlateVisibility::Collapsed);
		if (PB_HpBar)
			PB_HpBar->SetVisibility(ESlateVisibility::Visible);
		if (Img_HpBarFrame)
			Img_HpBarFrame->SetVisibility(ESlateVisibility::Visible);
		if (Overlay_Death)
			Overlay_Death->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerRow::SetTargetPlayerState(class APlayerState* InPlayerState)
{
	TargetPlayerState = InPlayerState;
	if (TargetPlayerState.IsValid() && Txt_Name)
	{
		Txt_Name->SetText(FText::FromString(TargetPlayerState->GetPlayerName()));
	}
}

void UPlayerRow::UpdateLobbyInfo(bool bIsReady, EJobType JobType)
{
	UpdateProfileImage(JobType);
	if (Txt_ReadyState)
	{
		Txt_ReadyState->SetText(bIsReady ? FText::FromString("READY") : FText::FromString("READY"));
		Txt_ReadyState->SetColorAndOpacity(bIsReady ? FLinearColor::Green : FLinearColor::Red);
	}
}

void UPlayerRow::UpdateProfileImage(EJobType JobType)
{
	//직업 아이콘 설정
	if (ProfileMaterial)
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			if (UTexture2D* FoundTexture = GI->GetJobIcon(JobType))
			{
				ProfileMaterial->SetTextureParameterValue(FName("ProfileImage"), FoundTexture);
			}
		}
	}

	//직업별 프레임 색상 설정
	if (Img_ProfileFrame)
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			Img_ProfileFrame->SetColorAndOpacity(GI->GetJobColor(JobType));
		}
	}
}

void UPlayerRow::UpdateStageInfo()
{
	if (!TargetPlayerState.IsValid()) return;

	if (Txt_Name)
	{
		Txt_Name->SetText(FText::FromString(TargetPlayerState->GetPlayerName()));
	}


	//HP바
	APawn* MyPawn = TargetPlayerState->GetPawn();
	if (MyPawn)
	{
		if (UHealthComponent* HPComp = MyPawn->FindComponentByClass<UHealthComponent>())
		{
			float Percent = (HPComp->MaxHealth > 0) ? (HPComp->CurrentHealth / HPComp->MaxHealth) : 0.f;
			if (PB_HpBar) PB_HpBar->SetPercent(Percent);
		}
	}
	else
	{
		if (PB_HpBar) PB_HpBar->SetPercent(0.f);
	}

	//부활 시간 & 사망 오버레이
	if (AStagePlayerState* StagePS = Cast<AStagePlayerState>(TargetPlayerState.Get()))
	{
		float RemainTime = 0.f;

		if (GetWorld()->GetGameState())
		{
			RemainTime = StagePS->GetRespawnEndTime() - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
		}
		if (RemainTime > 0.f) //죽음
		{
			if (Overlay_Death) Overlay_Death->SetVisibility(ESlateVisibility::Visible);
			if (Txt_RespawnTime) Txt_RespawnTime->SetText(FText::AsNumber(FMath::CeilToInt(RemainTime)));

			//프로필 어둡게
			if (Img_Profile) Img_Profile->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.f));
		}
		else
		{
			if (Overlay_Death) Overlay_Death->SetVisibility(ESlateVisibility::Hidden);
			if (Img_Profile) Img_Profile->SetColorAndOpacity(FLinearColor::White);
		}

		UpdateProfileImage(StagePS->GetJob());
	}
}
