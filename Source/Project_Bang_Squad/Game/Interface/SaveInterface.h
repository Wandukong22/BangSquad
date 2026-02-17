// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveInterface.generated.h"

USTRUCT(BlueprintType)
struct FActorSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform ActorTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, float> FloatData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, bool> BoolData;
};

UINTERFACE(MinimalAPI)
class USaveInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECT_BANG_SQUAD_API ISaveInterface
{
	GENERATED_BODY()

public:
	virtual FName GetSaveID() const = 0;
	virtual void SaveActorData(FActorSaveData& OutData) = 0;
	virtual void LoadActorData(const FActorSaveData& InData) = 0;
};
