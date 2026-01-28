// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "JobButton.generated.h"

class UButton;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJobSelectedSignature, EJobType, SelectedJob);

UCLASS()
class PROJECT_BANG_SQUAD_API UJobButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "BS|Events")
	FOnJobSelectedSignature OnJobSelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BS|Data")
	EJobType AssignedJob = EJobType::None;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Click;
	UPROPERTY(meta = (BindWidget))
	UImage* Img_JobIcon;
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* Img_Border;

	void SetSelectedState(bool bSelected);

private:
	UFUNCTION()
	void OnButtonClicked();

	void UpdateButtonIcon();
};
