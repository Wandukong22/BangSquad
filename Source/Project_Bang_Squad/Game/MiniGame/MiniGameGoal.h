// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Project_Bang_Squad/Game/InteractionInterface.h"
#include "MiniGameGoal.generated.h"

UCLASS()
class PROJECT_BANG_SQUAD_API AMiniGameGoal : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AMiniGameGoal();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};