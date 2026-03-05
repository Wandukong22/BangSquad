// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Stage/StageGameMode.h"

#if WITH_EDITOR
#include "BlueprintCompiledStatement.h"
#endif

#include "StagePlayerController.h"
#include "StagePlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "Checkpoint.h"
#include "EngineUtils.h"
#include "StageGameState.h"

AStageGameMode::AStageGameMode()
{
	PlayerStateClass = AStagePlayerState::StaticClass();
	PlayerControllerClass = AStagePlayerController::StaticClass();
}

FTransform AStageGameMode::GetRespawnTransform(AController* Controller)
{
	UWorld* World = GetWorld();
	if (!World) return FTransform::Identity;
	
	AStageGameState* GS = World->GetGameState<AStageGameState>();
	int32 TargetIndex = 0;
	
	if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
	{
		TargetIndex = GI->GetSavedCheckpointIndex();
	}
	if (TargetIndex == 0)
	{
		TargetIndex = GS->GetStageCheckpointIndex();
	}
	
	//스테이지에서의 체크포인트 확인
	
	if (TargetIndex > 0)
	{
		if (GS && GS->CheckpointMap.Contains(TargetIndex))
		{
			ACheckpoint* CP = GS->CheckpointMap[TargetIndex];
			if (IsValid(CP))
			{
				return CP->GetActorTransform();
			}
		}
		else
		{
			for (TActorIterator<ACheckpoint> It(World); It; ++It)
			{
				if (It->GetCheckpointIndex() == TargetIndex)
				{
					if (GS)
					{
						GS->CheckpointMap.Add(TargetIndex, *It);
					}
					return It->GetActorTransform();
				}
			}
		}
	}

	//아무것도 없을 땐 PlayerStart 위치
	AActor* StartSpot = FindPlayerStart(Controller);
	if (StartSpot)
	{
		return StartSpot->GetActorTransform();
	}
	return FTransform::Identity;
}

float AStageGameMode::GetRespawnDelay(AController* Controller)
{
	float FinalTime = BaseRespawnTime; // 기본 3초

	if (AStagePlayerState* PS = Controller->GetPlayerState<AStagePlayerState>())
	{
		PS->IncreaseDeathCount();
		
		// 시간 계산 (기본 + 죽은횟수 * 2초), 최대 15초 제한
		FinalTime = BaseRespawnTime + ((PS->GetDeathCount() - 1) * 2.f);
		FinalTime = FMath::Min(FinalTime, 15.f);
	}

	return FinalTime;
}

void AStageGameMode::OnBossDefeated()
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	if (!GI) return;

	// ✨ 스테이지 3 보스를 잡은 경우에만 엔딩 실행
	if (GI->GetCurrentStage() == EStageIndex::Stage3)
	{
		// 5초 뒤 엔딩 영상 시작
		GetWorldTimerManager().SetTimer(EndingTimerHandle, this, &AStageGameMode::StartEndingVideo, 5.0f, false);
	}
	else
	{
		// 스테이지 1, 2 보스라면 일반적인 스테이지 클리어 로직 처리 (예: 포탈 생성 등)
		UE_LOG(LogTemp, Log, TEXT("스테이지 3이 아니므로 엔딩을 재생하지 않습니다."));
	}
}

void AStageGameMode::StartEndingVideo()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AStagePlayerController* PC = Cast<AStagePlayerController>(It->Get()))
		{
			PC->Client_PlayEndingVideo(); // 클라이언트 화면에 영상 출력
		}
	}

	GetWorldTimerManager().SetTimer(EndingTimerHandle, this, &AStageGameMode::ReturnToMainMenu, EndingVideoDuration, false);
}

void AStageGameMode::ReturnToMainMenu()
{
	if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
	{
		GI->ResetAllGameData(); // 데이터 초기화
		GI->MoveToStage(EStageIndex::Lobby, EStageSection::Main); // 로비로 이동
	}
}