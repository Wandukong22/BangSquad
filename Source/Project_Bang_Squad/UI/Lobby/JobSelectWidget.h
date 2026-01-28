// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JobButton.h"
#include "Project_Bang_Squad/UI/Base/BaseMenu.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "JobSelectWidget.generated.h"

class UPlayerRow;
enum class EJobType : uint8;
class UButton;



UCLASS()
class PROJECT_BANG_SQUAD_API UJobSelectWidget : public UBaseMenu
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	UJobButton* Btn_SelectTitan;
	UPROPERTY(meta = (BindWidget))
	UJobButton* Btn_SelectStriker;
	UPROPERTY(meta = (BindWidget))
	UJobButton* Btn_SelectMage;
	UPROPERTY(meta = (BindWidget))
	UJobButton* Btn_SelectPaladin;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Confirm;

	void UpdateJobAvailAbility();
private:
	EJobType PendingJob = EJobType::None;
	
	UFUNCTION()
	void OnJobButtonClicked(EJobType SelectedJob);
	
	UFUNCTION()
	void OnConfirmClicked();
};
