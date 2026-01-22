// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECT_BANG_SQUAD_API IInteractionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void Interact(APawn* InstigatorPawn);
};
