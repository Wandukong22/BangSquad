// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Lobby/LobbyGameMode.h"

#include "LobbyGameState.h"
#include "LobbyPlayerController.h"
#include "LobbyPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"

ALobbyGameMode::ALobbyGameMode()
{
	PlayerStateClass = ALobbyPlayerState::StaticClass();
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
	GameStateClass = ALobbyGameState::StaticClass();

	bUseSeamlessTravel = true;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//TODO: 초기화 작업들 (PlayerState 데이터 갱신 등등...)
}

bool ALobbyGameMode::TryConfirmJob(EJobType Job, class ALobbyPlayerState* RequestingPS)
{
	if (ConfirmedJobs.Contains(Job)) return false;

	//플레이어 상태 업데이트
	RequestingPS->SetJob(Job);
	RequestingPS->SetIsConfirmedJob(true);

	//목록에 등록
	ConfirmedJobs.Add(Job);

	CheckConfirmedJob();

	return true;
}

void ALobbyGameMode::ChangePlayerCharacter(AController* Controller, EJobType NewJob)
{
	if (!Controller) return;

	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	if (!GI) return;
	
	TSubclassOf<ACharacter> TargetClass = GI->GetCharacterClass(NewJob);
	if (!TargetClass) return;
	
	APawn* OldPawn = Controller->GetPawn();
	FVector Loc = OldPawn ? OldPawn->GetActorLocation() : FVector::ZeroVector;
	FRotator Rot = OldPawn ? OldPawn->GetActorRotation() : FRotator::ZeroRotator;
	if (OldPawn) OldPawn->Destroy();
	
	if (ACharacter* NewChar = GetWorld()->SpawnActor<ACharacter>(TargetClass, Loc, Rot))
	{
		Controller->Possess(NewChar);
	}
}

void ALobbyGameMode::CheckAllReady()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS) return;

	if (GS->PlayerArray.Num() == 0) return;

	bool bAllReady = true;
	int32 ReadyCount = 0;

	for (APlayerState* PS : GS->PlayerArray)
	{
		ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS);
		if (LobbyPS)
		{
			if (LobbyPS->bIsReady) ReadyCount++;
			else bAllReady = false;
		}
	}
	
	//이동
	if (bAllReady && GS->PlayerArray.Num() == PlayerCount)
	{
		GS->SetLobbyPhase(ELobbyPhase::SelectJob);
	}
}

void ALobbyGameMode::CheckConfirmedJob()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS) return;

	if (GS->PlayerArray.Num() == 0) return;

	int32 ConfirmedCount = 0;
	int32 TotalPlayers = GS->PlayerArray.Num();

	for (APlayerState* PS : GS->PlayerArray)
	{
		ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS);
		if (LobbyPS && LobbyPS->bIsConfirmedJob)
		{
			ConfirmedCount++;
		}
	}
	
	//모두 직업 확정 완료
	if (ConfirmedCount == TotalPlayers)
	{
		//상태 변경
		if (GS)
			GS->SetLobbyPhase(ELobbyPhase::GameStarting);

		//상태 동기화 시간 벌기 위해 1초 뒤 이동
		FTimerHandle TravelTimer;
		GetWorldTimerManager().SetTimer(TravelTimer, [this, WeakThis = TWeakObjectPtr<ALobbyGameMode>(this)]()
		{
			if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
			{
				GI->MoveToStage(EStageIndex::Stage1, EStageSection::Main);
			}
		}, 1.f, false);
	}
}
