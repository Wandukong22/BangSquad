// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "BSMapData.generated.h"

USTRUCT(BlueprintType)
struct FMapInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStageIndex StageIndex = EStageIndex::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStageSection Section = EStageSection::Unknown;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Level;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* LoadingImage;
};

UCLASS()
class PROJECT_BANG_SQUAD_API UBSMapData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FMapInfo> Maps;

	const FMapInfo* GetMapInfo(EStageIndex StageIndex, EStageSection Section) const;
	FString GetMapPath(EStageIndex StageIndex, EStageSection Section) const;
	FString GetMapDisplayName(EStageIndex StageIndex, EStageSection Section) const;
};
