// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/MiniGame/MiniGameMode.h"

#include "EngineUtils.h"
#include "MiniGamePlayerController.h"
#include "MiniGamePlayerState.h"
#include "GameFramework/Character.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Game/Stage/Checkpoint.h"
#include "Project_Bang_Squad/Game/Stage/StageGameState.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerController.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"


AMiniGameMode::AMiniGameMode()
{
	//맵 이동 시 끊김 최소화
	bUseSeamlessTravel = true;

	PlayerStateClass = AMiniGamePlayerState::StaticClass();
	PlayerControllerClass = AMiniGamePlayerController::StaticClass();
	GameStateClass = AStageGameState::StaticClass();
}

void AMiniGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AMiniGameMode::SpawnPlayerCharacter(AController* Controller, EJobType JobType)
{
	if (!Controller) return;

	// 기존 폰 제거
	if (APawn* OldPawn = Controller->GetPawn())
	{
		OldPawn->Destroy();
	}

	FTransform SpawnTransform = GetRespawnTransform(Controller);

	// 소환
	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	if (!GI) return;
	
	UClass* PawnClass = GI->GetCharacterClass(JobType);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnParams))
	{
		Controller->Possess(NewPawn);
	}
}

void AMiniGameMode::RequestRespawn(AController* DeadPlayer)
{
	if (!DeadPlayer) return;
	
	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindUObject(this, &AMiniGameMode::ExecuteRespawn, DeadPlayer);

	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnTime, false);
}

void AMiniGameMode::OnPlayerReachedGoal(AController* ReachedPlayer)
{
	if (FinishedPlayers.Contains(ReachedPlayer)) return;

	FinishedPlayers.Add(ReachedPlayer);
	int32 Rank = FinishedPlayers.Num();

	//PlayerState에 순위 고정
	if (ReachedPlayer)
	{
		if (AMiniGamePlayerState* PS = ReachedPlayer->GetPlayerState<AMiniGamePlayerState>())
		{
			PS->SetMiniGameRank(Rank);
		}
	}
	//보상 로직
	
	//미니게임 종료
	CheckAllPlayersFinished();
}

void AMiniGameMode::ExecuteRespawn(AController* Controller)
{
	if (!Controller) return;

	EJobType JobToSpawn = EJobType::None;

	if (ABSPlayerState* PS = Controller->GetPlayerState<ABSPlayerState>())
	{
		JobToSpawn = PS->GetJob();
	}

	if (JobToSpawn == EJobType::None)
	{
		JobToSpawn = EJobType::Titan;
	}
	
	SpawnPlayerCharacter(Controller, JobToSpawn);
}

FTransform AMiniGameMode::GetRespawnTransform(AController* Controller)
{
	UWorld* World = GetWorld();
	if (!World) return FTransform::Identity;

	if (Controller)
	{
		AMiniGamePlayerState* PS = Controller->GetPlayerState<AMiniGamePlayerState>();
		// 체크포인트 인덱스가 0보다 크면 해당 체크포인트를 찾음
		if (PS && PS->GetMiniGameCheckpoint() > 0)
		{
			int32 TargetIndex = PS->GetMiniGameCheckpoint();

			// 월드에 있는 체크포인트 액터 중 내 인덱스와 같은 것 검색
			for (TActorIterator<ACheckpoint> It(World); It; ++It)
			{
				ACheckpoint* Checkpoint = *It;
				if (Checkpoint && Checkpoint->GetCheckpointIndex() == TargetIndex)
				{
					return Checkpoint->GetActorTransform();
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

void AMiniGameMode::CheckAllPlayersFinished()
{
	int32 TotalPlayers = GetNumPlayers();
	if (TotalPlayers <= 0) return;

	if (FinishedPlayers.Num() >= 1)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
			{
				GI->MoveToStage(EStageIndex::Stage1, EStageSection::Main);
			}
			//World->ServerTravel("/Game/TeamShare/Level/Stage1_Demo?listen");
		}
	}
}
