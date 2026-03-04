// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerRow.h"

#include "Boss/QTEWidget.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Project_Bang_Squad/Character/Component/HealthComponent.h"
#include "Project_Bang_Squad/Character/StageBoss/StageBossPlayerState.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"


void UPlayerRow::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshTimeUI();
}

void UPlayerRow::NativeConstruct()
{
	Super::NativeConstruct();
	if (Img_Profile)
	{
		ProfileMaterial = Img_Profile->GetDynamicMaterial();
	}
	if (QTEWidget)
	{
		QTEWidget->HideWidget();
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
	if (TargetPlayerState.IsValid())
	{
		TargetPlayerState->OnPawnSet.RemoveDynamic(this, &UPlayerRow::OnPawnSet);
		if (ABSPlayerState* BSPS = Cast<ABSPlayerState>(TargetPlayerState.Get()))
		{
			BSPS->OnRespawnTimeChanged.RemoveDynamic(this, &UPlayerRow::UpdateDeathState);
		}
	}

	TargetPlayerState = InPlayerState;
	if (TargetPlayerState.IsValid())
	{
		if (Txt_Name)
			Txt_Name->SetText(FText::FromString(TargetPlayerState->GetPlayerName()));

		if (QTEWidget)
		{
			if (AStageBossPlayerState* BossPS = Cast<AStageBossPlayerState>(TargetPlayerState.Get()))
			{
				QTEWidget->SetTargetPlayerState(BossPS);
			}
		}

		TargetPlayerState->OnPawnSet.AddDynamic(this, &UPlayerRow::OnPawnSet);
		if (ABSPlayerState* BSPS = Cast<ABSPlayerState>(TargetPlayerState.Get()))
		{
			BSPS->OnRespawnTimeChanged.AddDynamic(this, &UPlayerRow::UpdateDeathState);

			UpdateProfileImage(BSPS->GetJob());
			UpdateDeathState(BSPS->GetRespawnEndTime());
		}

		if (TargetPlayerState->GetPawn())
		{
			OnPawnSet(TargetPlayerState.Get(), TargetPlayerState->GetPawn(), nullptr);
		}
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

void UPlayerRow::OnPawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (NewPawn)
	{
		if (UHealthComponent* HPComp = NewPawn->FindComponentByClass<UHealthComponent>())
		{
			HPComp->OnHealthChanged.RemoveDynamic(this, &UPlayerRow::UpdateHealthUI);
			//HPComp->OnDead.RemoveDynamic(this, &UPlayerRow::HandleOnDead);

			HPComp->OnHealthChanged.AddDynamic(this, &UPlayerRow::UpdateHealthUI);
			//HPComp->OnDead.AddDynamic(this, &UPlayerRow::HandleOnDead);

			UpdateHealthUI(HPComp->CurrentHealth, HPComp->MaxHealth);
		}
	}
}

void UPlayerRow::UpdateHealthUI(float CurrentHealth, float MaxHealth)
{
	if (PB_HpBar)
	{
		float Percent = (MaxHealth > 0) ? (CurrentHealth / MaxHealth) : 0.f;
		PB_HpBar->SetPercent(Percent);
	}
}

void UPlayerRow::UpdateDeathState(float NewRespawnTime)
{
	if (!GetWorld()->GetGameState()) return;

	float ServerTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	float RemainTime = NewRespawnTime - ServerTime;

	if (RemainTime > 0.f) // 죽음 상태 진입
	{
		if (Overlay_Death) Overlay_Death->SetVisibility(ESlateVisibility::Visible);
		if (Img_Profile) Img_Profile->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.f)); // 어둡게	}

		RefreshTimeUI();
	}
	else // 부활
	{
		if (Overlay_Death) Overlay_Death->SetVisibility(ESlateVisibility::Hidden);
		if (Img_Profile) Img_Profile->SetColorAndOpacity(FLinearColor::White);
	}
}

void UPlayerRow::RefreshTimeUI()
{
	// 1. 방어 코드: UI가 꺼져있거나 타겟이 없으면 계산 X
	if (!TargetPlayerState.IsValid() || !Overlay_Death) return;

	ABSPlayerState* BSPS = Cast<ABSPlayerState>(TargetPlayerState.Get());
	AGameStateBase* GS = GetWorld()->GetGameState();

	if (BSPS && GS)
	{
		// 2. 시간 계산
		float RemainTime = BSPS->GetRespawnEndTime() - GS->GetServerWorldTimeSeconds();

		// 3. 텍스트 설정
		if (RemainTime > 0.f)
		{
			// 오버레이가 꺼져있다면 켭니다! (이게 없어서 안 떴던 것)
			if (Overlay_Death->GetVisibility() != ESlateVisibility::Visible)
			{
				Overlay_Death->SetVisibility(ESlateVisibility::Visible);
				if (Img_Profile) Img_Profile->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.f)); // 어둡게
			}

			//텍스트 갱신
			int32 CeilTime = FMath::CeilToInt(RemainTime);
			if (Txt_RespawnTime)
			{
				// 혹시 숨겨져 있을 수 있으니 확실하게 켜기
				Txt_RespawnTime->SetVisibility(ESlateVisibility::HitTestInvisible);
				Txt_RespawnTime->SetText(FText::AsNumber(CeilTime));
			}
			UE_LOG(LogTemp, Log, TEXT("부활 대기 중... 남은 시간: %f"), RemainTime);
		}
		else
		{
			if (Overlay_Death->GetVisibility() != ESlateVisibility::Hidden)
			{
				Overlay_Death->SetVisibility(ESlateVisibility::Hidden);
				if (Img_Profile) Img_Profile->SetColorAndOpacity(FLinearColor::White); // 밝게
			}
		}
	}
}

/*
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
*/
