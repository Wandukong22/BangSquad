// Fill out your copyright notice in the Description page of Project Settings.


#include "BSGameMode.h"

#include "BSPlayerState.h"
#include "GameFramework/Character.h"

ABSGameMode::ABSGameMode()
{
}

void ABSGameMode::SpawnPlayerCharacter(AController* Controller, EJobType JobType)
{
	if (!Controller) return;
	UBSGameInstance* GI = GetBSGameInstance();
	if (!GI) return;
	
	TSubclassOf<ACharacter> PawnClass = GI->GetCharacterClass(JobType);
	if (!PawnClass) return;
	
	//기존에 붙어있던 폰이 있다면 제거
	if (APawn* OldPawn = Controller->GetPawn())
	{
		OldPawn->Destroy();
	}

	//스폰 위치
	FTransform SpawnTransform = GetRespawnTransform(Controller);

	//소환 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnParams))
	{
		Controller->Possess(NewPawn);
	}
}

FTransform ABSGameMode::GetRespawnTransform(AController* Controller)
{
	AActor* PlayerStart = FindPlayerStart(Controller);
	if (PlayerStart)
	{
		return PlayerStart->GetActorTransform();
	}
	return FTransform::Identity;
}

void ABSGameMode::RequestRespawn(AController* Controller)
{
	if (!Controller) return;

	float WaitTime = GetRespawnDelay(Controller);
	
	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindUObject(this, &ABSGameMode::ExecuteRespawn, Controller);

	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, WaitTime, false);
}


void ABSGameMode::ExecuteRespawn(AController* Controller)
{
	if (!Controller) return;

	EJobType JobToSpawn = EJobType::Titan;
	
	if (ABSPlayerState* PS = Controller->GetPlayerState<ABSPlayerState>())
	{
		JobToSpawn = PS->GetJob();
	}

	SpawnPlayerCharacter(Controller, JobToSpawn);
}

UBSGameInstance* ABSGameMode::GetBSGameInstance() const
{
	return Cast<UBSGameInstance>(GetGameInstance());
}
