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
	if (SinkCurve)
	{
		StartLocation = GetActorLocation();
		SinkTimeline->PlayFromStart();
	}
}

void AArenaFloor::Multicast_StartSinking_Implementation()
{
	StartSinking();
}

void AArenaFloor::TimelineProgress(float Value)
{
	FVector NewLocation = StartLocation;
	NewLocation.Z = FMath::Lerp(StartLocation.Z, StartLocation.Z + TargetZOffset, Value);

	if (FloorMesh) FloorMesh->SetWorldLocation(NewLocation);
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
