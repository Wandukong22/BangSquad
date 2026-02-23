// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaFloor.h"

#include "Components/TimelineComponent.h"

AArenaFloor::AArenaFloor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	RootComponent = FloorMesh;
	FloorMesh->SetMobility(EComponentMobility::Movable);
	
	SinkTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SinkTimeline"));
}

void AArenaFloor::StartSinking()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Floor %d Start Sinking!"), FloorNumber));

	if (SinkCurve == nullptr)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("에러: SinkCurve가 할당되지 않았습니다!"));
		return;
	}
	
	StartLocation = GetActorLocation();
	SinkTimeline->PlayFromStart();
}

void AArenaFloor::Multicast_StartSinking_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("StartSinking called on: %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
	StartSinking();
}

void AArenaFloor::TimelineProgress(float Value)
{
	UE_LOG(LogTemp, Warning, TEXT("TimelineProgress: %f"), Value);
	
	FVector NewLocation = StartLocation;
	NewLocation.Z = FMath::Lerp(StartLocation.Z, StartLocation.Z + TargetZOffset, Value);

	if (FloorMesh) FloorMesh->SetWorldLocation(NewLocation);
	//SetActorLocation(NewLocation);
}

void AArenaFloor::TimelineFinished()
{
}

void AArenaFloor::BeginPlay()
{
	Super::BeginPlay();

	if (SinkCurve)
	{
		FOnTimelineFloat TimelineProgressDelegate;
		TimelineProgressDelegate.BindUFunction(this, FName("TimelineProgress"));
		SinkTimeline->AddInterpFloat(SinkCurve, TimelineProgressDelegate);

		FOnTimelineEvent TimelineFinishedDelegate;
		TimelineFinishedDelegate.BindUFunction(this, FName("TimelineFinished"));
		SinkTimeline->SetTimelineFinishedFunc(TimelineFinishedDelegate);

		SinkTimeline->SetPlayRate(1.f / SinkDuration);
	}
}
