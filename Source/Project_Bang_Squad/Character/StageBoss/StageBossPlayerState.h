// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "StageBossPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPersonalQTEChanged, int32, NewCount);

UCLASS()
class PROJECT_BANG_SQUAD_API AStageBossPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(ReplicatedUsing = OnRep_PersonalQTECount, BlueprintReadOnly, Category = "BS|QTE")
	int32 PersonalQTECount = 0;

	UPROPERTY(BlueprintAssignable, Category = "BS|Events")
	FOnPersonalQTEChanged OnPersonalQTEChanged;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "BS|Job")
	EJobType Job = EJobType::None;

	void SetJob(EJobType NewJob) { Job = NewJob; }
	void AddQTECount();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_PersonalQTECount();
};
