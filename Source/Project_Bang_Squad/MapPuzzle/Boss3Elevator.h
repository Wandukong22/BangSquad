#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Boss3Elevator.generated.h"

class UStaticMeshComponent;
class UCurveFloat;

UCLASS()
class PROJECT_BANG_SQUAD_API ABoss3Elevator : public AActor
{
	GENERATED_BODY()

public:
	ABoss3Elevator();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// 스위치 등 외부에서 이 함수를 호출하여 엘리베이터 가동
	UFUNCTION(BlueprintCallable, Category = "Boss3")
	void ActivateElevator();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UTimelineComponent> ElevatorTimeline;

	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
	TObjectPtr<UCurveFloat> RiseCurve;

	UPROPERTY(EditAnywhere, Category = "Elevator Settings")
	float RiseHeight = 1000.0f;

	UPROPERTY(VisibleInstanceOnly, Category = "Elevator Settings")
	bool bIsActivated = false;


	UPROPERTY(EditInstanceOnly, Category = "Linked Systems")
	TArray<AActor*> TargetMachines;


	UFUNCTION()
	void HandleTimelineProgress(float Value);

	UFUNCTION()
	void OnTimelineFinished();

private:
	FVector StartLocation;
	FVector EndLocation;
};