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

	//스테이지에서의 체크포인트 확인
	AStageGameState* GS = World->GetGameState<AStageGameState>();
	if (!GS) return FindPlayerStart(Controller)->GetActorTransform();
	
	int32 TargetIndex = GS->GetStageCheckpointIndex();

	if (TargetIndex > 0)
	{
		if (GS->CheckpointMap.Contains(TargetIndex))
		{
			ACheckpoint* CP = GS->CheckpointMap[TargetIndex];
			if (IsValid(CP))
			{
				return CP->GetActorTransform();
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
