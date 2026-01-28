// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "BSJobData.generated.h"

USTRUCT(BlueprintType)
struct FJobInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EJobType JobType = EJobType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor JobColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ACharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText Description;
};

UCLASS()
class PROJECT_BANG_SQUAD_API UBSJobData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BS|Data")
	TArray<FJobInfo> Jobs;

	UFUNCTION(BlueprintCallable, Category = "BS|Data")
	const FJobInfo& GetJobInfo(EJobType InJobType) const;

	UFUNCTION(BlueprintCallable, Category = "BS|Data")
	TSubclassOf<ACharacter> GetCharacterClass(EJobType InJobType) const;

	UFUNCTION(BlueprintCallable, Category = "BS|Data")
	FLinearColor GetJobColor(EJobType InJobType) const;

	UFUNCTION(BlueprintCallable, Category = "BS|Data")
	UTexture2D* GetJobIcon(EJobType InJobType) const;
	
};
