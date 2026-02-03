// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_Bang_Squad/Character/StageBoss/StageBossPlayerState.h"
#include "QTEWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UQTEWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void UpdateKeyCount(int32 KeyCount);

	void SetTargetPlayerState(class AStageBossPlayerState* InTargetPlayerState);

	UFUNCTION()
	void ShowWidget();

	UFUNCTION()
	void HideWidget();
	
	UFUNCTION()
	void HandleQTEStateChanged(bool bIsActive);
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KeyCountText;

private:
	bool bIsAnimating = false;
	float CurrentAnimTime = 0.f;

	const float AnimDuration = 0.2f;
	const float MaxScale = 1.5f;

	bool bIsInitialized = false;

	UPROPERTY()
	TWeakObjectPtr<AStageBossPlayerState> TargetPlayerState;

	int32 LastKeyCount = -1;
};
