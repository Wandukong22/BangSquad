// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Project_Bang_Squad/UI/PlayerRow.h"
#include "StageMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UStageMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	
public:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PlayerListContainer;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UPlayerRow> PlayerRowClass;

	void UpdatePartyList();

private:
	int32 CachedPlayerCount = 0;
};
