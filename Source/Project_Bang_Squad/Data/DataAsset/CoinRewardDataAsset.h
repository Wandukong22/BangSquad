// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "CoinRewardDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FCoinRewardData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StageClearReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> MiniGameRankRewards;
};

UCLASS()
class PROJECT_BANG_SQUAD_API UCoinRewardDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BS|Data")
	TMap<EStageIndex, FCoinRewardData> RewardMap;
};
