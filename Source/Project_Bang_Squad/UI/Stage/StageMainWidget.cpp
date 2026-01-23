// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/Stage/StageMainWidget.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Project_Bang_Squad/Character/Component/HealthComponent.h"

void UStageMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedPlayerCount = 0;
}

void UStageMainWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>())
	{
		bool bNeedUpdate = false;

		if (GS->PlayerArray.Num() != CachedPlayerCount)
		{
			bNeedUpdate = true;
		}
		else
		{
			static float TimeAccumulator = 0.f;
			TimeAccumulator += DeltaTime;
			if (TimeAccumulator > 1.f)
			{
				TimeAccumulator = 0.f;

				if (PlayerListContainer && PlayerListContainer->GetChildrenCount() != GS->PlayerArray.Num())
				{
					bNeedUpdate = true;
				}
				else if (PlayerListContainer)
				{
					for (int32 i = 0; i < GS->PlayerArray.Num(); ++i)
					{
						UPlayerRow* Row = Cast<UPlayerRow>(PlayerListContainer->GetChildAt(i));

						if (!Row || Row->GetTargetPlayerState() != GS->PlayerArray[i])
						{
							bNeedUpdate = true;
							break;
						}
					}
				}
			}
		}

		if (bNeedUpdate)
		{
			UpdatePartyList();
			CachedPlayerCount = GS->PlayerArray.Num();
		}
	}
}

void UStageMainWidget::UpdatePartyList()
{
	// 1. 안전 장치
	if (!PlayerListContainer || !PlayerRowClass)
	{
		return;
	}

	// 2. 기존 목록 초기화 (중복 방지)
	PlayerListContainer->ClearChildren();

	// 3. GameState 가져오기
	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
	if (!GS) return;

	//내 플레이어 스테이트
	APlayerState* MyPS = GetOwningPlayerState();

	// 4. 플레이어 목록 순회하며 생성
	for (APlayerState* PS : GS->PlayerArray)
	{
		if (!PS) continue;
		if (PS == MyPS)
		{
			//내 정보 업데이트
			if (MyInfoRow)
			{
				MyInfoRow->SetTargetPlayerState(PS);
				MyInfoRow->SetWidgetMode(ERowMode::Stage);
				MyInfoRow->UpdateStageInfo();
				MyInfoRow->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			UPlayerRow* Row = CreateWidget<UPlayerRow>(this, PlayerRowClass);
			if (Row)
			{
				//누구 정보인지 타겟 설정

				Row->SetTargetPlayerState(PS);
				//모드 설정 (Stage 모드로 변경 -> HP바 켜짐 등)
				Row->SetWidgetMode(ERowMode::Stage);
				//님이 만든 그 정보 갱신 함수 호출 (부활시간, 프로필, HP바 등 한방에 세팅)
				Row->UpdateStageInfo();
				//컨테이너에 추가
				PlayerListContainer->AddChild(Row);
			}
		}
	}
}
