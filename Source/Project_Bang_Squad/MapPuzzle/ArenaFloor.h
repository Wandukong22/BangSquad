// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArenaFloor.generated.h"

class UTimelineComponent;

UCLASS()
class PROJECT_BANG_SQUAD_API AArenaFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArenaFloor();

	void StartSinking();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartSinking();

	int32 GetFloorNumber() const { return FloorNumber; }
	void SetFloorNumber(int32 InFloorNumber) { FloorNumber = InFloorNumber; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FloorMesh;
	
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* SinkTimeline;
	
	UPROPERTY(EditAnywhere, Category = "BS|Sinking")
	UCurveFloat* SinkCurve;

	UPROPERTY(EditAnywhere, Category = "BS|Sinking")
	float TargetZOffset = -500.f;
	UPROPERTY(EditAnywhere, Category = "BS|Sinking")
	float SinkDuration = 5.f; //초
	
	UPROPERTY(EditAnywhere, Category = "BS|Sinking")
	int32 FloorNumber = 1;
	

private:
	FVector StartLocation;

	UFUNCTION()
	void TimelineProgress(float Value);

	UFUNCTION()
	void TimelineFinished();
};
