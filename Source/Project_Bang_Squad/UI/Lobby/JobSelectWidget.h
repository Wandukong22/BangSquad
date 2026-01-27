// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
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
	UVerticalBox* PlayerListContainer;

	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UPlayerRow> PlayerRowClass;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SelectTitan;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SelectStriker;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SelectMage;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SelectPaladin;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Confirm;

	//직업 버튼 상태 업데이트
	void UpdateJobAvailAbility();

	//PlayerList 갱신 함수
	void UpdatePlayerList();

private:
	EJobType PendingJob = EJobType::None;

	UFUNCTION()
	void OnPickTitan();
	UFUNCTION()
	void OnPickStriker();
	UFUNCTION()
	void OnPickMage();
	UFUNCTION()
	void OnPickPaladin();
	UFUNCTION()
	void OnConfirmClicked();
};
