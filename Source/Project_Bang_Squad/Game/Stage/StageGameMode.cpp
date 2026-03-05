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

void AStageGameMode::RestartPlayer(AController* NewPlayer)
{
	//Super::RestartPlayer(NewPlayer);

	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	// 이미 구현해두신 체크포인트/리스폰 위치를 가져옵니다.
	FTransform SpawnTransform = GetRespawnTransform(NewPlayer);
    
	// 기본 FindPlayerStart 대신, 특정 위치에서 플레이어를 다시 시작시킵니다.
	RestartPlayerAtTransform(NewPlayer, SpawnTransform);
}
