// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "Project_Bang_Squad/Game/InteractionInterface.h"
#include "MiniGameGoal.generated.h"

class UWidgetComponent;
class USphereComponent;

UCLASS()
class PROJECT_BANG_SQUAD_API AMiniGameGoal : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AMiniGameGoal();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* DetectSphere;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractWidget;
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "BS|Goal")
	EStageIndex StageIndex = EStageIndex::Stage1;
private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};