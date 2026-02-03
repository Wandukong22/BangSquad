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

		//전체 플레이어 수 변경 감지
		if (GS->PlayerArray.Num() != CachedPlayerCount)
		{
			bNeedUpdate = true;
		}
		else
		{
			//1초마다 UI 개수 정합성 체크
			static float TimeAccumulator = 0.f;
			TimeAccumulator += DeltaTime;
			
			if (TimeAccumulator > 1.f)
			{
				TimeAccumulator = 0.f;
				bNeedUpdate = true;
				/*if (PlayerListContainer)
				{
					int32 SelfCount = (GetOwningPlayerState()) ? 1 : 0;
					int32 ExpectedChildCount = GS->PlayerArray.Num() - SelfCount;

					ExpectedChildCount = FMath::Max(ExpectedChildCount, 0);

					if (PlayerListContainer->GetChildrenCount() != ExpectedChildCount)
					{
						bNeedUpdate = true;
					}
				}*/
				/*else if (PlayerListContainer)
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
				}*/
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
	if (!PlayerListContainer || !PlayerRowClass) return;

	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
	if (!GS) return;

	APlayerState* MyPS = GetOwningPlayerState();
	int32 CurrentChildIndex = 0;

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
				//MyInfoRow->UpdateStageInfo();
				MyInfoRow->SetVisibility(ESlateVisibility::Visible);
			}
			continue;
		}
		//팀원 위젯 처리
		UPlayerRow* Row = nullptr;
		
		if (CurrentChildIndex < PlayerListContainer->GetChildrenCount())
		{
			Row = Cast<UPlayerRow>(PlayerListContainer->GetChildAt(CurrentChildIndex));
		}
		if (!Row)
		{
			Row = CreateWidget<UPlayerRow>(this, PlayerRowClass);
			if (Row)
			{
				PlayerListContainer->AddChild(Row);
			}
		}

		if (Row)
		{
			//누구 정보인지 타겟 설정
			Row->SetTargetPlayerState(PS);
			//모드 설정 (Stage 모드로 변경 -> HP바 켜짐 등)
			Row->SetWidgetMode(ERowMode::Stage);
			//님이 만든 그 정보 갱신 함수 호출 (부활시간, 프로필, HP바 등 한방에 세팅)
			//Row->UpdateStageInfo();
			CurrentChildIndex++;

		}
	}
	while (PlayerListContainer->GetChildrenCount() > CurrentChildIndex)
	{
		PlayerListContainer->RemoveChildAt(PlayerListContainer->GetChildrenCount() - 1);
	}
}
