// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Stage/Checkpoint.h"

#include "Components/BoxComponent.h"
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
}

void ACheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//플레이어인지 확인
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn && Pawn->GetController())
	{
		AStagePlayerState* PS = Pawn->GetController()->GetPlayerState<AStagePlayerState>();
		if (PS)
		{
			PS->UpdateMiniGameCheckpoint(CheckpointIndex);
		}
	}
}
