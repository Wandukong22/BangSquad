// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/UI/Base/BaseMenu.h"
#include "MainMenu.generated.h"

class UJoinMenu;
class UButton;
class UHostMenu;
class UWidgetSwitcher;

UCLASS()
class PROJECT_BANG_SQUAD_API UMainMenu : public UBaseMenu
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MenuSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHostMenu> HostMenu;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoinMenu> JoinMenu;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> MainMenu;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HostButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;
protected:
	virtual bool Initialize() override;
public:
	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void QuitGame();

private:
	void SetButtonColorState(class UButton* InButton, bool bIsSelected);
	

};
