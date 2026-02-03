// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/Stage/Boss/QTEWidget.h"

#include "Components/TextBlock.h"
#include "Project_Bang_Squad/Character/StageBoss/StageBossGameState.h"
#include "Project_Bang_Squad/Character/StageBoss/StageBossPlayerState.h"

void UQTEWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (KeyCountText)
	{
		KeyCountText->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
		KeyCountText->SetText(FText::AsNumber(0));
	}

	SetVisibility(ESlateVisibility::Hidden);
	// [추가] 게임스테이트 찾아서 "QTE 켜졌니?" 방송 구독
	if (auto* GS = GetWorld()->GetGameState<AStageBossGameState>())
	{
		// 중복 방지 후 연결
		GS->OnQTEStateChanged.RemoveDynamic(this, &UQTEWidget::HandleQTEStateChanged);
		GS->OnQTEStateChanged.AddDynamic(this, &UQTEWidget::HandleQTEStateChanged);
		
		// (안전장치) 이미 켜져있는 상태일 수도 있으니 현재 상태 확인
		if (GS->bIsQTEActive) 
		{
			HandleQTEStateChanged(true);
		}
	}
}

void UQTEWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsAnimating && KeyCountText)
	{
		CurrentAnimTime += InDeltaTime;

		//진행도
		float Alpha = FMath::Clamp(CurrentAnimTime / AnimDuration, 0.f, 1.f);

		//Sin(0~PI): 0->1->0 부드럽게 변함
		float ScaleValue = 1.f + (MaxScale - 1.f) * FMath::Sin(Alpha * PI);

		//실제 UI에 적용
		KeyCountText->SetRenderScale(FVector2D(ScaleValue, ScaleValue));

		//끝났으면 종료
		if (Alpha >= 1.f)
		{
			bIsAnimating = false;
			KeyCountText->SetRenderScale(FVector2D(1.f, 1.f));
		}
	}
}

void UQTEWidget::UpdateKeyCount(int32 KeyCount)
{
	if (KeyCountText)
	{
		if (LastKeyCount != KeyCount)
		{
			KeyCountText->SetText(FText::AsNumber(KeyCount));

			if (KeyCount > 0)
			{
				ShowWidget();
				CurrentAnimTime = 0.f;
				bIsAnimating = true;
			}
			else
			{
				HideWidget();
			}

			LastKeyCount = KeyCount;
		}
	}
}

void UQTEWidget::SetTargetPlayerState(class AStageBossPlayerState* InTargetPlayerState)
{
	TargetPlayerState = InTargetPlayerState;

	if (TargetPlayerState.IsValid())
	{
		
		TargetPlayerState->OnPersonalQTEChanged.RemoveDynamic(this, &UQTEWidget::UpdateKeyCount);
		TargetPlayerState->OnPersonalQTEChanged.AddDynamic(this, &UQTEWidget::UpdateKeyCount);

		UpdateKeyCount(TargetPlayerState->PersonalQTECount);
		bIsInitialized = true;
	}
}

void UQTEWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UQTEWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UQTEWidget::HandleQTEStateChanged(bool bIsActive)
{
	if (bIsActive)
	{
		ShowWidget(); // 켜기
	}
	else
	{
		HideWidget(); // 끄기
	}
}
