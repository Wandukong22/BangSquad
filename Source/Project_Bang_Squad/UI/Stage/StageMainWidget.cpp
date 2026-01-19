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
		int32 CurrentPlayerCount = GS->PlayerArray.Num();

		if (CurrentPlayerCount != CachedPlayerCount)
		{
			UpdatePartyList();
			CachedPlayerCount = CurrentPlayerCount;
		}
	}
}

void UStageMainWidget::UpdatePartyList()
{
	UE_LOG(LogTemp, Warning, TEXT("=== UpdatePartyList Start ==="));
	
	// 1. 안전 장치
	if (!PlayerListContainer || !PlayerRowClass)
	{
		UE_LOG(LogTemp, Error, TEXT("FAIL: PlayerListContainer is NULL! (Check BindWidget in C++ / Name in BP)"));
		UE_LOG(LogTemp, Error, TEXT("FAIL: PlayerRowClass is NONE! (Check BP_StageMainWidget Default Properties)"));
		return;
	}

	// 2. 기존 목록 초기화 (중복 방지)
	PlayerListContainer->ClearChildren();

	// 3. GameState 가져오기
	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
	if (!GS)
	{
		UE_LOG(LogTemp, Error, TEXT("FAIL: GameState is NULL!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Current Player Count: %d"), GS->PlayerArray.Num());

	int32 CreatedCount = 0; //TODO: Debug용 코드 지우기
	// 4. 플레이어 목록 순회하며 생성
	for (APlayerState* PS : GS->PlayerArray)
	{
		if (!PS) continue;
		
		UPlayerRow* Row = CreateWidget<UPlayerRow>(this, PlayerRowClass);
		if (Row)
		{
			// (1) 누구 정보인지 타겟 설정
			Row->SetTargetPlayerState(PS);

			// (2) 모드 설정 (Stage 모드로 변경 -> HP바 켜짐 등)
			Row->SetWidgetMode(ERowMode::Stage);

			// (3) 님이 만든 그 정보 갱신 함수 호출 (부활시간, 프로필, HP바 등 한방에 세팅)
			Row->UpdateStageInfo();

			// (4) 컨테이너에 추가
			PlayerListContainer->AddChild(Row);
			UE_LOG(LogTemp, Warning, TEXT("-> Added Row for Player: %s"), *PS->GetPlayerName());
		}
	}

	if (CreatedCount == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WARNING: Loop Finished but CreatedCount is 0."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS: Created %d Rows."), CreatedCount);
	}
}
