// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Stage/Checkpoint.h"

#include "StageGameState.h"
#include "Project_Bang_Squad/Game/Base/InGameState.h"
#include "Components/BoxComponent.h"
#include "Project_Bang_Squad/Game/MiniGame/MiniGamePlayerState.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"

ACheckpoint::ACheckpoint()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	//오버랩 설정
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlapBegin);

	if (AInGameState* GS = GetWorld()->GetGameState<AInGameState>())
	{
		GS->RegisterCheckpoint(CheckpointIndex, this);
	}
}

void ACheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	//플레이어인지 확인
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn && Pawn->GetController())
	{
		APlayerState* PS = Pawn->GetController()->PlayerState;
		if (AMiniGamePlayerState* MiniPS = Cast<AMiniGamePlayerState>(PS))
		{
			MiniPS->UpdateMiniGameCheckpoint(CheckpointIndex);
		}
		else if (Cast<AStagePlayerState>(PS))
		{
			if (AStageGameState* GS = GetWorld()->GetGameState<AStageGameState>())
			{
				GS->UpdateStageCheckpoint(CheckpointIndex);
			}
		}
	}
}
